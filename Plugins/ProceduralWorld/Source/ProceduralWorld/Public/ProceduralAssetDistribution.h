// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"

#include "Tile.h"  

class PROCEDURALWORLD_API ProceduralAssetDistribution
{
public:
	ProceduralAssetDistribution();
	~ProceduralAssetDistribution();

	//Function for spawning object within a tile, objectType is tree/house
	void spawnActorObject(const UTile* t); 
	

};
