// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CRSpline.h"

class PROCEDURALWORLD_API Road
{
public:
	//temp constructor to start initiate a road with a spline
	Road(const CRSpline& inSpline);
	//used to add a spline to splinePaths array that starts from the end point of previous spline (input arg should be the random endpoint in the next tile)
	void extend(const ControlPoint& endPoint);
	Road();
	~Road();

	//Spline that acts as the skeleton for the road
	TArray<CRSpline> splinePaths;

	uint32 Width;
};