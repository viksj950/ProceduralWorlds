// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"
#include "Math/Vector.h"

#include "Tile.h"  

struct Triangle
{
	//Constructor, needs center position 
	Triangle(UTile* tile,float x, float y,float edgeLength = 1);

	FVector centroid;

	FVector p0; //top
	FVector p1; //bottom right
	FVector p2; //bottom left

	FVector normal;

};

struct Point2D 
{
	double x;
	double y;
};

class PROCEDURALWORLD_API ProceduralAssetDistribution
{
public:
	ProceduralAssetDistribution();
	~ProceduralAssetDistribution();

	void spawnActorObjectsCity(UTile* t, const int32 ComponentSizeQuads, const int32 ComponentsPerProxy, const int32 GridSizeOfProxies, int32 assetCount, float spread, float scaleVar);
	//Function for spawning object within a tile, objectType is tree/house
	void spawnActorObjectsPlains(UTile* t, const int32 ComponentSizeQuads, const int32 ComponentsPerProxy, const int32 GridSizeOfProxies, int32 assetCount, float scaleVar);

	bool Intersecting(Point2D tl1, Point2D br1, Point2D tl2, Point2D br2);

	/*float getRandomVal(float min, float max);*/

	/*FVector FindSuitableLocation(UTile* t, const float min, const float max);*/
	

};
