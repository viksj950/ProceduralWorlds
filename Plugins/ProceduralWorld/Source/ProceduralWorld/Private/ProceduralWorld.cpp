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


FReply FProceduralWorldModule::Setup()
{
	//Call to CreateLandscape and generate its properties 
	CreateLandscape myLand;
	landscapePtr = myLand.generate();
	
	ULandscapeInfo* LandscapeInfo = landscapePtr->GetLandscapeInfo();
	//TConstIterator<ALandscapeStreamingProxy> it(LandscapeInfo->Proxies.CreateConstIterator()); //LandscapeInfo->Proxies.CreateConstIterator();
	UE_LOG(LogTemp, Warning, TEXT("Num of proxies: %d"), LandscapeInfo->Proxies.Num());
	UE_LOG(LogTemp, Warning, TEXT("Num of tiles: %d"), tiles.Num());
	tiles.Empty(); //Should not be needed, but for some reason tiles start of with a maxed out array, we must empty it.
	UE_LOG(LogTemp, Warning, TEXT("Num of tiles after emptied: %d"), tiles.Num());

	//for (size_t i = 0; i < LandscapeInfo->Proxies.Num(); i++)
	//{
	//	/*TObjectPtr<ALandscapeStreamingProxy> myPointer = LandscapeInfo->Proxies[i];
	//	UE_LOG(LogTemp, Warning, TEXT("Value tx: %s"), *myPointer->GetName());*/

	//	//tiles.Add(Tile(LandscapeInfo->Proxies[i]));
	//	
	//}
	for (auto& it : LandscapeInfo->Proxies)
	{
		tiles.Add(Tile(it));

	}
	UE_LOG(LogTemp, Warning, TEXT("Num of tiles after addding them: %d"), tiles.Num());

	for (size_t i = 0; i < tiles.Num(); i++)
	{
		if (i % 2 ==0)
		{
			tiles[i].updateMaterial(LoadObject<UMaterial>(nullptr, TEXT("Material'/Game/Test_assets/M_gravelMaterial.M_gravelMaterial'")));
		}
	}

	/*UMaterial* ExampleMaterial = nullptr;
	ConstructorHelpers::FObjectFinderMaterialAsset(TEXT(""))
		ConstructorHelpers::*/

	//static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("D:/Unreal Engine Projects/ProceduralWorlds/Content/Test_assets/M_grassMaterial.uasset"));

	//if (Material.Object != NULL)
	//{
	//	tiles[0].streamingProxy->EditorSetLandscapeMaterial((UMaterial*)Material.Object);
	//	//TheMaterial = (UMaterial*)Material.Object;
	//}



	//tiles[0].streamingProxy->LandscapeMaterial

	//tiles[0].streamingProxy->material
	
	/*for (size_t i = 0; i < tiles.Num();i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("Value tx: %s"), *tiles[i].streamingProxy->GetName());
	}*/
	//for(auto& it:LandscapeInfo->Proxies)
	//{
	//	//it->GetName();
	//	//UE_LOG(LogTemp, Warning, TEXT("Value tx: %s"), *it->GetName());
	//	tiles.Add(Tile(it));
	//}
	/*for (auto& it : tiles)
	{
		UE_LOG(LogTemp, Warning, TEXT("Value tx: %s"), *it.streamingProxy->GetName());
		
	}*/
	//tiles.Add(Tile(LandscapeInfo.));

	//TArray<ALandscapeProxy*> proxies = landscapePtr->GetLandscapeProxies();
	
	
	return FReply::Handled();
}


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

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FProceduralWorldStyle::Shutdown();

	FProceduralWorldCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ProceduralWorldTabName);
}

TSharedRef<SDockTab> FProceduralWorldModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FProceduralWorldModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("ProceduralWorld.cpp"))
		);

	CreateLandscape boomboom;

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			//myButt;
			SNew(SBox)
			.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SButton)
			.Text(WidgetText)
			.OnClicked_Raw(this, &FProceduralWorldModule::Setup)
		
		
				
			]
		];
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