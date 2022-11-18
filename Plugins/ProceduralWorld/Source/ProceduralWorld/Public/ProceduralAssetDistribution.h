// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"
#include "Math/Vector.h"

#include "Tile.h"  
#include "CRSpline.h"
#include "Road.h"

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

struct biomeAssetSettings
{
	biomeAssetSettings(FString inObjectPath, int32 inAssetCount,
		float inScaleVar, float inAngleThreshold, bool inNoCollide, float inDensity, bool inConsiderRoad) : ObjectPath{ inObjectPath },
		assetCount{ inAssetCount }, scaleVar{ inScaleVar }, angleThreshold{ inAngleThreshold}, noCollide {
		inNoCollide },
		density { inDensity }, considerRoad{ inConsiderRoad }

	{
	};

	biomeAssetSettings(const biomeAssetSettings& inSettings):ObjectPath{ inSettings.ObjectPath },
		assetCount{ inSettings.assetCount }, scaleVar{ inSettings.scaleVar }, angleThreshold{ inSettings.angleThreshold }, noCollide{
		inSettings.noCollide }, density{ inSettings.density }, considerRoad{ inSettings.considerRoad }

	{
		slateThumbnail = inSettings.slateThumbnail;
	};
	
	TSharedPtr<FAssetThumbnail> slateThumbnail{ nullptr };

	FString ObjectPath;
	int32 assetCount;
	float scaleVar;
	float angleThreshold;
	bool noCollide;
	float density;
	bool considerRoad;

	//Compare paths
	bool operator==(const biomeAssetSettings& rhs) const {

		UE_LOG(LogTemp, Warning, TEXT("Comparing this->ObjectPath %s"), *this->ObjectPath);
		UE_LOG(LogTemp, Warning, TEXT("rhs.ObjectPath: %s"), *rhs.ObjectPath);


		return(this->ObjectPath.Equals(rhs.ObjectPath));
	};
	bool operator==( TSharedPtr<biomeAssetSettings>& rhs) const {

		UE_LOG(LogTemp, Warning, TEXT("Comparing this->ObjectPath %s"), *this->ObjectPath);
		UE_LOG(LogTemp, Warning, TEXT("rhs.ObjectPath: %s"), *rhs->ObjectPath);


		return(this->ObjectPath.Equals(rhs->ObjectPath));
	};

};

//Strcut for each biome type
struct biomeAssets
{
	biomeAssets(FString biotopeName, int32 biotopeIndex) : biotopeName{ biotopeName }, biotopeIndex{ biotopeIndex } {
	};

	FString biotopeName;
	int32 biotopeIndex;
	TArray<TSharedPtr<biomeAssetSettings>> AssetSettings;
};

class PROCEDURALWORLD_API ProceduralAssetDistribution
{
public:
	ProceduralAssetDistribution();
	~ProceduralAssetDistribution();

	void spawnAssets(TArray<TSharedPtr<biomeAssets>> biomeSettings, TArray<UTile*> tiles, const int32 ComponentSizeQuads, const int32 ComponentsPerProxy, const int32 GridSizeOfProxies, 
		const TArray<ControlPoint>& inRoadCoords, const TArray<Road>& roads, const int32& landscapeScale);

	void spawnWithNoCollide(UTile* tile, const FVector& Location, const float &scaleValue, const float &density, AStaticMeshActor* MyNewActor, UStaticMesh* Mesh, int& AssetCount);

	//void spawnWithNoCollideAndRoadConsider();

	bool roadConsiderCheck(const TArray<ControlPoint>& inRoadCoords, const TArray<Road>& roads, const int32& landscapeScale, const FVector& Location);

	void spawnActorObjectsCity(UTile* t, const int32 ComponentSizeQuads, const int32 ComponentsPerProxy, const int32 GridSizeOfProxies, int32 assetCount, float spread, float scaleVar, const TArray<ControlPoint>& inRoadCoords, const int& roadWidth);
	//Function for spawning object within a tile, objectType is tree/house
	void spawnActorObjectsPlains(UTile* t, const int32 ComponentSizeQuads, const int32 ComponentsPerProxy, const int32 GridSizeOfProxies, int32 assetCount, float scaleVar, const TArray<ControlPoint>& inRoadCoords, const int& roadWidth, bool withGrass);

	void spawnActorObjectsMountains(UTile* t, const int32 ComponentSizeQuads, const int32 ComponentsPerProxy, const int32 GridSizeOfProxies, int32 assetCount, float scaleVar);

	bool Intersecting(Point2D tl1, Point2D br1, Point2D tl2, Point2D br2);

	TArray<TWeakObjectPtr<AStaticMeshActor>> culledAssets;

	/*float getRandomVal(float min, float max);*/

	/*FVector FindSuitableLocation(UTile* t, const float min, const float max);*/
	

};
