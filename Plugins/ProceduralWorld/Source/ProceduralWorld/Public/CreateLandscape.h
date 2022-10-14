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
#include "ValueNoiseGenerator.h"

class PROCEDURALWORLD_API CreateLandscape : public IModuleInterface
{
public:
	CreateLandscape(int32 inSizeX, int32 inSizeY, int32 inQuadsPerComponent, int32 inComponentPerProxy, int32 inSectionsPerComponent, int32 inTileSize);
	~CreateLandscape();

	//Assign data from heightData to a tile, given a startVert for the tile
	int32 assignDataToTile(UTile* inTile, int32 startVert, int32 inSizeX, int32 inQuadsPerComponent);
	void assignDataToAllTiles(TArray<UTile*> &inTiles, int32 startVert, int32 inSizeX, int32 inQuadsPerComponent, int32 ComponentsPerProxy);

	//Test function for assigning value noise to city tiles, more a proof of concept
	void generateCityNoise();

	//Returns a column of heightdata, the column is index from left to right starting at index 0
	TArray<uint16> GetColumnOfHeightData(const TArray<uint16>& inData, int32 sizeOfSquare, int32 Column);
	TArray<uint16> GetRowOfHeightData(const TArray<uint16>& inData, int32 sizeOfSquare, int32 Row);

	void SetRowHeightData(TArray<uint16>& inData, const TArray<uint16>& inRowData, int32 sizeOfSquare, int32 Row);
	//Returns the index of a vertex's height, given coordinates
	uint32 GetVertexIndex(const TArray<uint16> &inData,int32 dataDimension, int32 inX, int32 inY);

	void concatHeightData(const TArray<UTile*> &inTiles, TArray<uint16>& data);

	void lerpAllAdjTiles(TArray<UTile*>& inTiles);

	//For now creating Perlin Noise and assigning it to the internal variable heightData asdwell as divide it among tiles
	void PreProcessNoise(TArray<UTile*> &inTiles, int const heightScale, int const octaveCount, float Amplitude, float persistence, float frequency, float lacunarity);
	ALandscape* generate();
	ALandscape* generateFromTileData(TArray<UTile*> &inTiles);

	const uint32 GetGridSizeOfProxies() const;



	TArray<uint16> heightData;
	TArray<uint16> cityHeightData;

	int32 SizeX;
	int32 SizeY;
	int32 QuadsPerComponent;
	int32 ComponentsPerProxy;
	int32 SectionsPerComponent;
	int32 TileSize;

	
private:

	uint32 gridSizeOfProxies{ 0 };
};
