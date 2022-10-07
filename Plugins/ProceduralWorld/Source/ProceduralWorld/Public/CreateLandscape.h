// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

//World partition
#include "LandscapeStreamingProxy.h"
#include "LandscapeSubsystem.h"
#include "Landscape.h"
#include "Engine/World.h"
#include "LandscapeInfo.h"
#include "UObject/UObjectGlobals.h"


//misc
#include "Engine/Selection.h"
#include "LevelEditor.h"
#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Modules/ModuleManager.h"

//Tile system
#include"Tile.h" 

//Noise
#include "PerlinNoiseGenerator.h"


class PROCEDURALWORLD_API CreateLandscape : public IModuleInterface
{
public:
	CreateLandscape(int32 inSizeX, int32 inSizeY, int32 inQuadsPerComponent, int32 inComponentPerProxy, int32 inSectionsPerComponent);
	~CreateLandscape();

	//Assign data from heightData to a tile, given a startVert for the tile
	int32 assignDataToTile(UTile* inTile, int32 startVert, int32 inSizeX, int32 inQuadsPerComponent);
	void assignDataToAllTiles(TArray<UTile*> &inTiles, int32 startVert, int32 inSizeX, int32 inQuadsPerComponent, int32 ComponentsPerProxy);

	//Returns a column of heightdata, the column is index from left to right starting at index 0
	TArray<uint16> GetColumnOfHeightData(const TArray<uint16>& inData, int32 sizeOfSquare, int32 Column);

	TArray<uint16> concatHeightData(const TArray<UTile*> &inTiles);

	//For now creating Perlin Noise and assigning it to the internal variable heightData asdwell as divide it among tiles
	void PreProcessNoise(TArray<UTile*> &inTiles);
	ALandscape* generate();
	ALandscape* generateFromTileData(TArray<UTile*> &inTiles);

	const uint32 GetGridSizeOfProxies() const;



	TArray<uint16> heightData;

	int32 SizeX;
	int32 SizeY;
	int32 QuadsPerComponent;
	int32 ComponentsPerProxy;
	int32 SectionsPerComponent;

	
private:

	uint32 gridSizeOfProxies{ 0 };
};
