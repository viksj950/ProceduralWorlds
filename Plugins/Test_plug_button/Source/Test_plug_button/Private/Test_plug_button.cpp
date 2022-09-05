// Copyright Epic Games, Inc. All Rights Reserved.

#include "Test_plug_button.h"
#include "Test_plug_buttonStyle.h"
#include "Test_plug_buttonCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "AssetRegistrymodule.h"
#include "Selection.h"
#include "LevelEditor.h"
#include "Editor.h"
#include "Landscape.h"
#include "Engine/World.h"


static const FName Test_plug_buttonTabName("Test_plug_button");

#define LOCTEXT_NAMESPACE "FTest_plug_buttonModule"

void FTest_plug_buttonModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FTest_plug_buttonStyle::Initialize();
	FTest_plug_buttonStyle::ReloadTextures();

	FTest_plug_buttonCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FTest_plug_buttonCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FTest_plug_buttonModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FTest_plug_buttonModule::RegisterMenus));
}

void FTest_plug_buttonModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FTest_plug_buttonStyle::Shutdown();

	FTest_plug_buttonCommands::Unregister();
}

void FTest_plug_buttonModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here

	
	
	USelection* SelectedActors = GEditor->GetSelectedActors();
	//TArray<AActor*> Actors;
	//TArray<ULevel*> UniqueLevels;
	//for (FSelectionIterator Iter(SelectedActors->get); Iter; ++Iter)
	//{
	//	AActor Actor = Cast(*Iter);
	//	if (Actor)
	//	{
	//		Actors.Add(Actor);
	//		//UniqueLevels.AddUnique 1(Actor->GetLevel());
	//	}
	//}
	
	FText DialogText = FText::FromString(SelectedActors->GetSelectedObject(0)->GetName());
	/*FText DialogText = FText::Format(
							LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
							FText::FromString(TEXT("FTest_plug_buttonModule::PluginButtonClicked()")),
							FText::FromString(TEXT("Test_plug_button.cpp"))
					   );*/

	
	//FText DialogText = FText::AsNumber(res->GetSizeX());

	FMessageDialog::Open(EAppMsgType::Ok, DialogText);

	FQuat InRotation{0,0,0,0};
	FVector InTranslation{0,0,0};
	FVector InScale{1,1,1};
	FTransform LandscapeTransform{ InRotation, InTranslation, InScale };
	int32 QuadsPerSection{63};
	int32 SectionsPerComponent{1};
	int32 ComponentCountX{8};
	int32 ComponentCountY{8};
	int32 QuadsPerComponent{505};
	int32 SizeX{505};
	int32 SizeY{505};


	TArray<FLandscapeImportLayerInfo> MaterialImportLayers;
	TMap<FGuid, TArray<uint16>> HeightDataPerLayers;
	TMap<FGuid, TArray<FLandscapeImportLayerInfo>> MaterialLayerDataPerLayers;

	TArray<uint16> HeightData;
	HeightData.SetNum(SizeX * SizeY);
	for (int32 i = 0; i < HeightData.Num(); i++)
	{
		HeightData[i] = 32768;
	}

	HeightDataPerLayers.Add(FGuid(), MoveTemp(HeightData));
	MaterialLayerDataPerLayers.Add(FGuid(), MoveTemp(MaterialImportLayers));


	UWorld* World = nullptr;
	{
		// We want to create the landscape in the landscape editor mode's world
		FWorldContext& EditorWorldContext = GEditor->GetEditorWorldContext();
		World = EditorWorldContext.World();
	}


	//ALandscape* Landscape = World->SpawnActor<ALandscape>();

	//const std::size_t width = 32;
	//const std::size_t height = 32;

	//TArray<FColor> Pixels;
	//Pixels.Init(FColor(0, 255, 0, 255), 32 * 32);

	//FString PackageName = TEXT("/Game/Texture");
	//FString TextureName = TEXT("my_heightmap");

	//PackageName += TextureName;
	//UPackage* Package = CreatePackage(NULL, *PackageName);
	//Package->FullyLoad();

	//UTexture2D* res = NewObject<UTexture2D>(Package, *TextureName, RF_Public | RF_Standalone | RF_MarkAsRootSet);

	//res->AddToRoot();
	//res->SRGB = 0;
	//res->MipGenSettings = TMGS_NoMipmaps;

	//res->PlatformData = new FTexturePlatformData();	// Then we initialize the PlatformData
	//res->PlatformData->SizeX = width;
	//res->PlatformData->SizeY = height;
	//res->PlatformData->PixelFormat = EPixelFormat::PF_R8G8B8A8;
	//res->UpdateResource();

	//// // Allocate first mipmap.
	//FTexture2DMipMap* Mip = new FTexture2DMipMap();
	//res->PlatformData->Mips.Add(Mip);
	//Mip->SizeX = width;
	//Mip->SizeY = height;

	//Mip->BulkData.Lock(LOCK_READ_WRITE);
	//uint8* TextureData = static_cast<uint8*>(Mip->BulkData.Realloc(width * height * sizeof(FColor)));
	//FMemory::Memcpy(TextureData, Pixels.GetData(), width * height * sizeof(FColor));
	//Mip->BulkData.Unlock();

	//// Apply Texture changes to GPU memory
	//res->UpdateResource();

	//res->Source.Init(
	//	width,
	//	height,
	//	1, 1, ETextureSourceFormat::TSF_RGBA8, reinterpret_cast<uint8*>(Pixels.GetData()));

	//res->UpdateResource();
	//Package->MarkPackageDirty();
	//FAssetRegistryModule::AssetCreated(res);

	//FString PackageFileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
	//bool bSaved = UPackage::SavePackage(Package, res, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *PackageFileName, GError, nullptr, true, true, SAVE_NoError);
	//check(bSaved)


}

void FTest_plug_buttonModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FTest_plug_buttonCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FTest_plug_buttonCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTest_plug_buttonModule, Test_plug_button)