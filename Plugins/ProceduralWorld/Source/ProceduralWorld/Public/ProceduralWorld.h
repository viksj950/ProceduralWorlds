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
#include "CreateLandscape.h" //includes setup for landscape properties
#include "CRSpline.h"
#include "Road.h"
#include "S2DPreviewWindow.h" //class for constructing our 2D previes along with functions


#include "AssetRegistry/AssetRegistryModule.h"
//utility to create UTexture2D
#include "ImageUtils.h"

#include "NoiseFunctions.h"

//Slate things (UI)
#include "Widgets/SWidget.h"
#include "SlateFwd.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Misc/Optional.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Input/SComboButton.h"
#include "Types/SlateEnums.h"
#include "Widgets/Input/SSegmentedControl.h"

#include "PropertyEditorModule.h"
#include "PropertyCustomizationHelpers.h"

#include "Brushes/SlateImageBrush.h"
#include "Widgets/SCanvas.h"

//Ouput/input
#include "HAL/PlatformFileManager.h"
#include "Misc/FileHelper.h"
#include <string>
#include <iostream>
#include <fstream>


//#include "AssetThumbnail.h"


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

	FReply Setup(); //old generate landscape

	FReply GenerateTerrainData();
	FReply GenerateTerrainDataNoInterp();
	FReply GenerateTerrain();
	FReply GenerateTerrainNoInterp();
	FReply PlaceAssets();
	FReply ListTiles();
	FReply DeleteLandscape();
	FReply DeleteTerrain();//old
	FReply DeleteAssets();

	void GetHeightMapData(ULandscapeInfo* inLandscapeInfo,const int32 X1, const int32 Y1, const int32 X2, const int32 Y2, TArray<uint16>& StoreData, UTexture2D* InHeightmap);
	void createTextureFromArray(const int32 SrcWidth, const int32 SrcHeight, TArray<FColor> inData);
	void createTextureFromArray(const int32 SrcWidth, const int32 SrcHeight, TArray< uint16 > inData);
	
	FLandscapeTextureDataInfo* GetTextureDataInfo(UTexture2D* Texture);
	bool GetShouldDirtyPackage() const { return bShouldDirtyPackage; }

	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();

	//member variable for landscape we are creating   
	
	ALandscape* landscapePtr{nullptr};
	CreateLandscape* ptrToTerrain{nullptr};
	TMap<UTexture2D*, FLandscapeTextureDataInfo*> TextureDataMap;
	bool bShouldDirtyPackage;
	/*UPROPERTY()*/
	TArray<UTile*> tiles;
	TArray<Road> roads;
	
	int32 SizeX{505};
	int32 SizeY{505};
	int32 QuadsPerComponent{63};
	int32 ComponentsPerProxy{1};
	int32 SectionsPerComponent{1};
	int32 TileSize{ 64 };
	
	//UI STUFF ---------------------------------------------------------------------------------------

	struct LandscapeSetting
	{
		LandscapeSetting(const FString &inDesc,const int32 &inSizeX,const int32 &inSizeY,const int32 & inQuadsPerComponent, const int32 &inComponentsPerProxy,
			const int32 &inSectionsPerComponent, const int32 &inTileSize) : 
			Description{inDesc}, SizeX{inSizeX}, SizeY{inSizeY}, QuadsPerComponent{inQuadsPerComponent}, ComponentsPerProxy{inComponentsPerProxy}, SectionsPerComponent{inSectionsPerComponent,},
			TileSize{inTileSize}
		{

		};
		FString Description;
		int32 SizeX;
		int32 SizeY;
		int32 QuadsPerComponent;
		int32 ComponentsPerProxy;
		int32 SectionsPerComponent;
		int32 TileSize;

	};

	TArray< TSharedPtr< FString > > ComboItems;

	TArray<TSharedPtr<LandscapeSetting>> LandscapeComboSettings= { MakeShareable(new LandscapeSetting("505 x 505 : 63 1 63x63 64(8x8)",505,505,63,1,1,64)),
	MakeShareable(new LandscapeSetting("505 x 505 63 : 4(2x2) 126x126 16(4x4)", 505, 505, 63, 2, 1, 127)),
	MakeShareable(new LandscapeSetting("1009 x 1009 : 63 : 1 : 63x63 256(16x16)", 1009, 1009, 63, 1, 1, 64)),
	MakeShareable(new LandscapeSetting("1009 x 1009 : 63 : 4(2x2) : 126x126 256(16x16)", 1009, 1009, 63, 2, 1, 127)),
	MakeShareable(new LandscapeSetting("2017 x 2017 : 63 : 4(2x2) : 126x126 256(16x16)", 2017, 2017, 63, 2, 1, 127)),
	MakeShareable(new LandscapeSetting("(CRASHES) 4033 x 4033 : 63 : 4(2x2) : 126x126 1024(32x32)", 4033, 4033, 63, 2, 1, 127)),
	MakeShareable(new LandscapeSetting("(CRASHES) 8129 x 8129 : 127 : 4(2x2) : 254x254 1024(32x32)", 8129, 8129, 127, 2, 1, 255)) };

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
		TileSize = inSettings->TileSize;

		//Update settings for 2D Preview when the landscape dimensions are changed.
		previewWindow.UpdateLandscapeSettings(inSettings->SizeX,inSettings->SizeY,inSettings->QuadsPerComponent,inSettings->ComponentsPerProxy,inSettings->SectionsPerComponent,inSettings->TileSize);

	};

	
	int heightScale{ 2050 }; //4192
	int octaveCount{  1 };

	float amplitude{ 1.0f };
	float persistence{ 0.5f }; //Higher gives larger amplitudes of peaks and valleys 
	float frequency{ 0.0015f };
	float lacunarity{ 1.0f }; //Higher gives more frequent holes and hills
	
	

	void SetSizeOfLandscape(int32 inSize);
	TOptional<int32> GetSizeOfLandscape() const;

	TArray<TSharedPtr<BiotopePerlinNoiseSetting>> BiotopeSettings = { MakeShareable(new BiotopePerlinNoiseSetting("City",0,64,"Material'/Game/Test_assets/M_Landscape_City.M_Landscape_City'",4096,3,0.17f,0.5f,0.015f,1.0f,false,false,false,0)) ,
		MakeShareable(new BiotopePerlinNoiseSetting("Plains",1,64,"Material'/Game/Test_assets/M_Landscape_Plains.M_Landscape_Plains'" ,4096,3,0.25f,0.5f,0.015f,1.0f,false,false,false,0)) ,
		MakeShareable(new BiotopePerlinNoiseSetting("Mountains",2,64,"Material'/Game/Test_assets/M_Default_Landscape_Material.M_Default_Landscape_Material'", 4096,12,7.0f,0.5f,0.0015f,2.0f,true,false,false,0))};
	

	uint32 BiotopePerlinNoiseSettingIndexer{ 3 };

	//helper func for saving biomes
	FString boolToString(const bool &inBool) {

		if (inBool) {
			return "1";
		}
		return "0";
	};
	bool stringToBool(const std::string inString) {

		if (inString.compare("0") == 0) {
			return false;
		}
		return true;
	};


	FReply saveAllBiomeSettings2() {

		FString filePath = "C:/Users/William/Documents/Examensarbete/Rapport things/TEMP2.txt";
		FString filePathPre = "C:/Users/William/Documents/Examensarbete/Rapport things/TEMP3.txt";

		FString Line = "";
		FString LinePre = "";
		FString Delimiter = "\n";

		for (auto& it : previewWindow.markedTiles) {
			LinePre += FString::FromInt(it.Key);
			LinePre += Delimiter;
			LinePre += FString::FromInt(it.Value);
			LinePre += Delimiter;
			/*UE_LOG(LogTemp, Warning, TEXT("markedTiles data : %d"), it.Key);
			UE_LOG(LogTemp, Warning, TEXT("markedTiles Value : %d"), it.Value);*/
		}
		if (!previewWindow.markedTilesVoronoi.IsEmpty()) {
			LinePre += "Voronoi marked tiles";
			LinePre += Delimiter;
			for (auto& it : previewWindow.markedTilesVoronoi) {
				LinePre += FString::FromInt(it.Key);
				LinePre += Delimiter;
				LinePre += FString::FromInt(it.Value);
				LinePre += Delimiter;
				/*UE_LOG(LogTemp, Warning, TEXT("voronoiTILE data : %d"), it.Key);
				UE_LOG(LogTemp, Warning, TEXT("voronoiTILE Value : %d"), it.Value);*/
			}
		}

		for (int i = 0; i < BiotopeSettings.Num(); i++) {

			Line += BiotopeSettings[i]->Biotope + Delimiter;
			Line += FString::FromInt(BiotopeSettings[i].Get()->OctaveCount);
			Line += Delimiter;
			Line += FString::SanitizeFloat(BiotopeSettings[i].Get()->Amplitude);
			Line += Delimiter;
			Line += FString::SanitizeFloat(BiotopeSettings[i].Get()->Persistence);
			Line += Delimiter;
			Line += FString::SanitizeFloat(BiotopeSettings[i].Get()->Frequency);
			Line += Delimiter;
			Line += FString::SanitizeFloat(BiotopeSettings[i].Get()->Lacunarity);
			Line += Delimiter;
			Line += boolToString(BiotopeSettings[i].Get()->Turbulence);
			Line += Delimiter;
			Line += boolToString(BiotopeSettings[i].Get()->cutOff);
			Line += Delimiter;
			Line += boolToString(BiotopeSettings[i].Get()->invCutOff);
			Line += Delimiter;
			Line += FString::FromInt(BiotopeSettings[i].Get()->Seed);
			Line += Delimiter;

			Line += "Asset_Settings:";
			Line += Delimiter;
			for (int index = 0; index < BiomeAssetsData[i]->AssetSettings.Num(); index++) {

				Line += BiomeAssetsData[i]->AssetSettings[index]->ObjectPath;
				Line += Delimiter;
				Line += FString::FromInt(BiomeAssetsData[i].Get()->AssetSettings[index]->assetCount);
				Line += Delimiter;
				Line += FString::SanitizeFloat(BiomeAssetsData[i].Get()->AssetSettings[index]->scaleVar);
				Line += Delimiter;
				Line += FString::SanitizeFloat(BiomeAssetsData[i].Get()->AssetSettings[index]->angleThreshold);
				Line += Delimiter;
				Line += boolToString(BiomeAssetsData[i].Get()->AssetSettings[index]->noCollide);
				Line += Delimiter;
				Line += FString::SanitizeFloat(BiomeAssetsData[i].Get()->AssetSettings[index]->density);
				Line += Delimiter;
				Line += boolToString(BiomeAssetsData[i].Get()->AssetSettings[index]->considerRoad);
				Line += Delimiter;
				Line += FString::FromInt(BiomeAssetsData[i].Get()->AssetSettings[index]->heightThreshold);
				Line += Delimiter;
				Line += "xxxxx";
				Line += Delimiter;
			}
			Line += "-----";
			Line += Delimiter;
		}

		if (FFileHelper::SaveStringToFile(Line, *filePath)) {
			UE_LOG(LogTemp, Warning, TEXT("Save to file SUCCEDED"));
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Save to file FAILED (wrong path OR file is read-only?"));
		}
		if (FFileHelper::SaveStringToFile(LinePre, *filePathPre)) {
			UE_LOG(LogTemp, Warning, TEXT("Save to file SUCCEDED preview"));
		}

		return FReply::Handled();
	};

	FReply fetchAllBiomeSettings2() { //spagethios codeos

		std::fstream myFile;
		std::fstream myFilePre;
		std::string line;
		std::string linePre;
		TArray<std::string> biomeInfo;
		TArray<std::string> previewInfo;
		TArray<int> newBiomeIndexes;

		myFile.open("C:/Users/William/Documents/Examensarbete/Rapport things/TEMP2.txt", std::ios::in);
		int fileLength = 0;

		newBiomeIndexes.Add(0);

		if (myFile.is_open()) {
			while (std::getline(myFile, line)) {
				biomeInfo.Add(line);
				fileLength++;
				if (line.compare("-----") == 0) {
					newBiomeIndexes.Add(fileLength);
				}

			}

		}

		if (!biomeInfo.IsEmpty()) {
			//newBiomeIndexes.RemoveAt(newBiomeIndexes.Num() - 1); //Remove eof instance

			for (int i = 0; i < BiotopeSettings.Num(); i++) {

				BiotopeSettings.Empty();
				BiomeAssetsData.Empty();
				BiotopePerlinNoiseSettingIndexer = 0;

			}

			FString copyOfName = "";
			FString tempMat = "";

			TArray<TSharedPtr<biomeAssetSettings>> AssetSettings;
			int startPos = 0; //line row where new biome starts
			int index = 0;
			int k = 0;

			while (k < fileLength) {
				startPos = k;
				copyOfName = (biomeInfo[startPos].c_str()); //fetch first line which will always be the first biome name
				BiotopeSettings.Add(MakeShareable(new BiotopePerlinNoiseSetting(copyOfName, BiotopePerlinNoiseSettingIndexer, 64,
					tempMat, 4096, std::stoi(biomeInfo[startPos + 1]),
					std::stof(biomeInfo[startPos + 2]), std::stof(biomeInfo[startPos + 3]), std::stof(biomeInfo[startPos + 4]),
					std::stof(biomeInfo[startPos + 5]), stringToBool(biomeInfo[startPos + 6]), stringToBool(biomeInfo[startPos + 7]),
					stringToBool(biomeInfo[startPos + 8]), std::stoi(biomeInfo[startPos + 9]))));

					FString hasAssets = biomeInfo[startPos + 11].c_str();

				if (!hasAssets.Equals("-----")) { //Biome has asset settings

					AssetSettings.Empty();

					FString objPath = biomeInfo[startPos + 11].c_str();
					
					AssetSettings.Add(MakeShareable((new biomeAssetSettings(objPath, (int32)std::stoi(biomeInfo[startPos + 12]), std::stof(biomeInfo[startPos + 13]), std::stof(biomeInfo[startPos + 14]), stringToBool(biomeInfo[startPos + 15]),
						std::stof(biomeInfo[startPos + 16]), stringToBool(biomeInfo[startPos + 17]), std::stof(biomeInfo[startPos + 18])))));
					
					FString ass_separator = biomeInfo[startPos + 19].c_str();

					if (ass_separator.Equals("xxxxx")) { //new entry of asset exists
						startPos += 19;
					}

					while (ass_separator.Equals("xxxxx") && !hasAssets.Equals("-----")) { // go through all remaining asset entries

						objPath = biomeInfo[startPos + 1].c_str();

						AssetSettings.Add(MakeShareable((new biomeAssetSettings(objPath, (int32)std::stoi(biomeInfo[startPos + 2]), std::stof(biomeInfo[startPos + 3]), std::stof(biomeInfo[startPos + 4]), stringToBool(biomeInfo[startPos + 5]),
							std::stof(biomeInfo[startPos + 6]), stringToBool(biomeInfo[startPos + 7]), std::stof(biomeInfo[startPos + 8])))));

						startPos += 9;
						ass_separator = biomeInfo[startPos].c_str();
						hasAssets = biomeInfo[startPos + 1].c_str();

					}
				
					BiomeAssetsData.Add(MakeShareable(new biomeAssets(copyOfName, BiomeAssetsData.Num(), AssetSettings)));
					
				}
				else { //Imported biome has no asset settings, just add empty data
					BiomeAssetsData.Add(MakeShareable(new biomeAssets(copyOfName, BiomeAssetsData.Num())));
				
				}
				
				BiotopePerlinNoiseSettingIndexer++;
				index++;
				k = newBiomeIndexes[index];
				if (k == newBiomeIndexes.Last()) {
					break;
				}


			}
			

		}
		myFile.close();


		//Here starts the preview read
		myFile.open("C:/Users/William/Documents/Examensarbete/Rapport things/TEMP3.txt", std::ios::in);
		fileLength = 0;
		bool voronoiMarked = false;
		previewWindow.markedTiles.Empty();
		previewWindow.markedTilesVoronoi.Empty();
		if (myFile.is_open()) {
			while (std::getline(myFile, linePre)) {
				previewInfo.Add(linePre);
				fileLength++;

			}

		}
		
		for (size_t i = 0; i < fileLength-1; i += 2)
		{
			
			FString tileIndx = previewInfo[i].c_str();
			FString tileBiome = previewInfo[i+1].c_str();

		
			
			if (previewInfo[i].compare("Voronoi marked tiles") == 0) {
				voronoiMarked = true;
				//continue;
				
			}

			if (!voronoiMarked) {
	
				previewWindow.markedTiles.Add(FCString::Atoi(*tileIndx), FCString::Atoi(*tileBiome));
			}
			else {
				
					tileIndx = previewInfo[i+1].c_str();
					tileBiome = previewInfo[i + 2].c_str();
	
				previewWindow.markedTilesVoronoi.Add(FCString::Atoi(*tileIndx), FCString::Atoi(*tileBiome));
			}
		}


		myFile.close();
		return FReply::Handled();
	};

	FText newBiomeName;
	FReply addNewBiotope() {
		BiotopeSettings.Add(MakeShareable(new BiotopePerlinNoiseSetting(newBiomeName.ToString(), BiotopePerlinNoiseSettingIndexer, 64,
			BiotopeSettings[BiomeSettingSelection]->MaterialPath, BiotopeSettings[BiomeSettingSelection]->HeightScale, BiotopeSettings[BiomeSettingSelection]->OctaveCount,
			BiotopeSettings[BiomeSettingSelection]->Amplitude, BiotopeSettings[BiomeSettingSelection]->Persistence, BiotopeSettings[BiomeSettingSelection]->Frequency,
			BiotopeSettings[BiomeSettingSelection]->Lacunarity, BiotopeSettings[BiomeSettingSelection]->Turbulence, BiotopeSettings[BiomeSettingSelection]->cutOff, 
			BiotopeSettings[BiomeSettingSelection]->invCutOff, BiotopeSettings[BiomeSettingSelection]->Seed)));
		BiomeAssetsData.Add(MakeShareable(new biomeAssets(newBiomeName.ToString(), BiomeAssetsData.Num())));
		BiotopePerlinNoiseSettingIndexer++;
		return FReply::Handled();
	};


	FReply deleteBiotope() {

		auto megh = FMessageDialog::Open(EAppMsgType::OkCancel, FText::FromString("Do you want to DELETE the selected biotope?"));
		if (megh == EAppReturnType::Ok)
		{

			UE_LOG(LogTemp, Warning, TEXT("Deleting Biotope %s"), *BiotopeSettings[BiomeSettingSelection]->Biotope);
			FString biotopeToDelete = BiotopeSettings[BiomeSettingSelection]->Biotope;


			//2D 
			for (auto it = previewWindow.markedTiles.CreateConstIterator(); it; ++it)
			{
				if (it.Value() == BiotopeSettings[BiomeSettingSelection]->BiotopeIndex)
				{
					previewWindow.markedTiles.Remove(it.Key());
				}

			}
			for (auto it = previewWindow.markedTilesVoronoi.CreateConstIterator(); it; ++it)
			{
				if (it.Value() == BiotopeSettings[BiomeSettingSelection]->BiotopeIndex)
				{
					previewWindow.markedTilesVoronoi.Remove(it.Key());
				}

			}


			//Remove noise settings for the biotope and change the selection aswell
			BiotopeSettings.RemoveAt(BiomeSettingSelection);
			BiomeSettingSelection = 0;

			//Remove asset placement for the deleted biotope
			for (size_t i = 0; i < BiomeAssetsData.Num(); i++)
			{
				if (biotopeToDelete.Equals(BiomeAssetsData[i]->biotopeName))
				{
					BiomeAssetsData.RemoveAt(i);
					break;
				}
			}

			//BiomeAssetsData.RemoveAt(BiomeAssetSettingSelection);





			BiomeAssetSettingSelection = 0;
			assetSettingList->RebuildList();



		}
		
		

		return FReply::Handled();
	}

	TSharedPtr<STextBlock> ComboBoxTitleBlockNoise;
	int BiomeSettingSelection{ 0 };

	int nmbrOfBiomes{ 5 };

	//Frequency
	TOptional<float> GetFrequency() const { return BiotopeSettings.IsEmpty() ?  0 : BiotopeSettings[BiomeSettingSelection]->Frequency; }
	void SetFrequency(float inFreq) { BiotopeSettings[BiomeSettingSelection]->Frequency = inFreq; }
	//Seed
	TOptional<int32> GetSeed() const {
		return BiotopeSettings.IsEmpty() ? 0 : BiotopeSettings[BiomeSettingSelection]->Seed;
	}
	void SetSeed(int32 inSeed) { BiotopeSettings[BiomeSettingSelection]->Seed = inSeed; }
	//OctaveCount
	TOptional<int32> GetOctaveCount() const { return BiotopeSettings.IsEmpty() ? 0: BiotopeSettings[BiomeSettingSelection]->OctaveCount; }
	void SetOctaveCount(int32 inOct) { BiotopeSettings[BiomeSettingSelection]->OctaveCount= inOct; }
	//Persistance
	TOptional<float> GetPersistence() const { return BiotopeSettings.IsEmpty() ? 0 : BiotopeSettings[BiomeSettingSelection]->Persistence; }
	void SetPersistence(float inPers) { BiotopeSettings[BiomeSettingSelection]->Persistence = inPers; }
	//Lacunarity
	TOptional<float> GetLacunarity() const { return BiotopeSettings.IsEmpty() ? 0 : BiotopeSettings[BiomeSettingSelection]->Lacunarity; }
	void SetLacunarity(float inLac) { BiotopeSettings[BiomeSettingSelection]->Lacunarity = inLac; }
	//Amplitude
	TOptional<float> GetAmplitude() const { return BiotopeSettings.IsEmpty() ? 0 : BiotopeSettings[BiomeSettingSelection]->Amplitude; }
	void SetAmplitude(float inAmp) { BiotopeSettings[BiomeSettingSelection]->Amplitude = inAmp;}

	TSharedPtr<SCheckBox> turbCheckbox;

	TSharedPtr<SCheckBox> cutOffCheckbox;

	TSharedPtr<SCheckBox> invCutOffCheckbox;
	

	//UI 2D INTERFACE----------------------------------------------------------------------------------------------
	
	TSharedPtr<SSegmentedControl<int32>> biotopeGenerationMode;

	UPROPERTY()
	//UTexture2D* CustomTexture;

	TSharedPtr<SBorder> previewTextureBorder;
	TSharedPtr<FSlateImageBrush> myImageBrush;

	//Some default values
	S2DPreviewWindow previewWindow{ 505,505,63,1,1,64 };

	uint32 biotopePlacementSelection{ 1 };

	//ROAD UI------------------------------------------------------------------------------------------------------

	TSharedPtr<SCheckBox> smartRoadPlacementMode;
	TSharedPtr<SCheckBox> manualRoadPlacementMode;

	//TSharedPtr<SNumericEntryBox<uint32>> roadWidthEntryBox;
	uint32 currentRoadWidth{10};

	uint32 roadSlopeThreshold{ 600 };

	uint32 deformationStrength{ 3 };
	
	//UI Asset Distribution ---------------------------------------------------------------------------------------
	 
	
	//Storing all settings for each biome regarding its assets
	TArray<TSharedPtr<biomeAssets>> BiomeAssetsData{ MakeShareable(new biomeAssets("City",0)), MakeShareable(new biomeAssets("Plains",1)),
	  MakeShareable(new biomeAssets("Mountains",2)) };

	TSharedPtr<SCheckBox> noCollCheckBox;

	TSharedPtr<SCheckBox> avoidRoadCheckBox;

	//Some intermediate placeholder used for displaying the settings for each biotop
	TSharedPtr<STextBlock> ComboBoxTitleBlockBiotopeAsset;
	int BiomeAssetSettingSelection{ 0 };

	TSharedPtr<SNumericEntryBox<float>> myDensityNumBox;
	//Intermediate setting used as a placeholder when displaying settings for assets.
	TSharedPtr<biomeAssetSettings> IntermediateBiomeAssetSetting =MakeShareable(new biomeAssetSettings("",1,0,0.5,false,1.0,false,0 ));

	//EXPERIMENTAL THUMBNAIL STUFF
	TSharedPtr<FAssetThumbnail> slateThumbnail{nullptr}; //= MakeShareable(new FAssetThumbnail());
	//TSharedRef<SWidget> thumbnailWidget;


	TSharedPtr<SListView< TSharedPtr<biomeAssetSettings>>> assetSettingList;
	TSharedPtr<SButton> addAssetButton;
	TSharedPtr<SButton> modifyAssetButton;

	
	
	//Functionality for saving asset paths when selecting static meshes
	FString storedNamePath;
	//Saving Thumbnails for the asset selection functionality in the UI (Set to store up to 50 thumbnaails)
	TSharedPtr<FAssetThumbnailPool> myAssetThumbnailPool = MakeShareable(new FAssetThumbnailPool(50));

	FReply addNewAssetSetting() {
		//BiomeAssetsData[BiomeAssetSettingSelection]->AssetSettings.Add(biomeAssetSetting("asd", 0, 0, 0, false, 0, false));
		bool exists{ false };
		for (auto& it : BiomeAssetsData[BiomeAssetSettingSelection]->AssetSettings)
		{
			if (it->ObjectPath.Equals(IntermediateBiomeAssetSetting->ObjectPath) )
			{
				exists = true;
				break;
			}
		}

		if (!exists)
		{
			BiomeAssetsData[BiomeAssetSettingSelection]->AssetSettings.Add(MakeShareable(new biomeAssetSettings(*IntermediateBiomeAssetSetting)));
			UE_LOG(LogTemp, Warning, TEXT("Tried to add a setting to biotope: %d" ),BiomeAssetSettingSelection);
			//thumbnailWidget = slateThumbnail->MakeThumbnailWidget();
			assetSettingList->RebuildList();
		
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Cant add two of the same type!"));
		}

		return FReply::Handled();
	};


	FReply modifyAssetSetting() {

		
		//assetSettingList->GetSelectedItems()[0] = MakeShareable(new biomeAssetSettings(*IntermediateBiomeAssetSetting));
		UE_LOG(LogTemp, Warning, TEXT("Tried modifying selected item:"));
		
		IntermediateBiomeAssetSetting = MakeShareable(new biomeAssetSettings(*IntermediateBiomeAssetSetting));

		modifyAssetButton->SetEnabled(false);
		addAssetButton->SetEnabled(true);
		assetSettingList->ClearSelection();
		assetSettingList->RebuildList();

			return FReply::Handled();
	};
	
	
private:

	void RegisterMenus();

	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);
	TSharedRef<class SDockTab> OnSpawnPluginAssetTab(const class FSpawnTabArgs& SpawnTabArgs);

private:
	TSharedPtr<class FUICommandList> PluginCommands;
};
