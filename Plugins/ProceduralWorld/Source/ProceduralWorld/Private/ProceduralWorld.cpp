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
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
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

			+SVerticalBox::Slot()
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

			+SVerticalBox::Slot()
				[
					SNew(SBox)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					[
						SNew(SButton)
						.Text(FText::FromString("List and delete tiles"))
						.OnClicked_Raw(this, &FProceduralWorldModule::DeleteLandscape)
					]
				]

			
			
		];
}

FReply FProceduralWorldModule::Setup()
{
	//Call to CreateLandscape and generate its properties 
	CreateLandscape myLand;
	landscapePtr = myLand.generate();

	//Dynamic sizes

	//Reserve to not reallacoate during runtime. 
	tiles.Reserve(myLand.GetGridSizeOfProxies()*myLand.GetGridSizeOfProxies());

	//tiles.Init(Tile(), 16);

	ULandscapeInfo* LandscapeInfo = landscapePtr->GetLandscapeInfo();
	//TConstIterator<ALandscapeStreamingProxy> it(LandscapeInfo->Proxies.CreateConstIterator()); //LandscapeInfo->Proxies.CreateConstIterator();
	UE_LOG(LogTemp, Warning, TEXT("Num of proxies: %d"), LandscapeInfo->Proxies.Num());
	UE_LOG(LogTemp, Warning, TEXT("Num of tiles: %d"), tiles.Num());
	if (!tiles.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("tiles is not empty, attempting to solve ezpz by resizing"));
			
	}
	//tiles.Empty(); //Should not be needed, but for some reason tiles start of with a maxed out array, we must empty it.
	//UE_LOG(LogTemp, Warning, TEXT("Num of tiles after being resized: %d"), tiles.Num());

	uint32 index{ 0 };
	for (auto& it : LandscapeInfo->Proxies)
	{
	
		UTile* temp = new UTile(it);/* = NewObject<UTile>();*/
		//temp->streamingProxy = it;
		temp->index = index;
		temp->updateMaterial(LoadObject<UMaterial>(nullptr, TEXT("Material'/Game/Test_assets/M_gravelMaterial.M_gravelMaterial'")));
		tiles.Add(temp);
		index++;

	}
	UE_LOG(LogTemp, Warning, TEXT("Num of tiles after adding them: %d"), tiles.Num());

	tiles[11]->updateMaterial(LoadObject<UMaterial>(nullptr, TEXT("Material'/Game/Test_assets/M_grassMaterial.M_grassMaterial'")));
	for (size_t i = 0; i < tiles.Num(); i++)
	{
	
		tiles[i]->updateAdjacentTiles(tiles, myLand.GetGridSizeOfProxies());

	}

	ULandscapeInfo* myInfo = landscapePtr->CreateLandscapeInfo();
	TArray<uint16> myHeightData;
	GetHeightMapData(myInfo, 130, 130, 330, 330, myHeightData, nullptr);

	UE_LOG(LogTemp, Warning, TEXT("A HEIGHT VALUE (0)?: %d"), myHeightData[0]);
	
	/*FString PackageName = TEXT("/Game/Content/");
	PackageName += "asd";
	UPackage* Package = CreatePackage(NULL, *PackageName);
	Package->FullyLoad();

	UTexture2D* NewTexture = NewObject<UTexture2D>(Package, "asd", RF_Public | RF_Standalone | RF_MarkAsRootSet);*/

	//FTexture2DMipMap* MyMipMap = &Texture2D->PlatformData->Mips[0];

	//FByteBulkData* RawImageData = &MyMipMap->BulkData;

	//FColor* FormatedImageData = static_cast<FColor*>(RawImageData->Lock(LOCK_READ_ONLY));

	//RawImageData->Unlock();

	/////////////////////////
	//Try Changing the heightdata of one LandscapeComponent.
	/*TArray<FColor> mtasd;
	FColor myColor;
	myColor.A = 254;
	myColor.B = 254;
	myColor.G = 254;
	myColor.R = 254;
	mtasd.Init(myColor,64*64);
	tiles[0]->streamingProxy->LandscapeComponents[0]->InitHeightmapData(mtasd, true);
	tiles[0]->streamingProxy->LandscapeComponents[0]->RequestHeightmapUpdate(true,true);*/

	//UTexture2D* Texture2D = tiles[0]->streamingProxy->LandscapeComponents[0]->GetHeightmap();


	//Texture2D->UpdateResource();

	//// Get Platform Data.
	//const auto PlatformData = Texture2D->GetPlatformData();

	//// Get The Bulk Data
	//const auto TextureData = PlatformData->Mips[0].BulkData;

	//// Array Pixel Data Will Be Copied To
	//TArray<FColor> DestPixels{};

	//// Initialize The Array With The Correct Pixel Size;
	//DestPixels.AddDefaulted(PlatformData->SizeX * PlatformData->SizeY);

	//// Lock The Pixel Data For CPU Read Access.
	//const auto PixelData = static_cast<const FColor*>(TextureData.LockReadOnly());

	//// Copy The Pixel Data.
	//FMemory::Memcpy(DestPixels.GetData(), &PixelData, sizeof(FColor) * DestPixels.Num());

	//// Unlock So The GPU Can Access The Data Again.
	//TextureData.Unlock();

	// We Now Have All Pixel Data For The Texture And We Can Do What We Want Without Holding The GPU Up.

	//for (int i = 0; i < DestPixels.Num(); i++) {
	//	UE_LOG(LogTemp, Warning, TEXT("Value of raw data (FColor R): %d"), DestPixels[i].R);
	//	UE_LOG(LogTemp, Warning, TEXT("Value of raw data (FColor G): %d"), DestPixels[i].G);
	//	UE_LOG(LogTemp, Warning, TEXT("Value of raw data (FColor B): %d"), DestPixels[i].B);
	//	UE_LOG(LogTemp, Warning, TEXT("Value of raw data (FColor A): %d"), DestPixels[i].A);
	//}

	//

	//
	//NewTexture->AddToRoot();				// This line prevents garbage collection of the texture
	//NewTexture->PlatformData = new FTexturePlatformData();	// Then we initialize the PlatformData
	//NewTexture->PlatformData->SizeX = Texture2D->GetSizeX();
	//NewTexture->PlatformData->SizeY = Texture2D->GetSizeY();
	////NewTexture->PlatformData->NumSlices = 1;
	//NewTexture->PlatformData->PixelFormat = EPixelFormat::PF_B8G8R8A8;
	//uint8* Pixels = (uint8*)malloc(Texture2D->GetSizeY() * Texture2D->GetSizeY() * 4);
	////uint8* Pixels = new uint8[Texture2D->GetSizeX() * Texture2D->GetSizeY() * 4];
	//for (int32 y = 0; y < Texture2D->GetSizeX(); y++)
	//{
	//	for (int32 x = 0; x < Texture2D->GetSizeY(); x++)
	//	{
	//		int32 curPixelIndex = ((y * Texture2D->GetSizeX()) + x);
	//		Pixels[4 * curPixelIndex] = DestPixels[curPixelIndex].B;
	//		Pixels[4 * curPixelIndex + 1] = DestPixels[curPixelIndex].G;
	//		Pixels[4 * curPixelIndex + 2] = DestPixels[curPixelIndex].R;
	//		Pixels[4 * curPixelIndex + 3] = DestPixels[curPixelIndex].A;
	//	}
	//}


	///*FCreateTexture2DParamaters asd;
	//UTexture2D* myimage = FImageUtils::CreateTexture2D(Texture2D->GetSizeX(), Texture2D->GetSizeY(), DestPixels, Package, "asde", EObjectFlags::RF_Standalone, );*/
	//// Allocate first mipmap.
	//FTexture2DMipMap* Mip = new(NewTexture->PlatformData->Mips) FTexture2DMipMap();
	//Mip->SizeX = Texture2D->GetSizeX();
	//Mip->SizeY = Texture2D->GetSizeY();

	//// Lock the texture so it can be modified
	//Mip->BulkData.Lock(LOCK_READ_WRITE);
	//uint8* TextureData = (uint8*)Mip->BulkData.Realloc(Texture2D->GetSizeX() * Texture2D->GetSizeY() * 4);
	////FMemory::Memcpy(TextureData, Pixels, sizeof(uint8) * Texture2D->GetSizeX() * Texture2D->GetSizeY() * 4);

	//int width = Texture2D->GetSizeX();
	//int height = Texture2D->GetSizeY();

	////FMemory::Memcpy(TextureData, Pixels, sizeof(uint8) * width* height * 4);
	//Mip->BulkData.Unlock();

	//NewTexture->Source.Init(Texture2D->GetSizeX(), Texture2D->GetSizeY(), 1, 1, ETextureSourceFormat::TSF_BGRA8, Pixels);

	//NewTexture->UpdateResource();
	//Package->MarkPackageDirty();
	//FAssetRegistryModule::AssetCreated(NewTexture);

	//FString PackageFileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
	//bool bSaved = UPackage::SavePackage(Package, NewTexture, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *PackageFileName, GError, nullptr, true, true, SAVE_NoError);

	//delete[] Pixels;	// Don't forget to free the memory here

	
	//UTexture2D* NewTexture = tiles[0]->streamingProxy->LandscapeComponents[0]->GetHeightmap();
	
		/*UE_LOG(LogTemp, Warning, TEXT("LandscapeComponent has Layer Data %d"), tiles[0]->streamingProxy->LandscapeComponents[0]->GetHeightmap()->Source.GetSizeX());

		UTexture2D *namenTitta =  tiles[0]->streamingProxy->LandscapeComponents[0]->GetHeightmap();*/

		//namenTitta->Get; //boom bam
		
		//UTexture2D* namenTex = namenTitta.Texture;
		
		//TArray64< uint8 > mipasd;
		//mipasd.Init(0, tiles[0]->streamingProxy->LandscapeComponents[0]->GetHeightmap()->Source.GetSizeX() * tiles[0]->streamingProxy->LandscapeComponents[0]->GetHeightmap()->Source.GetSizeX());
		
		//tiles[0]->streamingProxy->LandscapeComponents[0]->GetHeightmap()->Source.GetMipData(mipasd,0,nullptr);

		
		//UE_LOG(LogTemp, Warning, TEXT("THIS IS THE DATA FROM SOURCE MIP %d"), mipasd[0]);

	
	
		
	
	

	//createTextureFromArray(64, 64, namenTitta);
	
	//UE_LOG(LogTemp, Warning, TEXT("DestPixels contains so many pixels: %d"),DestPixels.Num());
	return FReply::Handled();
}

FReply FProceduralWorldModule::ListTiles()
{
	UE_LOG(LogTemp, Warning, TEXT("tiles contains so many tiles %d"), tiles.Num());

	for (auto& it: tiles)
	{
		if (it->streamingProxy != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Tile has a proxy, the id is: %d"), it->index);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Tile DO NOT have a proxy, the id is: %d"), it->index);
		}


	}

	//for (auto& it: tiles)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Adjacent tile to nr 0: %s"), *it->streamingProxy->GetName());

	//}

	for (int i = 0; i < tiles[11]->adjacentTiles.Num(); i++) {
		if (tiles[11]->adjacentTiles[i] != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Adjacent tile has index : %d"), tiles[11]->adjacentTiles[i]->index);

		}else{
			UE_LOG(LogTemp, Warning, TEXT("Null tile with adjacent array index: %d"), i);
		}
		
		
	}
	/*UE_LOG(LogTemp, Warning, TEXT("tiles contains so many tiles %d"), tiles.Num());

	for (auto& it : tiles) {
		if (IsValid(it)) {
			if (it->streamingProxy.IsValid()) {

				UE_LOG(LogTemp, Warning, TEXT("Tiles contain a tile with index: %d"), it->index);
				UE_LOG(LogTemp, Warning, TEXT("The proxy is not null, pointing to: %s"), *it->streamingProxy->GetName());

			}
			else {

				UE_LOG(LogTemp, Warning, TEXT("Proxy has been destroyed or is pending destruction"));
				break;
			}

		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Tile is destroyed or about to be destroyed   "));
			break;
		}

	}*/

	return FReply::Handled();
}

FReply FProceduralWorldModule::DeleteLandscape()
{

	//UE_LOG(LogTemp, Warning, TEXT("tiles contains so many tiles %d"), tiles.Num());

	////for array traversal (index)
	//int counter = 0;

	//for(auto& it : tiles  ){
	//	if(IsValid(it)){
	//		if (it->streamingProxy.IsValid()) {

	//			UE_LOG(LogTemp, Warning, TEXT("Tiles contain a tile with index: %d"), it->index);
	//			UE_LOG(LogTemp, Warning, TEXT("The proxy is not null, pointing to: %s"), *it->streamingProxy->GetName());

	//			/*if (it->streamingProxy) {
	//				UE_LOG(LogTemp, Warning, TEXT("The proxy is not null, pointing to: %s"), *it->streamingProxy->GetName());
	//			}
	//			else {
	//				UE_LOG(LogTemp, Warning, TEXT("Proxy is null "));
	//			}*/
	//		}
	//		else {

	//			UE_LOG(LogTemp, Warning, TEXT("Proxy has been destroyed or is pending destruction"));
	//			
	//			//tiles.RemoveAt(counter, 1, true);
	//			break;
	//			
	//		}

	//	}else{
	//			UE_LOG(LogTemp, Warning, TEXT("Tile is destroyed or about to be destroyed   "));
	//			break;
	//	}
	//	

	//	counter++;
	//}
	//for (auto& it : tiles)
	//{
	//	if(it->streamingProxy == nullptr){
	//		UE_LOG(LogTemp, Warning, TEXT("Proxy is null with id %d"), it->index);
	//	}

	//}

	return FReply::Handled();

	//if (!tiles.IsEmpty() && landscapePtr != nullptr)
	//{
	//	bool isDestroyed{ false };
	//	for (auto& it : tiles)
	//	{

	//		isDestroyed = it->streamingProxy->Destroy();
	//		if (!isDestroyed)
	//		{
	//			UE_LOG(LogTemp, Warning, TEXT("Could not delete all tiles"));
	//			return FReply::Unhandled();
	//		}
	//	}
	//	tiles.Empty();
	//	landscapePtr->Destroy();
	//	//delete landscapePtr;

	//	
	//	return FReply::Handled();

	//}
	//else
	//{	
	//	UE_LOG(LogTemp, Warning, TEXT("Could not delete landscape, no landscape is being pointed"));
	//	return FReply::Unhandled();
	//}
	
}
//LandscapeEditInterface.cpp ///Line 600
void FProceduralWorldModule::GetHeightMapData(ULandscapeInfo* inLandscapeInfo, const int32 X1, const int32 Y1, const int32 X2, const int32 Y2, TArray<uint16>& StoreData, UTexture2D* InHeightmap)
{
	int32 ComponentIndexX1, ComponentIndexY1, ComponentIndexX2, ComponentIndexY2;
	int32 ComponentSizeQuads = inLandscapeInfo->ComponentSizeQuads;
	int32 SubsectionSizeQuads = inLandscapeInfo->SubsectionSizeQuads;
	int32 ComponentNumSubsections = inLandscapeInfo->ComponentNumSubsections;

	UE_LOG(LogTemp, Warning, TEXT("ComponentSizeQuads: %d"), ComponentSizeQuads);
	UE_LOG(LogTemp, Warning, TEXT("SubsectionSizeQuads: %d"), SubsectionSizeQuads);
	UE_LOG(LogTemp, Warning, TEXT("Compoentnumsetionsc: %d"), ComponentNumSubsections);

	ALandscape::CalcComponentIndicesNoOverlap(X1, Y1, X2, Y2, ComponentSizeQuads, ComponentIndexX1, ComponentIndexY1, ComponentIndexX2, ComponentIndexY2);

	UE_LOG(LogTemp, Warning, TEXT("ComponentIndexX1: %d"), ComponentIndexX1);
	UE_LOG(LogTemp, Warning, TEXT("ComponentIndexY1: %d"), ComponentIndexY1);
	UE_LOG(LogTemp, Warning, TEXT("ComponentIndexX2: %d"), ComponentIndexX2);
	UE_LOG(LogTemp, Warning, TEXT("ComponentIndexY2: %d"), ComponentIndexY2);

	for (int32 ComponentIndexY = ComponentIndexY1; ComponentIndexY <= ComponentIndexY2; ComponentIndexY++)
	{
		for (int32 ComponentIndexX = ComponentIndexX1; ComponentIndexX <= ComponentIndexX2; ComponentIndexX++)
		{
			ULandscapeComponent* Component = inLandscapeInfo->XYtoComponentMap.FindRef(FIntPoint(ComponentIndexX, ComponentIndexY));
			
			if (Component == nullptr)
			{
				continue;
			}
			/*else
			{
				UE_LOG(LogTemp, Warning, TEXT("We found a component: %s"), *Component->GetName());
			}*/

			UTexture2D* Heightmap = InHeightmap != nullptr ? InHeightmap : Component->GetHeightmap(true);

			if (Heightmap == nullptr)
			{
				continue;
			}
			//else
			//{
			//	UE_LOG(LogTemp, Warning, TEXT("We got a heightmap with sizeX: %d"), Heightmap->GetSizeX());
			//}
			


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

			UE_LOG(LogTemp, Warning, TEXT("ComponenX1 %d"), ComponentX1); //4 
			UE_LOG(LogTemp, Warning, TEXT("ComponenX2 %d"), ComponentX2); //63

			UE_LOG(LogTemp, Warning, TEXT("SubX1 %d"), SubIndexX1); ///0
			UE_LOG(LogTemp, Warning, TEXT("SubX2 %d"), SubIndexX2); //0



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
							
							//int32 LandscapeX = SubIndexX * SubsectionSizeQuads + ComponentIndexX * ComponentSizeQuads + SubX;
							//int32 LandscapeY = SubIndexY * SubsectionSizeQuads + ComponentIndexY * ComponentSizeQuads + SubY;

							// Find the texture data corresponding to this vertex
							int32 SizeU = Heightmap->Source.GetSizeX();
							int32 SizeV = Heightmap->Source.GetSizeY();
							int32 HeightmapOffsetX = Component->HeightmapScaleBias.Z * (float)SizeU;
							int32 HeightmapOffsetY = Component->HeightmapScaleBias.W * (float)SizeV;

							int32 TexX = HeightmapOffsetX + (SubsectionSizeQuads + 1) * SubIndexX + SubX;
							int32 TexY = HeightmapOffsetY + (SubsectionSizeQuads + 1) * SubIndexY + SubY;
							FColor& TexData = HeightmapTextureData[TexX + TexY * SizeU];

							UE_LOG(LogTemp, Warning, TEXT("Texdata (red): %d"), TexData.R);
							UE_LOG(LogTemp, Warning, TEXT("Texdata (green): %d"), TexData.G);
							UE_LOG(LogTemp, Warning, TEXT("Texdata (blue irrelvanet): %d"), TexData.B);


							uint16 Height = (((uint16)TexData.R) << 8) | TexData.G;

							StoreData.Add(Height);
							//StoreData.Store(LandscapeX, LandscapeY, Height);
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
	FGlobalTabmanager::Get()->TryInvokeTab(ProceduralWorldTabName);
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