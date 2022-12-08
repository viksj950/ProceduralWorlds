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
#include "GenericPlatform/GenericPlatformMath.h"
#include "Math/Vector2D.h"
#include "math.h"
#include "Math/IntVector.h"
#include "CoreFwd.h"
#include "Containers/Map.h"
//Tile system
#include"Tile.h" 

//Spline
#include "Road.h"

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

	void GenerateAndAssignHeightData(TArray<UTile*>& inTiles, const TArray<TSharedPtr<BiotopePerlinNoiseSetting>> &BiotopeSettings);

	//Test function for assigning value noise to city tiles, more a proof of concept
	void generateCityNoise();

	//Returns a column of heightdata, the column is index from left to right starting at index 0
	TArray<uint16> GetColumnOfHeightData(const TArray<uint16>& inData, int32 sizeOfSquare, int32 Column);
	TArray<uint16> GetRowOfHeightData(const TArray<uint16>& inData, int32 sizeOfSquare, int32 Row);

	void SetRowHeightData(TArray<uint16>& inData, const TArray<uint16>& inRowData, int32 sizeOfSquare, int32 Row);
	void SetColumnHeightData(TArray<uint16>& inData, const TArray<uint16>& inColumnData, int32 sizeOfSquare, int32 Column);
	//Returns the index of a vertex's height, given coordinates
	uint32 GetVertexIndex(int32 dataDimension, int32 inX, int32 inY) const;

	void concatHeightData(const TArray<UTile*> &inTiles);

	//Perlin interpolation (smoothstep)
	void interpAllAdjTiles(TArray<UTile*>& inTiles, int32 stepAmount);

	void copyToRawConcatData();

	//Function for deforming the world height data based on road placement (brute force)
	void roadAnamarphosis(const TArray<Road>& roads,float sigma = 0.3f, int kernelSize = 9, int interpolationPadding = 0);
	//sugma func
	void generateRoadSmart(const TArray<UTile*>& inTiles, TArray<Road> &inRoads);
	//With start and end point
	void generateRoadSmarter(const TArray<UTile*>& inTiles, TArray<Road>& inRoads, FVector& start, FVector& end, int16 maxTries);

	float calcDist(const FVector& p1, const FVector& p2);

	void GetCandidates(TMap<float, ControlPoint>& candidates, CRSpline& inSpline, const float& X, const float& Y, const int32& tileSize, 
		float &oldDist, float &newDist, const FVector& end, ControlPoint& EndCP, const int32& slopeThreshold, bool &regardDist);

	bool checkBounds(const CRSpline& spline);

	int16 GetTileIndex(const int32& X, const int32& Y);

	//Main function for interpolation between biomes
	void interpBiomes(TArray<UTile*>& inTiles, int kernelSize, float sigma, int32 interpWidth, int32 passes);

	//new and improved gaussian blur 
	void interpGaussianBlur(TArray<UTile*>& inTiles, int kernelSize, float sigma, int32 interpWidth);

	//Randomly placing biotopes depending on nmbrOfBiomes and then fillin by Voronoi
	void AssignBiotopesToTiles(TArray<UTile*>& inTiles, const int &nmbrOfBiomes, const TArray<TSharedPtr<BiotopePerlinNoiseSetting>>&BiotopeSettings) const;

	//Assign biotopes by using the data where the user manually place biotopes.
	void AssignBiotopesToTiles(TArray<UTile*>& inTiles, const TMap<int32,int32>&inMarkedTiles) const;

	//For now creating Perlin Noise and assigning it to the internal variable heightData asdwell as divide it among tiles
	void GenerateHeightMapsForBiotopes(TArray<UTile*>& inTiles, const TArray<TSharedPtr<BiotopePerlinNoiseSetting>>& BiotopeSettings);
	ALandscape* generate();
	ALandscape* generateFromTileData(TArray<UTile*> &inTiles);

	FVector GetWorldCoordinates(const TArray<uint16>& inData, int32 inX, int32 inY) const;

	//Returns number of tiles in each row/column
	const uint32 GetGridSizeOfProxies() const;

	//contains the heightmap for the whole map
	TArray<uint16> concatedHeightData;
	//For 2D preview
	TArray<uint16> rawConcatData;

	//Struct for storing information of biomes that are placed in the landscape  
	struct BiomeOriginInformation {
		
		BiomeOriginInformation(int inBiomeType, FVector2D inCoordinates, float inMaxBiomeSize) : biomeType{ inBiomeType }, coordinates{ inCoordinates },
			maxBiomeSize{ inMaxBiomeSize } {};

		int biomeType;
		FVector2D coordinates;
		float maxBiomeSize;

	};

	//Struct used for storing kernal weights and coordinates, used for Gaussian interpolation
	struct kernelElement {

		kernelElement() : coords{ 0,0 }, weight{ -1 } {};

		kernelElement(float inWeight, FVector2D inCoords = FVector2D(-100, -100)) : coords{ inCoords }, weight{inWeight} {};

		FVector2D coords;
		float weight;

	};


	//Legacy heightmaps
	TArray<uint16> heightData;
	TArray<uint16> cityHeightData;
	TArray<uint16> mountainHeightData;

	int32 SizeX;
	int32 SizeY;
	int32 QuadsPerComponent;
	int32 ComponentsPerProxy;
	int32 SectionsPerComponent;
	int32 TileSize;
	FVector LandscapeScale{100,100,100};

	
private:

	uint32 gridSizeOfProxies{ 0 };
};
