// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CRSpline.h"
#include "Tile.h"

class PROCEDURALWORLD_API Road
{
public:
	//temp constructor to start initiate a road with a spline
	Road(const CRSpline& inSpline);

	Road();
	~Road();

	//used to add a spline to splinePaths array that starts from the end point of previous spline (input arg should be the random endpoint in the next tile)
	void extend(const ControlPoint& endPoint);

	void generateRoad(const TArray<UTile*> &inTiles, const uint32 &inGridSize); 
	
	//void generateRoadSmart(const TArray<UTile*>& inTiles, const uint32& inGridSize);
	
	void calcLengthsSplines();
	void vizualizeRoad(const FVector &inLandscapeScale);

	//Spline that acts as the skeleton for the road
	TArray<CRSpline> splinePaths;

	//Width for the total road width (should always be odd)
	uint32 Width = 10;
};
