// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Landscape.h"
#include "Containers/Array.h"
#include "LandscapeProxy.h"
#include "LandscapeInfo.h"

#include "Tile.h"


class FToolBarBuilder;
class FMenuBuilder;

class FProceduralWorldModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	FReply Setup();
	FReply ListTiles();
	FReply DeleteLandscape();

	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();

	//member variable for landscape we are creating   
	
	ALandscape* landscapePtr{nullptr};

	
	/*TArray<UTile*> tiles;*/
	/*UPROPERTY()*/
	TArray<TObjectPtr<UTile>> tiles;
	
private:

	void RegisterMenus();

	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

private:
	TSharedPtr<class FUICommandList> PluginCommands;
};
