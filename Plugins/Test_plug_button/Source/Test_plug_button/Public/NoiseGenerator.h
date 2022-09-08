// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <random>
#include <cmath> 

template<typename T>
class Vec2
{
public:
	Vec2() : x(T(0)), y(T(0)) {}
	Vec2(T xx, T yy) : x(xx), y(yy) {}
	Vec2 operator * (const T& r) const { return Vec2(x * r, y * r); }
	T x, y;
};

typedef Vec2<float> Vec2f;

template<typename T = float>
inline T lerp(const T& lo, const T& hi, const T& t)
{
	return lo * (1 - t) + hi * t;
}

inline
float smoothstep(const float& t) { return t * t * (3 - 2 * t); }

template <typename T, unsigned N>
class TEST_PLUG_BUTTON_API NoiseGenerator
{
public:
	NoiseGenerator();
	~NoiseGenerator() = default;

	void GenerateNoiseValues(int32 seed = 2016);

	float processCoord(Vec2<float> coord) const;

	T size;
	T data[N * N]{};
};

template <typename T, unsigned N>
NoiseGenerator<T, N>::NoiseGenerator() : size{N} {
	
}

template<typename T, unsigned N>
inline void NoiseGenerator<T, N>::GenerateNoiseValues(int32 seed)
{
	std::mt19937 gen(seed);
	std::uniform_real_distribution<float> distrFloat;
	auto randFloat = std::bind(distrFloat, gen);

	// create an array of random values (white noise)
	for (auto k{0}; k < (size * size); ++k) {
		data[k] = randFloat();
	}

}

template<typename T, unsigned N>
inline float NoiseGenerator<T, N>::processCoord(Vec2<float> coord) const
{
	int xi = std::floor(coord.x);
	int yi = std::floor(coord.y);

	float tx = coord.x - xi;
	float ty = coord.y - yi;

	int rx0 = xi & size; //????? Bitwise AND desnuts
	int rx1 = (rx0 + 1) & size;
	int ry0 = yi & size;
	int ry1 = (ry0 + 1) & size;

	// random values at the corners of the cell using permutation table, (size - 1) is guesswork
	const float& c00 = data[ry0 * (size - 1) + rx0];
	const float& c10 = data[ry0 * (size - 1) + rx1];
	const float& c01 = data[ry1 * (size - 1) + rx0];
	const float& c11 = data[ry1 * (size - 1) + rx1];

	// remapping of tx and ty using the Smoothstep function 
	float sx = smoothstep(tx);
	float sy = smoothstep(ty);

	// linearly interpolate values along the x axis
	float nx0 = lerp(c00, c10, sx);
	float nx1 = lerp(c01, c11, sx);

	// linearly interpolate the nx0/nx1 along they y axis
	return lerp(nx0, nx1, sy);
	
}
