// Fill out your copyright notice in the Description page of Project Settings.


#include <cmath>
#include <random>



//NoiseGeneration::NoiseGeneration(const int32 gridSize) : gridSize{gridSize}
//{
	//float r[N];

	//const int kMaxTableSize = 256;	

	//		std::mt19937 gen(2016);
	//		std::uniform_real_distribution<float> distrFloat;
	//		auto randFloat = std::bind(distrFloat, gen);

	//		for (unsigned k = 0; k < kMaxTableSize * kMaxTableSize; ++k) {
	//			r[k] = randFloat();
	//		}



//}

//NoiseGeneration::~NoiseGeneration()
//{
//}
template <typename T, unsigned N>
float NoiseGeneration<T,N>::smoothstep(const float& t) {
	return t * t * (3 - 2 * t);
}
