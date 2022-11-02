// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

//for cube placement
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"

struct ControlPoint {

	ControlPoint() : pos{ 0,0,0 } {
	};
	ControlPoint(float inX, float inY, float inZ) : pos{ inX,inY,inZ } {
	};
	FVector pos;
	
	float length = 0;
};

class PROCEDURALWORLD_API CRSpline
{
public:

	CRSpline(ControlPoint p0, ControlPoint p1, ControlPoint p2, ControlPoint p3);
	CRSpline();
	~CRSpline();

	ControlPoint GetSplinePoint(float t) const;

	//Used for agent moving, might prove unuseful
	ControlPoint GetSplineGradient(float t);

	//computes the float value for the segment length
	float calcSegmentLength(int cp_index,float stepSize);

	//calculates the normalized t-value for each segment
	float GetNormalisedOffset(float p) const;

	//assigns length to the spline
	void calcLengths();
	
	//Useful for adding singular control point at the end of spline
	void addControlPoint(const ControlPoint& cp);

	//highlights spline curve
	void visualizeSpline(const FVector &inLandscapeScale);

	float TotalLength = 0.0f;
	float alpha = 0.1f;
	float tension = 0.5f;

	TArray<TWeakObjectPtr<AStaticMeshActor>> splineActors;
	TArray<ControlPoint> points;

private:

	

	

};
