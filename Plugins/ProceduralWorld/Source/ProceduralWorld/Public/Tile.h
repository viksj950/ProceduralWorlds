// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LandscapeStreamingProxy.h"


#include "UObject/UObjectGlobals.h"
#include "Containers/StaticArray.h"

#include "Tile.generated.h"


UCLASS() //uclass for reflection system 
class PROCEDURALWORLD_API UTile : public UObject
{

GENERATED_BODY()
public:
	UTile();
	UTile(TObjectPtr<ALandscapeStreamingProxy> inProxy);
	~UTile() = default;

	void updateMaterial(UMaterial* inMaterial);
	void updateAdjacentTiles(TArray<UTile> &inTiles, const uint32 gridSizeProxy);
	UPROPERTY()
	TObjectPtr<ALandscapeStreamingProxy> streamingProxy = nullptr;
	UPROPERTY()
	UMaterial* tileMaterial = nullptr;

	bool isCity = false;
	bool isRoad = false;
	//Addd more (dungeons, biotope, river, lakes)

	int32 index{0};
	
	TArray<UTile*> adjacentTiles;
	//TStaticArray<Tile*> asd[8]{ nullptr };
};
