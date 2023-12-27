// Fill out your copyright notice in the Description page of Project Settings.

#include "VanagandrGameInstance.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemTypes.h"
#include "UObject/Class.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

void UVanagandrGameInstance::LoginWithEOS(FString ID, FString Token, FString LoginType)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	if (!Subsystem) return;
	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	if (!Identity) return;

	FOnlineAccountCredentials Credentials;
	Credentials.Id = ID;
	Credentials.Token = Token;
	Credentials.Type = LoginType;
	if (LoginWithEOSReturnHandle.IsValid())
	{
		Identity->OnLoginCompleteDelegates->Remove(LoginWithEOSReturnHandle);
		LoginWithEOSReturnHandle.Reset();
	}
	LoginWithEOSReturnHandle = Identity->OnLoginCompleteDelegates->AddUObject(this, &UVanagandrGameInstance::LoginWithEOS_Return);
	Identity->Login(0, Credentials);
}

void UVanagandrGameInstance::LoginWithEOS_Return(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	if (!bWasSuccessful)
	{
		UKismetSystemLibrary::PrintString(this, FString("Fail: ") + Error);
	}

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	if (!Subsystem) return;
	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	if (!Identity) return;
	Identity->OnLoginCompleteDelegates->Remove(LoginWithEOSReturnHandle);
	LoginWithEOSReturnHandle.Reset();
}

FString UVanagandrGameInstance::GetPlayerUsername()
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	if (!Subsystem) return FString();
	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	if (!Identity) return FString();

	if (Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
	{
		return Identity->GetPlayerNickname(0);
	}

	return FString();
}

bool UVanagandrGameInstance::IsPlayerLoggedIn()
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	if (!Subsystem) return false;
	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	if (!Identity) return false;

	return Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn;
}

void UVanagandrGameInstance::CreateEOSSession(bool bIsDedicated, bool bIsLanServer, int32 NumberOfPublicConnections, FString SessionTitle)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	if (!Subsystem) return;
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	if (!Session) return;

	SessionSearchResults.Sessions.Empty();
	OnSessionSearchUpdate.Broadcast();

	FOnlineSessionSettings SessionSettings;
	SessionSettings.NumPublicConnections = NumberOfPublicConnections;
	SessionSettings.bIsDedicated = bIsDedicated;
	SessionSettings.bIsLANMatch = bIsLanServer;
	SessionSettings.bAllowInvites = true;
	SessionSettings.bUseLobbiesIfAvailable = false;
	SessionSettings.bUsesPresence = false;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bAllowJoinInProgress = false;
	SessionSettings.Set(SEARCH_KEYWORDS, ALL_SESSIONS, EOnlineDataAdvertisementType::ViaOnlineService);
	SessionSettings.Set(SESSION_TITLE, SessionTitle, EOnlineDataAdvertisementType::ViaOnlineService);
	SessionSettings.Set<bool>(IN_PROGRESS, false, EOnlineDataAdvertisementType::ViaOnlineService);
	SessionSettings.Set(TEAM_A_WINS, FString::FromInt(0), EOnlineDataAdvertisementType::ViaOnlineService);
	SessionSettings.Set(TEAM_B_WINS, FString::FromInt(0), EOnlineDataAdvertisementType::ViaOnlineService);
	SessionSettings.Set(GAMES_HALFTIME, FString::FromInt(6), EOnlineDataAdvertisementType::ViaOnlineService);
	Session->ClearOnCreateSessionCompleteDelegates(this);
	Session->OnCreateSessionCompleteDelegates.AddUObject(this, &UVanagandrGameInstance::OnCreateSessionCompleted);
	Session->CreateSession(0, SESSION_NAME, SessionSettings);

}

void UVanagandrGameInstance::OnCreateSessionCompleted(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		GetWorld()->ServerTravel(OpenLevelText);
	}
}

void UVanagandrGameInstance::FindSessions()
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	if (!Subsystem) return;
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	if (!Session) return;

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = false;
	SessionSearch->MaxSearchResults = 20;
	SessionSearch->QuerySettings.SearchParams.Empty();
	SessionSearchResults.Sessions.Empty();
	OnSessionSearchUpdate.Broadcast();
	Session->ClearOnFindSessionsCompleteDelegates(this);
	Session->OnFindSessionsCompleteDelegates.AddUObject(this, &UVanagandrGameInstance::OnFindSessionCompleted);
	Session->FindSessions(0, SessionSearch.ToSharedRef());
}

void UVanagandrGameInstance::OnFindSessionCompleted(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
		if (!Subsystem) return;
		IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
		if (!Session) return;


		if (SessionSearch->SearchResults.Num() > 0) // && SessionSearch->SearchResults[0].IsValid()
		{
			SessionSearchResults.Sessions.Empty();
			for (int i = 0; i < FMath::Min(10, SessionSearch->SearchResults.Num()); i++)
			{
				FSessionDetails Details;

				FString GetName;
				SessionSearch->SearchResults[i].Session.SessionSettings.Get<FString>(SESSION_TITLE, GetName);
				Details.SessionName = GetName;

				bool bIsInProgress;
				SessionSearch->SearchResults[i].Session.SessionSettings.Get<bool>(IN_PROGRESS, bIsInProgress);
				Details.bIsInProgress = bIsInProgress;

				FString TeamAWins, TeamBWins;
				SessionSearch->SearchResults[i].Session.SessionSettings.Get(TEAM_A_WINS, TeamAWins);
				SessionSearch->SearchResults[i].Session.SessionSettings.Get(TEAM_B_WINS, TeamBWins);
				Details.TeamAWins = FCString::Atoi(*TeamAWins);
				Details.TeamBWins = FCString::Atoi(*TeamBWins);

				FString GamesPerHalftime;
				SessionSearch->SearchResults[i].Session.SessionSettings.Get(GAMES_HALFTIME, GamesPerHalftime);
				Details.GamesPerHalftime = FCString::Atoi(*GamesPerHalftime);

				FString MapName;
				SessionSearch->SearchResults[i].Session.SessionSettings.Get(LOADED_MAP_NAME, MapName);
				Details.MapName = MapName;

				Details.MaxConnections = SessionSearch->SearchResults[i].Session.SessionSettings.NumPublicConnections + SessionSearch->SearchResults[i].Session.SessionSettings.NumPrivateConnections;
				Details.OpenConnections = SessionSearch->SearchResults[i].Session.NumOpenPublicConnections + SessionSearch->SearchResults[i].Session.NumOpenPrivateConnections;
				SessionSearchResults.Sessions.Add(Details);
			}
		}
		else
		{
			SessionSearchResults.Sessions.Empty();
		}
		OnSessionSearchUpdate.Broadcast();
	}
	else
	{
		PRINT(FString("Failed to search"))
	}
}

void UVanagandrGameInstance::JoinEoSSession(int Index)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	if (!Subsystem) return;
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	if (!Session) return;
	SessionSearchResults.Sessions.Empty();
	OnSessionSearchUpdate.Broadcast();
	if (SessionSearch->SearchResults.Num() > 0 && SessionSearch->SearchResults.Num() > Index)
	{
		FString SessionName;
		SessionSearch->SearchResults[Index].Session.SessionSettings.Get<FString>(SESSION_TITLE, SessionName);
		Session->ClearOnJoinSessionCompleteDelegates(this);
		Session->OnJoinSessionCompleteDelegates.AddUObject(this, &UVanagandrGameInstance::OnJoinEoSSessionCompleted);
		Session->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
	}
}

void UVanagandrGameInstance::DestroyEoSSession()
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	if (!Subsystem) return;
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	if (!Session) return;

	Session->ClearOnDestroySessionCompleteDelegates(this);
	Session->OnDestroySessionCompleteDelegates.AddUObject(this, &UVanagandrGameInstance::OnDestroySessionCompleted);
	Session->DestroySession(SESSION_NAME);
}

void UVanagandrGameInstance::OnDestroySessionCompleted(FName SessionName, bool bWasSuccessful)
{
	PRINT(FString("Destroyed Session: ") + SessionName.ToString())
}

void UVanagandrGameInstance::OnJoinEoSSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
			if (!Subsystem) return;
			IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
			if (!Session) return;

			FString JoinAddress;
			Session->GetResolvedConnectString(SessionName, JoinAddress);
			if (!JoinAddress.IsEmpty())
			{
				PRINT(FString("Join address: "))
				PlayerController->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
			}
		}
	}
	else
	{
		PRINT(FString("Couldn't join session"))
	}
}

void UVanagandrGameInstance::SetSessionInProgress(bool bIsInProgress)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	if (!Subsystem) return;
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	if (!Session) return;

	auto SettingsPtr = Session->GetSessionSettings(SESSION_NAME);
	if (!SettingsPtr) return;
	FOnlineSessionSettings Settings = *SettingsPtr;
	Settings.Set<bool>(IN_PROGRESS, bIsInProgress, EOnlineDataAdvertisementType::ViaOnlineService);
	Session->UpdateSession(SESSION_NAME, Settings);

	/*
	* I've commented out this entire block as we no longer want to stop people from joining 
	* a game in progress. I'm replacing it with some property that shows if it's in progress without
	* locking the session.
	* 
	EOnlineSessionState::Type State = Session->GetSessionState(SESSION_NAME);

	switch (State)
	{
	case EOnlineSessionState::Type::Creating:
	case EOnlineSessionState::Type::Destroying:
	case EOnlineSessionState::Type::Ended:
	case EOnlineSessionState::Type::Ending:
	case EOnlineSessionState::Type::Starting:
	case EOnlineSessionState::Type::NoSession:
		PRINT(FString("Incorrect Session State: ").Append(FString::FromInt(State)))
		break;
	case EOnlineSessionState::Type::Pending:
		if (bIsInProgress)
		{
			Session->ClearOnStartSessionCompleteDelegates(this);
			Session->OnStartSessionCompleteDelegates.AddUObject(this, &UVanagandrGameInstance::OnStartSessionCompleted);
			Session->StartSession(SESSION_NAME);
		}
		else 
		{
			PRINT(FString("Incorrect Session State: ").Append(FString::FromInt(State)))
		}
		break;
	case EOnlineSessionState::Type::InProgress:
		if (!bIsInProgress)
		{
			Session->ClearOnEndSessionCompleteDelegates(this);
			Session->OnEndSessionCompleteDelegates.AddUObject(this, &UVanagandrGameInstance::OnEndSessionCompleted);
			Session->EndSession(SESSION_NAME);
		}
		else
		{
			PRINT(FString("Incorrect Session State: ").Append(FString::FromInt(State)))
		}
		break;
	}
	*/
}

bool UVanagandrGameInstance::IsSessionInProgress()
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	if (!Subsystem) return false;
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	if (!Session) return false;

	return Session->GetSessionState(SESSION_NAME) == EOnlineSessionState::InProgress;
}

void UVanagandrGameInstance::UpdateSessionGameWins(int TeamAWins, int TeamBWins)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(GetWorld());
	if (!Subsystem) return;
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
	if (!Session) return;

	auto SettingsPtr = Session->GetSessionSettings(SESSION_NAME);
	if (!SettingsPtr) return;
	FOnlineSessionSettings Settings = *SettingsPtr;
	Settings.Set(TEAM_A_WINS, FString::FromInt(TeamAWins), EOnlineDataAdvertisementType::ViaOnlineService);
	Settings.Set(TEAM_B_WINS, FString::FromInt(TeamBWins), EOnlineDataAdvertisementType::ViaOnlineService);
	Session->UpdateSession(SESSION_NAME, Settings);
	
}

void UVanagandrGameInstance::OnEndSessionCompleted(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
		PRINT(FString("Session ended, should be able to join again"))
	else
		PRINT(FString("Failed to end session"))
}

void UVanagandrGameInstance::OnStartSessionCompleted(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
		PRINT(FString("Session started"))
	else
		PRINT(FString("Failed to start session"))
}