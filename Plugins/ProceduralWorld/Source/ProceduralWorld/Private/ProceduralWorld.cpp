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
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FProceduralWorldModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("ProceduralWorld.cpp"))
		);

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
					.Text(WidgetText)
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
						.Text(FText::FromString("Delete prev Landscape"))
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

	//Dynamic size
	//int sizeOfMyCock = myLand.GetGridSizeOfProxies() * myLand.GetGridSizeOfProxies();

	//Reserve to not reallacoate during runtime. 
	//tiles.Reserve(myLand.GetGridSizeOfProxies()*myLand.GetGridSizeOfProxies());

	//tiles.Init(Tile(), 16);

	ULandscapeInfo* LandscapeInfo = landscapePtr->GetLandscapeInfo();
	//TConstIterator<ALandscapeStreamingProxy> it(LandscapeInfo->Proxies.CreateConstIterator()); //LandscapeInfo->Proxies.CreateConstIterator();
	UE_LOG(LogTemp, Warning, TEXT("Num of proxies: %d"), LandscapeInfo->Proxies.Num());
	UE_LOG(LogTemp, Warning, TEXT("Num of tiles: %d"), tiles.Num());
	if (!tiles.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("tiles is not empty, attempting to solve ezpz by resizing"));
		//tiles.SetNum(sizeOfMyCock);
			
	}
	//tiles.Empty(); //Should not be needed, but for some reason tiles start of with a maxed out array, we must empty it.
	//UE_LOG(LogTemp, Warning, TEXT("Num of tiles after being resized: %d"), tiles.Num());

	//uint32 index{ 0 };
	//for (auto& it : LandscapeInfo->Proxies)
	//{
	//	Tile temp(it);
	//	temp.index = index;
	//	tiles.Add(temp);

	//	index++;

	//	//delete temp;
	//	//Här kallas temps destructor
	//}
	//UE_LOG(LogTemp, Warning, TEXT("Num of tiles after adding them: %d"), tiles.Num());

	/*for (size_t i = 0; i < tiles.Num(); i++)
	{

		tiles[i].updateAdjacentTiles(tiles, myLand.GetGridSizeOfProxies());

	}*/
	
	return FReply::Handled();
}

FReply FProceduralWorldModule::DeleteLandscape()
{
	if (!tiles.IsEmpty() && landscapePtr != nullptr)
	{
		bool isDestroyed{ false };
		for (auto& it : tiles)
		{

			isDestroyed = it.streamingProxy->Destroy();
			if (!isDestroyed)
			{
				UE_LOG(LogTemp, Warning, TEXT("Could not delete all tiles"));
				return FReply::Unhandled();
			}
		}
		tiles.Empty();
		landscapePtr->Destroy();
		//delete landscapePtr;

		
		return FReply::Handled();

	}
	else
	{	
		UE_LOG(LogTemp, Warning, TEXT("Could not delete landscape, no landscape is being pointed"));
		return FReply::Unhandled();
	}
	
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