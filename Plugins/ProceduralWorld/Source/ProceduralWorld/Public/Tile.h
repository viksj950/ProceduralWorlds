// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LandscapeStreamingProxy.h"


#include "UObject/UObjectGlobals.h"
#include "Containers/StaticArray.h"
#include "UObject/WeakObjectPtrTemplates.h" //For tweaker ptr

//#include "Tile.generated.h"


//UCLASS() //uclass for reflection system 
class PROCEDURALWORLD_API UTile //: public UObject
{


public:
	UTile(int32 NumOfQuads, int32 ComponentsPerProxy, int32 inTileSize);
	UTile(TObjectPtr<ALandscapeStreamingProxy> inProxy, int32 NumOfQuads, int32 ComponentsPerProxy);
	~UTile() = default;

	void updateMaterial(UMaterial* inMaterial);
	void updateAdjacentTiles(TArray<UTile*> &inTiles, const uint32 gridSizeProxy);

	///OLD
	//UPROPERTY()
	//TObjectPtr<ALandscapeStreamingProxy> streamingProxy = nullptr;
	
	UPROPERTY()
	TWeakObjectPtr<ALandscapeStreamingProxy> streamingProxy = nullptr;

	UPROPERTY()
	UMaterial* tileMaterial = nullptr;

	TArray<TWeakObjectPtr<AStaticMeshActor>> tileAssets;

	bool isCity = false;
	bool isRoad = false;
	//Addd more (dungeons, biotope, river, lakes)

	int biotope = 1;
	//0 = city, 1 = plains, 2 = mountains

	int32 index{0};
	int32 tileSize{0};
	TArray<uint16> tileHeightData;
	TArray<UTile*> adjacentTiles;
	//TStaticArray<Tile*> asd[8]{ nullptr };
};
