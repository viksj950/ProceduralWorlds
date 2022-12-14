// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Math/UnrealMathUtility.h"
#include <cmath> 

//Our functions and Vec2
#include "NoiseFunctions.h"

//template<typename T>
//class Vec2;
//
//float smoothstep(const float& t);
//
//template<typename T>
//T lerp(const T& lo, const T& hi, const T& t);
//
//float dot(const Vec2<float>& v1, const Vec2<float>& v2);

template <typename T, unsigned N>
class TEST_PLUG_BUTTON_API PerlinNoiseGenerator
{
public:
	PerlinNoiseGenerator(): size{N}{};
	~PerlinNoiseGenerator() = default;

	//Function to generate noise values from gradient
	float generateNoiseVal(const Vec2<float> p);

	//Generates random vectors and inserts them into gradients array + sets up and randiomizes pe tablerm
	void generateGradients();

	int hash(const int& x, const int& y) const;

	T size;
	static const unsigned tableSizeMask = N - 1;
	Vec2<float> gradients[N]{};
	unsigned permutationTable[N * 2]{};

};

template <typename T, unsigned N>
float PerlinNoiseGenerator<T, N>::generateNoiseVal(const Vec2<float> p) {


	//UE_LOG(LogTemp, Warning, TEXT("Value of px: %f"), p.x);
	//This casts x/y to an int, which we then bit AND opeator to be fast AF (omegalul using MOD)
	int xi0 = ((int)std::floor(p.x)) & tableSizeMask;
	int yi0 = ((int)std::floor(p.y)) & tableSizeMask;

	int xi1 = (xi0 + 1) & tableSizeMask;
	int yi1 = (yi0 + 1) & tableSizeMask;

	//Diff between int and decimal, used in smoothstep
	float tx = p.x - ((int)std::floor(p.x)); 
	float ty = p.y - ((int)std::floor(p.y));
	//UE_LOG(LogTemp, Warning, TEXT("Value of px (floored) %f"), ((int)std::floor(p.y)));
	//UE_LOG(LogTemp, Warning, TEXT("Value tx: %f"), tx);
	//UE_LOG(LogTemp, Warning, TEXT("Value ty: %f"), ty);


	float u = smoothstep(tx);
	float v = smoothstep(ty);

	//assign each coner to correonding corner, uzing hasing table
	const Vec2<float>& c00 = gradients[hash(xi0, yi0)];
	const Vec2<float>& c10 = gradients[hash(xi1, yi0)];
	const Vec2<float>& c01 = gradients[hash(xi0, yi1)];
	const Vec2<float>& c11 = gradients[hash(xi1, yi1)];
	//UE_LOG(LogTemp, Warning, TEXT("Value c00.x: %f"), c00.x); //s?g rimligt ut
	//UE_LOG(LogTemp, Warning, TEXT("Value c00.y: %f"), c00.y);

	//generate vectors going from the grid points to p
	float x0 = tx, x1 = tx - 1;
	float y0 = ty, y1 = ty - 1;

	//Arrows that points from corner to p
	Vec2<float> p00 = Vec2<float>(x0, y0);
	Vec2<float> p10 = Vec2<float>(x1, y0);
	Vec2<float> p01 = Vec2<float>(x0, y1);
	Vec2<float> p11 = Vec2<float>(x1, y1);
	//UE_LOG(LogTemp, Warning, TEXT("Value p00.x: %f"), p00.x);
	//UE_LOG(LogTemp, Warning, TEXT("Value p00.y: %f"), p00.y);

	//linear interpolation
	float a = lerp(dot(c00, p00), dot(c10, p10), u);
	float b = lerp(dot(c01, p01), dot(c11, p11), u);

	//Bilinar intepr
	return lerp(a, b, v);
	

}

template <typename T, unsigned N>
void PerlinNoiseGenerator<T,N>::generateGradients(){

	FMath mathInstance;
	float gradientLength = 0;

	for(int i = 0; i < N;  ++i ){ 
		do {	//Is this needed?
			gradients[i] = Vec2<float>(2 * mathInstance.FRandRange(0.0f, 1.0f) - 1, 2 * mathInstance.FRandRange(0.0f, 1.0f) - 1); 
			gradientLength = gradients[i].length();
		} while (gradientLength > 1);
		//UE_LOG(LogTemp, Warning, TEXT("Length before normalize :  %f"), gradients[i].length());
		gradients[i] = Vec2(gradients[i].x / gradientLength, gradients[i].y / gradientLength); //normalization
		//UE_LOG(LogTemp, Warning, TEXT("Length after normalize:  %f"), gradients[i].length());
		
		//Insert each index 0 - (N - 1) into permutation table
		permutationTable[i] = i; 
		
	}
	//Permuation table swap
	for (unsigned k = 0; k < size; ++k) {		//2147000
		unsigned i = mathInstance.RandRange(0, 2147000) & tableSizeMask; //bit matching is faster than MOD
		unsigned temp;
		temp = permutationTable[k];
		permutationTable[k] = permutationTable[i];
		permutationTable[i] = temp;

	}

	//This adds values to extend to ((N*2) -1) values
	for (unsigned i = 0; i < size; i++)
	{
		permutationTable[size + i] = permutationTable[i];
	}
}
template <typename T, unsigned N>
	int PerlinNoiseGenerator<T,N>::hash(const int& x, const int& y) const {
		return permutationTable[permutationTable[x] + y];
}





