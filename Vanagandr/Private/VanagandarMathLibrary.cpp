// Fill out your copyright notice in the Description page of Project Settings.


#include "VanagandarMathLibrary.h"

float UVanagandarMathLibrary::InverseLerp(float a, float b, float value) {
	
	return (value - a) / (b - a);
}
