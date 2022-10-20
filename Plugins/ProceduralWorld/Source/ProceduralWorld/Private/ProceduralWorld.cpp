// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProceduralWorld.h"
#include "ProceduralWorldStyle.h"
#include "ProceduralWorldCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"


//We added this
#include "Widgets/Input/SButton.h" //sbutton for UI
#include "CreateLandscape.h" //includes setup for landscape properties



static const FName ProceduralWorldTabName("ProceduralWorld");

#define LOCTEXT_NAMESPACE "FProceduralWorldModule"

void FProceduralWorldModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FProceduralWorldStyle::Initialize();
	FProceduralWorldStyle::ReloadTextures();

	FProceduralWorldCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FProceduralWorldCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FProceduralWorldModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FProceduralWorldModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ProceduralWorldTabName, FOnSpawnTab::CreateRaw(this, &FProceduralWorldModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FProceduralWorldTabTitle", "ProceduralWorld"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FProceduralWorldModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	//DClean up landscape modules (tiles/landscapePtr)
	//DeleteLandscape();

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FProceduralWorldStyle::Shutdown();

	FProceduralWorldCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ProceduralWorldTabName);
}

TSharedRef<SDockTab> FProceduralWorldModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	//FText WidgetText = FText::Format(
	//	LOCTEXT("WindowWidgetText", "Generate landscape")
	//	);

	//CreateLandscape boomboom;

	//return SNew(SDockTab)
	//	.TabRole(ETabRole::NomadTab)
	//	[
	//		// Put your tab content here!
	//		//myButt;
	//		
	//		SNew(SBox)
	//		.HAlign(HAlign_Center)
	//		.VAlign(VAlign_Center)
	//		[
	//			SNew(SButton)
	//			.Text(WidgetText)
	//			.OnClicked_Raw(this, &FProceduralWorldModule::Setup)
	//		]
	//
	//	];
	

	
	


	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SHorizontalBox)	//Main Horizontal box, now dividing setting on one box and generation/listinging/deletion in the other
			+ SHorizontalBox::Slot()
		[
			SNew(SVerticalBox)	//Vertical box to store rows(Horizontal boxes) with text / settings
			+ SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.AutoWidth()
		.MaxWidth(150)
		.Padding(0)
		.FillWidth(1.0f)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[

			SNew(STextBlock)
			.Text(FText::FromString("HeightScale"))



		]

	+ SHorizontalBox::Slot()
		.MaxWidth(150)

		.Padding(5.0f)
		.FillWidth(1.0f)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[
			SNew(SNumericEntryBox<int32>)
			.AllowSpin(true)
		.MinValue(1)
		.MaxValue(4096)
		.MaxSliderValue(4096)
		.MinDesiredValueWidth(2)
		.Value_Raw(this, &FProceduralWorldModule::GetHeightScale)
		.OnValueChanged_Raw(this, &FProceduralWorldModule::SetHeightScale)
		]


		]
	+ SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.AutoWidth()
		.MaxWidth(150)
		.Padding(0)
		.FillWidth(1.0f)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[

			SNew(STextBlock)
			.Text(FText::FromString("Octaves"))



		]

	+ SHorizontalBox::Slot()
		.AutoWidth()
		.MaxWidth(150)
		[
			SNew(SNumericEntryBox<int32>)
			.AllowSpin(true)
		.MinValue(1)
		.MaxValue(16)
		.MaxSliderValue(5)
		.MinDesiredValueWidth(2)
		.Value_Raw(this, &FProceduralWorldModule::GetOctaveCount)
		.OnValueChanged_Raw(this, &FProceduralWorldModule::SetOctaveCount)
		]


		]
	+ SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.AutoWidth()
		.MaxWidth(150)
		.Padding(0)
		.FillWidth(1.0f)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[

			SNew(STextBlock)
			.Text(FText::FromString("Amplitude"))



		]

	+ SHorizontalBox::Slot()
		.AutoWidth()
		.MaxWidth(150)
		[
			SNew(SNumericEntryBox<float>)
			.AllowSpin(true)
		.MinValue(0)
		.MaxValue(12)
		.MaxSliderValue(5)
		.MinDesiredValueWidth(2)
		.Value_Raw(this, &FProceduralWorldModule::GetAmplitude)
		.OnValueChanged_Raw(this, &FProceduralWorldModule::SetAmplitude)
		]


		]

	+ SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.AutoWidth()
		.MaxWidth(150)
		.Padding(0)
		.FillWidth(1.0f)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[

			SNew(STextBlock)
			.Text(FText::FromString("Persistence"))



		]

	+ SHorizontalBox::Slot()
		.AutoWidth()
		.MaxWidth(150)
		[
			SNew(SNumericEntryBox<float>)
			.AllowSpin(true)
		.MinValue(0)
		.MaxValue(1)
		.MaxSliderValue(1)
		.MinDesiredValueWidth(2)
		.Value_Raw(this, &FProceduralWorldModule::GetPersistence)
		.OnValueChanged_Raw(this, &FProceduralWorldModule::SetPersistence)
		]


		]
	+ SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.AutoWidth()
		.MaxWidth(150)
		.Padding(0)
		.FillWidth(1.0f)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[

			SNew(STextBlock)
			.Text(FText::FromString("Frequency"))



		]

	+ SHorizontalBox::Slot()
		.AutoWidth()
		.MaxWidth(150)
		[
			SNew(SNumericEntryBox<float>)
			.AllowSpin(true)
		.MinValue(0)
		.MaxValue(1)
		.MaxSliderValue(1)
		.MinDesiredValueWidth(2)
		.Value_Raw(this, &FProceduralWorldModule::GetFrequency)
		.OnValueChanged_Raw(this, &FProceduralWorldModule::SetFrequency)
		]


		]

	+ SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.AutoWidth()
		.MaxWidth(150)
		.Padding(0)
		.FillWidth(1.0f)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[

			SNew(STextBlock)
			.Text(FText::FromString("Lacunarity"))



		]

	+ SHorizontalBox::Slot()
		.AutoWidth()
		.MaxWidth(150)
		[
			SNew(SNumericEntryBox<float>)
			.AllowSpin(true)
		.MinValue(1)
		.MaxValue(4)
		.MaxSliderValue(16)
		.MinDesiredValueWidth(2)
		.Value_Raw(this, &FProceduralWorldModule::GetLacunarity)
		.OnValueChanged_Raw(this, &FProceduralWorldModule::SetLacunarity)
		]


		]
	+ SVerticalBox::Slot()
		.MaxHeight(100)
		[

			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.AutoWidth()
		.MaxWidth(150)
		.Padding(0)
		.FillWidth(1.0f)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[

			SNew(STextBlock)
			.Text(FText::FromString("Landscape dimensions (OBS only 1 component per proxy is working)"))



		]
	+ SHorizontalBox::Slot()
		[
			SNew(SComboBox<TSharedPtr<LandscapeSetting>>)
			.OptionsSource(&LandscapeComboSettings)
		.OnGenerateWidget_Lambda([](TSharedPtr<LandscapeSetting> Item)
			{
				return SNew(STextBlock).Text(FText::FromString(*Item->Description));
				//return SNew(SButton).Text(FText::FromString(*Item->Description));
			})
		.OnSelectionChanged_Lambda([this](TSharedPtr<LandscapeSetting> InSelection, ESelectInfo::Type InSelectInfo)
			{
				if (InSelection.IsValid() && ComboBoxTitleBlock.IsValid())
				{
					ComboBoxTitleBlock->SetText(FText::FromString(*InSelection->Description));
					this->SetLandscapeSettings(InSelection);

				}

			})
				[
					SAssignNew(ComboBoxTitleBlock, STextBlock).Text(LOCTEXT("ComboLabel", "Label"))
				]


		]

		]

		]


	+ SHorizontalBox::Slot()
		[

			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
		.Padding(1.0f)
		.HAlign(HAlign_Center)
		[
			SNew(SBox)
			.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SButton)
			.Text(FText::FromString("Generate landscape"))
		.OnClicked_Raw(this, &FProceduralWorldModule::Setup)
		]
		]

	+ SVerticalBox::Slot()
		[
			SNew(SBox)
			.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SButton)
			.Text(FText::FromString("List tiles"))
		.OnClicked_Raw(this, &FProceduralWorldModule::ListTiles)
		]
		]

	+ SVerticalBox::Slot()
		[
			SNew(SBox)
			.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SButton)
			.Text(FText::FromString("Delete Landscape"))
		.OnClicked_Raw(this, &FProceduralWorldModule::DeleteLandscape)
		]
		]
		]





		];
}

FReply FProceduralWorldModule::Setup()
{
	//Call to CreateLandscape and generate its properties 
	CreateLandscape myLand(SizeX,SizeY,QuadsPerComponent,ComponentsPerProxy,SectionsPerComponent,TileSize);
	
	//DO THIS BETTER----------------
	int32 nmbrOfTilesInARow = (SizeX -1) / (QuadsPerComponent * ComponentsPerProxy);

	//tiles.Init(new UTile(QuadsPerComponent, ComponentsPerProxy), nmbrOfTilesInARow * nmbrOfTilesInARow);

	for (size_t i{0}; i < nmbrOfTilesInARow*nmbrOfTilesInARow; i++)
	{

		UTile* temp = new UTile(QuadsPerComponent, ComponentsPerProxy, TileSize);
		temp->index = i;
		tiles.Add(temp);
	}

	for (size_t i = 0; i < tiles.Num(); i++)
	{

		tiles[i]->updateAdjacentTiles(tiles, nmbrOfTilesInARow);

	}

	tiles[9]->biotope = 0;
	tiles[10]->biotope = 0;
	//tiles[5]->biotope = 2;
	//tiles[6]->biotope = 2;
	//tiles[17]->biotope = 0;
	//tiles[18]->biotope = 0;
	//tiles[19]->biotope = 0;
	myLand.generateCityNoise();
	//Generate Perlin Noise and assign it to all tiles
	myLand.PreProcessNoise(tiles,heightScale,octaveCount,amplitude,persistence,frequency,lacunarity);

	/*tiles[9]->tileHeightData.Empty();
	tiles[9]->tileHeightData.Init(32500,64*64);*/
	
	myLand.interpAllAdjTiles(tiles, 5);

	//myLand.GetRowOfHeightData(tiles[9]->tileHeightData,64,0);
	//UE_LOG(LogTemp, Warning, TEXT("Row of data from Tile (index 9): %d"), myLand.GetRowOfHeightData(tiles[9]->tileHeightData, 64, 0).Num());


	//Concatinate heightData from all tiles and spawn a landscape
	landscapePtr = myLand.generateFromTileData(tiles);
	ULandscapeInfo* LandscapeInfo = landscapePtr->GetLandscapeInfo();

	ProceduralAssetDistribution temp;
	int32 assetCountF = 5;
	int32 assetCountC = 15;
	float scaleVarF = 0.7;
	float scaleVarC = 0.2;
	
	//after the landscape has been spawned assign proxies to each tile
	size_t i{ 0 };
	for (auto& it: LandscapeInfo->Proxies)
	{
		tiles[i]->streamingProxy = it;
		if (tiles[i]->biotope == 0)
		{
			temp.spawnActorObjectsCity(tiles[i], QuadsPerComponent, ComponentsPerProxy, myLand.GetGridSizeOfProxies(), assetCountC, scaleVarC);
			tiles[i]->updateMaterial(LoadObject<UMaterial>(nullptr, TEXT("Material'/Game/Test_assets/M_gravelMaterial.M_gravelMaterial'")));
			/*tiles[i]->updateMaterial(LoadObject<UMaterial>(nullptr, TEXT("Material'/Game/Test_assets/M_grassMaterial.M_grassMaterial'")));*/
		}
		else if(tiles[i]->biotope == 1)
		{
			//temp.spawnActorObjectsForest(tiles[i], QuadsPerComponent, ComponentsPerProxy, myLand.GetGridSizeOfProxies(), assetCountF, scaleVarF);
			tiles[i]->updateMaterial(LoadObject<UMaterial>(nullptr, TEXT("Material'/Game/Test_assets/M_gravelMaterial.M_gravelMaterial'")));
		}
		else {

		}
		
		i++;
	}

	//Place actors in the Landscape, (Foliage is the focus for now)
	//ProceduralAssetDistribution temp;
	//int32 assetCount = 5;
	//float scaleVar = 0.5;
	//temp.spawnActorObjects(tiles, QuadsPerComponent, ComponentsPerProxy, myLand.GetGridSizeOfProxies(), assetCount, scaleVar);
	
	return FReply::Handled();
}

FReply FProceduralWorldModule::ListTiles()
{
	UE_LOG(LogTemp, Warning, TEXT("heightScale %d"), SizeX);
	UE_LOG(LogTemp, Warning, TEXT("SizeY: %d"), SizeY);
	UE_LOG(LogTemp, Warning, TEXT("QuadsPerComponent: %d"), QuadsPerComponent);
	UE_LOG(LogTemp, Warning, TEXT("ComponentsPerProxy: %d"), ComponentsPerProxy);
	UE_LOG(LogTemp, Warning, TEXT("SectionsPerComponent: %d"), SectionsPerComponent);



	UE_LOG(LogTemp, Warning, TEXT("heightScale %d"), heightScale);
	UE_LOG(LogTemp, Warning, TEXT("octaveCount: %d"), octaveCount);
	UE_LOG(LogTemp, Warning, TEXT("Amplitude: %f"), amplitude);
	UE_LOG(LogTemp, Warning, TEXT("persistence: %f"), persistence);
	UE_LOG(LogTemp, Warning, TEXT("frequency: %f"), frequency);
	UE_LOG(LogTemp, Warning, TEXT("Lacuanarity: %f"), lacunarity);

	return FReply::Handled();
}

FReply FProceduralWorldModule::DeleteLandscape()
{

	UE_LOG(LogTemp, Warning, TEXT("Removal of tiles was called, number of tiles to remove:  %d"), tiles.Num());

	for (auto& it : tiles) {
		bool isDestroyed{ false };
		if (it) {
			if (it->streamingProxy.IsValid()) {
				//auto name = it->streamingProxy.Pin();
				//UE_LOG(LogTemp, Warning, TEXT("The proxy is not null, pointing to: %s"), *it->streamingProxy->GetName());
				isDestroyed = it->streamingProxy->Destroy();

			}

			for (auto& i: it->tileAssets )
			{
				if (i.IsValid())
				{
					i->Destroy();
				}
			}
			it->tileAssets.Empty();


		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Tile to be removed is null, nothing was done"));
		}

	}
	tiles.Empty();
	landscapePtr->Destroy();
	landscapePtr = nullptr;
	//delete landscapePtr;
	

	return FReply::Handled();
	
}
//LandscapeEditInterface.cpp ///Line 600
void FProceduralWorldModule::GetHeightMapData(ULandscapeInfo* inLandscapeInfo, const int32 X1, const int32 Y1, const int32 X2, const int32 Y2, TArray<uint16>& StoreData, UTexture2D* InHeightmap)
{
	int32 ComponentIndexX1, ComponentIndexY1, ComponentIndexX2, ComponentIndexY2;
	int32 ComponentSizeQuads = inLandscapeInfo->ComponentSizeQuads;
	int32 SubsectionSizeQuads = inLandscapeInfo->SubsectionSizeQuads;
	int32 ComponentNumSubsections = inLandscapeInfo->ComponentNumSubsections;

	ALandscape::CalcComponentIndicesNoOverlap(X1, Y1, X2, Y2, ComponentSizeQuads, ComponentIndexX1, ComponentIndexY1, ComponentIndexX2, ComponentIndexY2);


	for (int32 ComponentIndexY = ComponentIndexY1; ComponentIndexY <= ComponentIndexY2; ComponentIndexY++)
	{
		for (int32 ComponentIndexX = ComponentIndexX1; ComponentIndexX <= ComponentIndexX2; ComponentIndexX++)
		{
			ULandscapeComponent* Component = inLandscapeInfo->XYtoComponentMap.FindRef(FIntPoint(ComponentIndexX, ComponentIndexY));
			
			if (Component == nullptr)
			{
				continue;
			}

			UTexture2D* Heightmap = InHeightmap != nullptr ? InHeightmap : Component->GetHeightmap(true);

			if (Heightmap == nullptr)
			{
				continue;
			}
			
			FLandscapeTextureDataInfo* TexDataInfo = NULL;
			
			FColor* HeightmapTextureData = NULL;
			TexDataInfo = GetTextureDataInfo(Heightmap);
			HeightmapTextureData = (FColor*)TexDataInfo->GetMipData(0);

			//UE_LOG(LogTemp, Warning, TEXT("HeghtMapTeurextiData (Red): %d"), HeightmapTextureData->R);

			// Find coordinates of box that lies inside component
			int32 ComponentX1 = FMath::Clamp<int32>(X1 - ComponentIndexX * ComponentSizeQuads, 0, ComponentSizeQuads);
			int32 ComponentY1 = FMath::Clamp<int32>(Y1 - ComponentIndexY * ComponentSizeQuads, 0, ComponentSizeQuads);
			int32 ComponentX2 = FMath::Clamp<int32>(X2 - ComponentIndexX * ComponentSizeQuads, 0, ComponentSizeQuads);
			int32 ComponentY2 = FMath::Clamp<int32>(Y2 - ComponentIndexY * ComponentSizeQuads, 0, ComponentSizeQuads);

			// Find subsection range for this box
			int32 SubIndexX1 = FMath::Clamp<int32>((ComponentX1 - 1) / SubsectionSizeQuads, 0, ComponentNumSubsections - 1);	// -1 because we need to pick up vertices shared between subsections
			int32 SubIndexY1 = FMath::Clamp<int32>((ComponentY1 - 1) / SubsectionSizeQuads, 0, ComponentNumSubsections - 1);
			int32 SubIndexX2 = FMath::Clamp<int32>(ComponentX2 / SubsectionSizeQuads, 0, ComponentNumSubsections - 1);
			int32 SubIndexY2 = FMath::Clamp<int32>(ComponentY2 / SubsectionSizeQuads, 0, ComponentNumSubsections - 1);


			for (int32 SubIndexY = SubIndexY1; SubIndexY <= SubIndexY2; SubIndexY++)
			{
				for (int32 SubIndexX = SubIndexX1; SubIndexX <= SubIndexX2; SubIndexX++)
				{
					//UE_LOG(LogTemp, Warning, TEXT("WE GOT HERE"));
					// Find coordinates of box that lies inside subsection
					int32 SubX1 = FMath::Clamp<int32>(ComponentX1 - SubsectionSizeQuads * SubIndexX, 0, SubsectionSizeQuads);
					int32 SubY1 = FMath::Clamp<int32>(ComponentY1 - SubsectionSizeQuads * SubIndexY, 0, SubsectionSizeQuads);
					int32 SubX2 = FMath::Clamp<int32>(ComponentX2 - SubsectionSizeQuads * SubIndexX, 0, SubsectionSizeQuads);
					int32 SubY2 = FMath::Clamp<int32>(ComponentY2 - SubsectionSizeQuads * SubIndexY, 0, SubsectionSizeQuads);

					// Update texture data for the box that lies inside subsection
					for (int32 SubY = SubY1; SubY <= SubY2; SubY++)
					{
						for (int32 SubX = SubX1; SubX <= SubX2; SubX++)
						{
							
							int32 LandscapeX = SubIndexX * SubsectionSizeQuads + ComponentIndexX * ComponentSizeQuads + SubX;
							int32 LandscapeY = SubIndexY * SubsectionSizeQuads + ComponentIndexY * ComponentSizeQuads + SubY;

							// Find the texture data corresponding to this vertex
							int32 SizeU = Heightmap->Source.GetSizeX();
							int32 SizeV = Heightmap->Source.GetSizeY();
							int32 HeightmapOffsetX = Component->HeightmapScaleBias.Z * (float)SizeU;
							int32 HeightmapOffsetY = Component->HeightmapScaleBias.W * (float)SizeV;

							int32 TexX = HeightmapOffsetX + (SubsectionSizeQuads + 1) * SubIndexX + SubX;
							int32 TexY = HeightmapOffsetY + (SubsectionSizeQuads + 1) * SubIndexY + SubY;
							FColor& TexData = HeightmapTextureData[TexX + TexY * SizeU];

							uint16 Height = (((uint16)TexData.R) << 8) | TexData.G;

							StoreData.Add(Height);
							//StoreData.Store(LandscapeX, LandscapeY, Height);
							
							uint16 Normals = (((uint16)TexData.B) << 8) | TexData.A;
							FVector temp;
							temp.X = LandscapeX;
							temp.Y = LandscapeY;
							temp.Z = Normals;
							//UE_LOG(LogTemp, Warning, TEXT("Normals: %s"), *temp.ToString());
							/*if (NormalData)
							{
								uint16 Normals = (((uint16)TexData.B) << 8) | TexData.A;
								NormalData->Store(LandscapeX, LandscapeY, Normals);
							}*/
						}
					}
				}
			}




		}
	}

}
void FProceduralWorldModule::createTextureFromArray(const int32 SrcWidth, const int32 SrcHeight, TArray<FColor> inData)
{
	// Texture Information
	int width = SrcWidth;
	int height = SrcHeight;
	uint8* pixels = (uint8*)malloc(height * width * 4); // x4 because it's RGBA. 4 integers, one for Red, one for Green, one for Blue, one for Alpha

	 //filling the pixels with dummy data (4 boxes: red, green, blue and white)
	//for (int y = 0; y < height; y++)
	//{
	//	for (int x = 0; x < width; x++)
	//	{
	//		if (x < width / 2) {
	//			if (y < height / 2) {
	//				pixels[y * 4 * width + x * 4 + 0] = 255; // R
	//				pixels[y * 4 * width + x * 4 + 1] = 0;   // G
	//				pixels[y * 4 * width + x * 4 + 2] = 0;   // B
	//				pixels[y * 4 * width + x * 4 + 3] = 255; // A
	//			}
	//			else {
	//				pixels[y * 4 * width + x * 4 + 0] = 0;   // R
	//				pixels[y * 4 * width + x * 4 + 1] = 255; // G
	//				pixels[y * 4 * width + x * 4 + 2] = 0;   // B
	//				pixels[y * 4 * width + x * 4 + 3] = 255; // A
	//			}
	//		}
	//		else {
	//			if (y < height / 2) {
	//				pixels[y * 4 * width + x * 4 + 0] = 0;   // R
	//				pixels[y * 4 * width + x * 4 + 1] = 0;   // G
	//				pixels[y * 4 * width + x * 4 + 2] = 255; // B
	//				pixels[y * 4 * width + x * 4 + 3] = 255; // A
	//			}
	//			else {
	//				pixels[y * 4 * width + x * 4 + 0] = 255; // R
	//				pixels[y * 4 * width + x * 4 + 1] = 255; // G
	//				pixels[y * 4 * width + x * 4 + 2] = 255; // B
	//				pixels[y * 4 * width + x * 4 + 3] = 255; // A
	//			}
	//		}
	//	}
	//}
	int counter{ 0 };
	//uint8 t = 65536;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)  
		{
	
					pixels[y * 4 * width + x * 4 + 0] = inData[counter].R; // R
					pixels[y * 4 * width + x * 4 + 1] = inData[counter].G;  // G
					pixels[y * 4 * width + x * 4 + 2] = inData[counter].B;   // B
					pixels[y * 4 * width + x * 4 + 3] = 255;				  // A
					counter++;
			
		}
	}

	
	
		
	

	FString PackageName = TEXT("/Game/Content/");
	PackageName += "test_texture";
	UPackage* Package = CreatePackage(NULL, *PackageName);
	Package->FullyLoad();

	UTexture2D* Texture = NewObject<UTexture2D>(Package, "test_texture", RF_Public | RF_Standalone | RF_MarkAsRootSet);

	// Texture Settings
	Texture->PlatformData = new FTexturePlatformData();
	Texture->PlatformData->SizeX = width;
	Texture->PlatformData->SizeY = height;
	//Texture->PlatformData->PixelFormat = PF_R8G8B8A8;
	Texture->PlatformData->PixelFormat = PF_A16B16G16R16;

	// Passing the pixels information to the texture
	FTexture2DMipMap* Mip = new(Texture->PlatformData->Mips) FTexture2DMipMap();
	Mip->SizeX = width;
	Mip->SizeY = height;
	Mip->BulkData.Lock(LOCK_READ_WRITE);
	uint8* TextureData = (uint8*)Mip->BulkData.Realloc(height * width * sizeof(uint8) * 4);
	FMemory::Memcpy(TextureData, pixels, sizeof(uint8) * height * width * 4);
	Mip->BulkData.Unlock();

	Texture->Source.Init(SrcWidth, SrcHeight, 1, 1, ETextureSourceFormat::TSF_RGBA16, pixels);

	UE_LOG(LogTemp, Log, TEXT("Texture created:"));

	free(pixels);
	pixels = NULL;

}
void FProceduralWorldModule::createTextureFromArray(const int32 SrcWidth, const int32 SrcHeight, TArray64<uint8> inData)
{
	// Texture Information
	int width = SrcWidth;
	int height = SrcHeight;
	uint8* pixels = (uint8*)malloc(height * width * 4); // x4 because it's RGBA. 4 integers, one for Red, one for Green, one for Blue, one for Alpha
	
	int counter{ 0 };
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{

			pixels[y * 4 * width + x * 4 + 0] = inData[y * 4 * width + x * 4 + 0]; // R
			pixels[y * 4 * width + x * 4 + 1] = inData[y * 4 * width + x * 4 + 1];  // G
			pixels[y * 4 * width + x * 4 + 2] = inData[y * 4 * width + x * 4 + 2];   // B
			pixels[y * 4 * width + x * 4 + 3] = 255;				  // A
			counter++;

		}
	}






	FString PackageName = TEXT("/Game/Content/");
	PackageName += "test_texture_2";
	UPackage* Package = CreatePackage(NULL, *PackageName);
	Package->FullyLoad();

	UTexture2D* Texture = NewObject<UTexture2D>(Package, "test_texture_2", RF_Public | RF_Standalone | RF_MarkAsRootSet);

	// Texture Settings
	Texture->PlatformData = new FTexturePlatformData();
	Texture->PlatformData->SizeX = width;
	Texture->PlatformData->SizeY = height;
	//Texture->PlatformData->PixelFormat = PF_R8G8B8A8;
	Texture->PlatformData->PixelFormat = PF_A16B16G16R16;

	// Passing the pixels information to the texture
	FTexture2DMipMap* Mip = new(Texture->PlatformData->Mips) FTexture2DMipMap();
	Mip->SizeX = width;
	Mip->SizeY = height;
	Mip->BulkData.Lock(LOCK_READ_WRITE);
	uint8* TextureData = (uint8*)Mip->BulkData.Realloc(height * width * sizeof(uint8) * 4);
	FMemory::Memcpy(TextureData, pixels, sizeof(uint8) * height * width * 4);
	Mip->BulkData.Unlock();

	Texture->Source.Init(SrcWidth, SrcHeight, 1, 1, ETextureSourceFormat::TSF_RGBA16, pixels);

	UE_LOG(LogTemp, Log, TEXT("Texture created:"));

	free(pixels);
	pixels = NULL;
}
FLandscapeTextureDataInfo* FProceduralWorldModule::GetTextureDataInfo(UTexture2D* Texture)
{
	FLandscapeTextureDataInfo* Result = TextureDataMap.FindRef(Texture);
	if (!Result)
	{
		Result = TextureDataMap.Add(Texture, new FLandscapeTextureDataInfo(Texture, GetShouldDirtyPackage()));
	}
	return Result;
	
}
void FProceduralWorldModule::PluginButtonClicked()
{

	//UI settings for Landscape resolution
	LandscapeComboSettings.Empty();
	LandscapeComboSettings.Add(MakeShareable(new LandscapeSetting("505 x 505 : 63 1 63x63 64(8x8)",505,505,63,1,1,64)));
	LandscapeComboSettings.Add(MakeShareable(new LandscapeSetting("505 x 505 63 : 4(2x2) 126x126 16(4x4)", 505, 505, 63, 2, 1, 127)));
	LandscapeComboSettings.Add(MakeShareable(new LandscapeSetting("1009 x 1009 : 63 : 1 : 63x63 256(16x16)", 1009, 1009, 63, 1, 1, 64)));
	LandscapeComboSettings.Add(MakeShareable(new LandscapeSetting("1009 x 1009 : 63 : 4(2x2) : 126x126 256(16x16)", 1009, 1009, 63, 2, 1, 127)));
	LandscapeComboSettings.Add(MakeShareable(new LandscapeSetting("2017 x 2017 : 63 : 4(2x2) : 126x126 256(16x16)", 2017, 2017, 63, 2, 1, 127)));
	LandscapeComboSettings.Add(MakeShareable(new LandscapeSetting("(CRASHES) 4033 x 4033 : 63 : 4(2x2) : 126x126 1024(32x32)", 4033, 4033, 63, 2, 1, 127)));

	LandscapeComboSettings.Add(MakeShareable(new LandscapeSetting("(CRASHES) 8129 x 8129 : 127 : 4(2x2) : 254x254 1024(32x32)", 8129, 8129, 127, 2, 1, 255)));
	FGlobalTabmanager::Get()->TryInvokeTab(ProceduralWorldTabName);
}


TOptional<int32> FProceduralWorldModule::GetNumberOfTiles()
{
	
	return tiles.Num();
}

void FProceduralWorldModule::SetSizeOfLandscape(int32 inSize)
{
	sizeOfLandscape = inSize;
	UE_LOG(LogTemp, Warning, TEXT("Changed size using the UI button of the landscape to: %d"),sizeOfLandscape);

}

TOptional<int32> FProceduralWorldModule::GetSizeOfLandscape() const
{
	return sizeOfLandscape;
}

void FProceduralWorldModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FProceduralWorldCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FProceduralWorldCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FProceduralWorldModule, ProceduralWorld)