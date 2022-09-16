// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

//World partitiin
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


#include "Modules/ModuleManager.h"
//Noise
//#include "NoiseGenerator.h" we have not implemented Value Noise for this plugin
#include "PerlinNoiseGenerator.h"

class PROCEDURALWORLD_API CreateLandscape : public IModuleInterface
{
public:
	CreateLandscape();
	~CreateLandscape();
	ALandscape* generate();
};
