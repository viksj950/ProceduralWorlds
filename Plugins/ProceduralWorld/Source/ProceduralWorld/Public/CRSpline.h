// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct controlPoint {

	controlPoint(float inX, float inY, float inZ) : pos{ inX,inY,inZ } {
	};
	FVector pos;
	//add length later
};

class PROCEDURALWORLD_API CRSpline
{
public:


	
	CRSpline();
	~CRSpline();

	controlPoint GetSplinePoint(const float t);

	TArray<controlPoint> points;

	float alpha = 0.5f;
	float tension = 0.0f;

	controlPoint p0;
	controlPoint p1;
	controlPoint p2;
	controlPoint p3;
};
