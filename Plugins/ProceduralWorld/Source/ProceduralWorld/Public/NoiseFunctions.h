// Fill out your copyright notice in the Description page of Project Settings.


// THIS IS FOR NEW PLUGION
#pragma once

#include "CoreMinimal.h"
#include <cmath> 
#include "Math/UnrealMathUtility.h"

template<typename T = float>
class PROCEDURALWORLD_API Vec2
{
public:
	Vec2() : x(T(0)), y(T(0)) {}
	Vec2(T xx, T yy) : x(xx), y(yy) {}

	float length() {
		return sqrt(x * x + y * y);
	}
	Vec2 operator* (const T& r) const { return Vec2(x * r, y * r); }

	Vec2& operator /= (T scalar) {
		x = x / scalar;
		y = y / scalar;
		return *this;
	};

	T x, y;
};

template<typename T = float>
inline T lerp(const T& lo, const T& hi, const T& t)
{
	return lo * (1 - t) + hi * t;
}

inline
float smoothstep(const float& t) { return 6 * pow(t, 5) - (15 * pow(t, 4)) + (10 * pow(t, 3)); }  // return t * t * (3 - 2 * t);

inline
float dot(const Vec2<float>& v1, const Vec2<float>& v2) {
	return v1.x * v2.x + v1.y * v2.y;
}

struct BiotopePerlinNoiseSetting
{
	BiotopePerlinNoiseSetting(const FString inBiotope, const int inBiotopeIndex, const int inCellSize, const int inHeightScale, const int inOctaveCount, const float inAmplitude, const float inPersistance,
		const float inFrequency, const float inLacunarity) :
		Biotope{inBiotope}, BiotopeIndex{inBiotopeIndex}, CellSize{
		inCellSize
	}, HeightScale{
		inHeightScale}, OctaveCount{ inOctaveCount }, Amplitude{ inAmplitude }, Persistence{inPersistance},
		Frequency{ inFrequency }, Lacunarity{ inLacunarity } {};

	BiotopePerlinNoiseSetting(const BiotopePerlinNoiseSetting &inCopy):
		Biotope{ inCopy.Biotope }, BiotopeIndex{ inCopy.BiotopeIndex }, CellSize{ inCopy.CellSize }, HeightScale{ inCopy .HeightScale}, OctaveCount{ inCopy.OctaveCount}, Amplitude{ inCopy.Amplitude}, Persistence{ inCopy.Persistence},
			Frequency{ inCopy.Frequency }, Lacunarity{ inCopy.Lacunarity } {};
	
	FString Biotope;
	int BiotopeIndex;

	int CellSize;
	int HeightScale; //4192
	int OctaveCount;

	float Amplitude;
	float Persistence; //Higher gives larger amplitudes of peaks and valleys 
	float Frequency;
	float Lacunarity; //Higher gives more frequent holes and hills




};