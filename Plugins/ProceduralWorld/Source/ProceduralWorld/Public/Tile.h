// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LandscapeStreamingProxy.h"


#include "UObject/UObjectGlobals.h"

class PROCEDURALWORLD_API Tile
{
public:
	Tile();
	Tile(TObjectPtr<ALandscapeStreamingProxy> inProxy);
	~Tile() = default;

	void updateMaterial(UMaterial* inMaterial);


	TObjectPtr<ALandscapeStreamingProxy> streamingProxy = nullptr;
	UMaterial* tileMaterial = nullptr;

	bool isCity = false;
	bool isRoad = false;
	//Addd more (dungeons, biotope, river, lakes)

};
