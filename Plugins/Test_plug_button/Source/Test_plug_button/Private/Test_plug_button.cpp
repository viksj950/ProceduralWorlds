// Copyright Epic Games, Inc. All Rights Reserved.

#include "Test_plug_button.h"
#include "Test_plug_buttonStyle.h"
#include "Test_plug_buttonCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "AssetRegistrymodule.h"
#include "Engine/Selection.h"
#include "LevelEditor.h"
#include "Editor.h"
#include "Editor/EditorEngine.h"

//For world partition
#include "LandscapeStreamingProxy.h"
#include "LandscapeSubsystem.h"
#include "Landscape.h"
#include "Engine/World.h"
#include "LandscapeInfo.h"
#include "UObject/UObjectGlobals.h"

//For noise
#include "NoiseGenerator.h"
#include "PerlinNoiseGenerator.h"


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
	


	USelection* SelectedActors = nullptr;
	FText DialogText;
	
	if (GEditor->GetSelectedActors() == nullptr) {
		SelectedActors = GEditor->GetSelectedActors();
	
	}
	else {
		DialogText = FText::FromString("Landscape plugin activated, creating landscape...");
	}
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
	
	/*FText DialogText = FText::Format(
							LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
							FText::FromString(TEXT("FTest_plug_buttonModule::PluginButtonClicked()")),
							FText::FromString(TEXT("Test_plug_button.cpp"))
					   );*/

	
	//FText DialogText = FText::AsNumber(res->GetSizeX());

	FMessageDialog::Open(EAppMsgType::Ok, DialogText);

	FQuat InRotation{0,0,0,0};
	FVector InTranslation{0,0,5};
	FVector InScale{100,100,100};
	FTransform LandscapeTransform{ InRotation, InTranslation, InScale };
	int32 QuadsPerSection{63};
	int32 SectionsPerComponent{1};
	//int32 ComponentCountX{12345};
	//int32 ComponentCountY{21123};
	int32 QuadsPerComponent{63}; //deaultf is 63
	int32 SizeX{505};
	int32 SizeY{505}; //default is 505


	TArray<FLandscapeImportLayerInfo> MaterialImportLayers;
	TMap<FGuid, TArray<uint16>> HeightDataPerLayers;
	TMap<FGuid, TArray<FLandscapeImportLayerInfo>> MaterialLayerDataPerLayers;

	TArray<uint16> HeightData;
	HeightData.SetNum(SizeX * SizeY);


	PerlinNoiseGenerator<uint16, 64> PerlinNoise{};
	PerlinNoise.generateGradients();
	NoiseGenerator<uint16, 64> noise{}; //N is "cell size", 127 is tiny tiles 1009 is large tiles
	noise.GenerateNoiseValues(2016);

	int heightScale = 4192; 
	for (size_t j = 0; j < SizeY; j++)
	{
		for (size_t i = 0; i < SizeX; i++)
		{	

			HeightData[j * SizeX + i] = PerlinNoise.generateNoiseVal(Vec2<float>(i, j) * 0.015625) * heightScale + 32768;
			//HeightData[j * SizeX + i] = noise.processCoord(Vec2<float>(i, j)) * heightScale + 32768;

			if((j * SizeX + i ) == SizeX*j*8){
				//(LogTemp, Warning, TEXT("Value of heightdata: %f"), PerlinNoise.generateNoiseVal(Vec2<float>(i, j)));
			}

		}
	}

	HeightDataPerLayers.Add(FGuid(), MoveTemp(HeightData));
	MaterialLayerDataPerLayers.Add(FGuid(), MoveTemp(MaterialImportLayers));

	UWorld* World = nullptr;
	
		// We want to create the landscape in the landscape editor mode's world
		FWorldContext& EditorWorldContext = GEditor->GetEditorWorldContext();
		World = EditorWorldContext.World();

	ALandscape* Landscape = World->SpawnActor<ALandscape>(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f)); //This is working Pog

	Landscape->bCanHaveLayersContent = true;
	Landscape->LandscapeMaterial = nullptr;

	Landscape->SetActorTransform(LandscapeTransform);
	Landscape->Import(FGuid::NewGuid(),0,0,SizeX - 1, SizeY - 1,SectionsPerComponent,QuadsPerComponent,
		HeightDataPerLayers,nullptr,MaterialLayerDataPerLayers,ELandscapeImportAlphamapType::Additive); 

	Landscape->StaticLightingLOD = FMath::DivideAndRoundUp(FMath::CeilLogTwo((SizeX * SizeY) / (2048 * 2048) + 1), (uint32)2);
	// Register all the landscape components
	ULandscapeInfo* LandscapeInfo = Landscape->GetLandscapeInfo();
	
	LandscapeInfo->UpdateLayerInfoMap(Landscape);

	Landscape->RegisterAllComponents();

	// Need to explicitly call PostEditChange on the LandscapeMaterial property or the landscape proxy won't update its material
	FPropertyChangedEvent MaterialPropertyChangedEvent(FindFieldChecked< FProperty >(Landscape->GetClass(), FName("LandscapeMaterial")));
	Landscape->PostEditChangeProperty(MaterialPropertyChangedEvent);
	Landscape->PostEditChange();
	
	//Changing Gridsize which will create LandscapestreamProcies, Look at file: LandscapeEditorDetailCustomization_NewLandscape.cpp line 800
	EditorWorldContext.World()->GetSubsystem<ULandscapeSubsystem>()->ChangeGridSize(LandscapeInfo,2);


	

	
	

	

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