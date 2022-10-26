// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

//for cube placement
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"

struct ControlPoint {

	ControlPoint() : pos{ 0,0,0} {
	};
	ControlPoint(float inX, float inY, float inZ) : pos{ inX,inY,inZ } {
	};
	FVector pos;
	float length;
};

class PROCEDURALWORLD_API CRSpline
{
public:

	CRSpline();
	~CRSpline();

	ControlPoint GetSplinePoint(float t);

	//Used for agent moving, might prove unuseful
	ControlPoint GetSplineGradient(float t);

	//computes the float value for the segment length
	float calcSegmentLength(int cp_index,float stepSize);

	//calculates the normalized t-value for each segment
	float GetNormalisedOffset(float p);

	//assigns length to the spline
	void calcLengths();
	
	//Useful for adding singular control point at the end of spline
	void addControlPoint(const ControlPoint& cp);

	//highlights spline curve
	void visualizeSpline();

	float TotalLength = 0.0f;
	float alpha = 0.1f;
	float tension = 0.5f;

private:

	ControlPoint p0;
	ControlPoint p1;
	ControlPoint p2;
	ControlPoint p3;

	TArray<ControlPoint> points;

};
