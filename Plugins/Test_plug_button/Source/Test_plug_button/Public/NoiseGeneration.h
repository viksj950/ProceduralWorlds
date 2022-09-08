// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


/**
 * 
 */
template <typename T, unsigned N> 
class NoiseGeneration //TEST_PLUG_BUTTON_API
{
public:
	NoiseGeneration(const int32 gridSize);
	float smoothstep(const float& t);
	/*~NoiseGeneration();*/

	T data[N]{};
};



#include "NoiseGeneration.cpp"
