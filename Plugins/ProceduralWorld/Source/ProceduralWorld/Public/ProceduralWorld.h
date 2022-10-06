// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Landscape.h"
#include "Containers/Array.h"
#include "LandscapeProxy.h"
#include "LandscapeInfo.h"

#include "Tile.h"
#include "ProceduralAssetDistribution.h"

#include "AssetRegistry/AssetRegistryModule.h"
//utility to create UTexture2D
#include "ImageUtils.h"



//Slate things (UI)
#include "Widgets/SWidget.h"
#include "SlateFwd.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Misc/Optional.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Input/SComboButton.h"
#include "Types/SlateEnums.h"

class FToolBarBuilder;
class FMenuBuilder;
struct FLandscapeTextureDataInfo
{
	struct FMipInfo
	{
		void* MipData;
		TArray<FUpdateTextureRegion2D> MipUpdateRegions;
		bool bFull;
	};

	FLandscapeTextureDataInfo(UTexture2D* InTexture, bool bShouldDirtyPackage) : Texture(InTexture) {
		{
			MipInfo.AddZeroed(Texture->Source.GetNumMips());
			Texture->SetFlags(RF_Transactional);
			Texture->TemporarilyDisableStreaming();
			Texture->Modify(bShouldDirtyPackage);
		}
	};



	virtual ~FLandscapeTextureDataInfo() {
		for (int32 i = 0; i < MipInfo.Num(); i++)
		{
			if (MipInfo[i].MipData)
			{
				Texture->Source.UnlockMip(i);
				MipInfo[i].MipData = NULL;
			}
		}
		Texture->ClearFlags(RF_Transactional);
	};

	// returns true if we need to block on the render thread before unlocking the mip data
	bool UpdateTextureData();

	int32 NumMips() { return MipInfo.Num(); }

	void AddMipUpdateRegion(int32 MipNum, int32 InX1, int32 InY1, int32 InX2, int32 InY2)
	{
		if (MipInfo[MipNum].bFull)
		{
			return;
		}

		check(MipNum < MipInfo.Num());
		uint32 Width = 1 + InX2 - InX1;
		uint32 Height = 1 + InY2 - InY1;
		// Catch situation where we are updating the whole texture to avoid adding redundant regions once the whole region as been included.
		if (Width == GetMipSizeX(MipNum) && Height == GetMipSizeY(MipNum))
		{
			MipInfo[MipNum].bFull = true;
			MipInfo[MipNum].MipUpdateRegions.Reset();
			// Push a full region for UpdateTextureData() to process later
			new(MipInfo[MipNum].MipUpdateRegions) FUpdateTextureRegion2D(0, 0, 0, 0, Width, Height);
			return;
		}

		new(MipInfo[MipNum].MipUpdateRegions) FUpdateTextureRegion2D(InX1, InY1, InX1, InY1, Width, Height);
	}

	void* GetMipData(int32 MipNum)
	{
		check(MipNum < MipInfo.Num());
		if (!MipInfo[MipNum].MipData)
		{
			MipInfo[MipNum].MipData = Texture->Source.LockMip(MipNum);
		}
		return MipInfo[MipNum].MipData;
	}

	int32 GetMipSizeX(int32 MipNum) const
	{
		return FMath::Max(Texture->Source.GetSizeX() >> MipNum, 1);
	}

	int32 GetMipSizeY(int32 MipNum) const
	{
		return FMath::Max(Texture->Source.GetSizeY() >> MipNum, 1);
	}

private:
	UTexture2D* Texture;
	TArray<FMipInfo> MipInfo;
};

class FProceduralWorldModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	FReply Setup();
	FReply ListTiles();
	FReply DeleteLandscape();

	void GetHeightMapData(ULandscapeInfo* inLandscapeInfo,const int32 X1, const int32 Y1, const int32 X2, const int32 Y2, TArray<uint16>& StoreData, UTexture2D* InHeightmap);
	void createTextureFromArray(const int32 SrcWidth, const int32 SrcHeight, TArray<FColor> inData);
	void createTextureFromArray(const int32 SrcWidth, const int32 SrcHeight, TArray64< uint8 > inData);
	
	FLandscapeTextureDataInfo* GetTextureDataInfo(UTexture2D* Texture);
	bool GetShouldDirtyPackage() const { return bShouldDirtyPackage; }

	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();

	//member variable for landscape we are creating   
	
	ALandscape* landscapePtr{nullptr};
	TMap<UTexture2D*, FLandscapeTextureDataInfo*> TextureDataMap;
	bool bShouldDirtyPackage;
	/*UPROPERTY()*/
	TArray<UTile*> tiles;

	
	int32 SizeX;
	int32 SizeY;
	int32 QuadsPerComponent;
	int32 ComponentsPerProxy;
	int32 SectionsPerComponent;
	
	//UI STUFF ---------------------------------------------------------------------------------------

	struct LandscapeSetting
	{
		LandscapeSetting(const FString &inDesc,const int32 &inSizeX,const int32 &inSizeY,const int32 & inQuadsPerComponent, const int32 &inComponentsPerProxy,
			const int32 &inSectionsPerComponent) : 
			Description{inDesc}, SizeX{inSizeX}, SizeY{inSizeY}, QuadsPerComponent{inQuadsPerComponent}, ComponentsPerProxy{inComponentsPerProxy}, SectionsPerComponent{inSectionsPerComponent}
		{

		};
		FString Description;
		int32 SizeX;
		int32 SizeY;
		int32 QuadsPerComponent;
		int32 ComponentsPerProxy;
		int32 SectionsPerComponent;


	};

	TArray< TSharedPtr< FString > > ComboItems;

	TArray<TSharedPtr<LandscapeSetting>> LandscapeComboSettings;




	TSharedPtr<STextBlock> ComboBoxTitleBlock;

	///Some functions for UI, maybe move this later?
	int32 sizeOfLandscape{ 0 };
	TOptional<int32> GetNumberOfTiles();

	void SetLandscapeSettings(TSharedPtr<LandscapeSetting>inSettings) {
		SizeX = inSettings->SizeX;
		SizeY = inSettings->SizeY;
		QuadsPerComponent = inSettings->QuadsPerComponent;
		ComponentsPerProxy = inSettings->ComponentsPerProxy;
		SectionsPerComponent = inSettings->SectionsPerComponent;
		UE_LOG(LogTemp, Warning, TEXT("Changed Settings for Landscape"));
	};

	void SetSizeOfLandscape(int32 inSize);
	TOptional<int32> GetSizeOfLandscape() const;
	
private:

	void RegisterMenus();

	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

private:
	TSharedPtr<class FUICommandList> PluginCommands;
};
