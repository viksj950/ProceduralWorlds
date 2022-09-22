// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <random>
#include <cmath> 
#include <functional>
#include <algorithm>
#include "Math/UnrealMathUtility.h"

//Our functions and Vec2
#include "NoiseFunctions.h"


template <typename T, unsigned N>
class TEST_PLUG_BUTTON_API NoiseGenerator
{
public:
	NoiseGenerator();
	~NoiseGenerator() = default;

	void GenerateNoiseValues(unsigned seed = 2022);

	float processCoord(Vec2<float> coord) const;

	T size;
	T data[N]{};

	unsigned permutationTable[N * 2]{};
	const unsigned kMaxTableSizeMask = (N - 1);
};

template <typename T, unsigned N>
NoiseGenerator<T, N>::NoiseGenerator() : size{ N } {
	
}

template<typename T, unsigned N>
inline void NoiseGenerator<T, N>::GenerateNoiseValues(unsigned seed)
{
	FMath mathInstance;

	float tempa = mathInstance.FRandRange(0.0f, 10.0f);
	std::mt19937 gen(seed);
	UE_LOG(LogTemp, Warning, TEXT("Random float generation (tempa): %f"), tempa);
	std::uniform_real_distribution<float> distrFloat;
	auto randFloat = std::bind(distrFloat, tempa);

	// create an array of random values (white noise)
	for (unsigned k{0}; k < size; ++k) {
		data[k] = mathInstance.FRandRange(0.0f, 10.0f);
		permutationTable[k] = k;
	}
	UE_LOG(LogTemp, Warning, TEXT("PERMUTATION TABLE VALUE: %d"), permutationTable[150]);
	
	int tempa2 = mathInstance.RandRange(0, 2147000);
	UE_LOG(LogTemp, Warning, TEXT("Random float generation (tepa2m): %d"), tempa2);
	//thisd shuffle thepeu tablerm s
	std::uniform_int_distribution<unsigned> distrUInt;
	auto randUInt = std::bind(distrUInt, gen);
	for (unsigned k = 0; k < size; ++k) {
		unsigned i = mathInstance.RandRange(0, 2147000) & kMaxTableSizeMask;
		unsigned temp;
		temp = permutationTable[k];
		permutationTable[k] = permutationTable[i];
		permutationTable[i] = temp;

		//std::swap(permutationTable[k], permutationTable[i]);
		permutationTable[k + size] = permutationTable[k];
	}

}

template<typename T, unsigned N>
inline float NoiseGenerator<T, N>::processCoord(Vec2<float> coord) const
{

	int xi = std::floor(coord.x);
	int yi = std::floor(coord.y);


	float tx = coord.x - xi;
	float ty = coord.y - yi;

	int rx0 = xi & kMaxTableSizeMask; //????? Bitwise AND desnuts
	int rx1 = (rx0 + 1) & kMaxTableSizeMask;
	int ry0 = yi & kMaxTableSizeMask;
	int ry1 = (ry0 + 1) & kMaxTableSizeMask;

	// random values at the corners of the cell using permutation table, (size - 1) is guesswork
	const float& c00 = data[permutationTable[permutationTable[rx0] + ry0]];
	const float& c10 = data[permutationTable[permutationTable[rx1] + ry0]];
	const float& c01 = data[permutationTable[permutationTable[rx0] + ry1]];
	const float& c11 = data[permutationTable[permutationTable[rx1] + ry1]];

	// remapping of tx and ty using the Smoothstep function 
	float sx = smoothstep(tx);
	float sy = smoothstep(ty);

	// linearly interpolate values along the x axis
	float nx0 = lerp(c00, c10, sx);
	float nx1 = lerp(c01, c11, sx);

	// linearly interpolate the nx0/nx1 along they y axis
	return lerp(nx0, nx1, sy);
	
}
