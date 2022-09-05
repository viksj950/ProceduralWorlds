// Copyright Epic Games, Inc. All Rights Reserved.

#include "Test_plugin.h"
#include "Test_pluginStyle.h"
#include "Test_pluginCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

static const FName Test_pluginTabName("Test_plugin");

#define LOCTEXT_NAMESPACE "FTest_pluginModule"

void FTest_pluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FTest_pluginStyle::Initialize();
	FTest_pluginStyle::ReloadTextures();

	FTest_pluginCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FTest_pluginCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FTest_pluginModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FTest_pluginModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(Test_pluginTabName, FOnSpawnTab::CreateRaw(this, &FTest_pluginModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FTest_pluginTabTitle", "Test_plugin"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FTest_pluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FTest_pluginStyle::Shutdown();

	FTest_pluginCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(Test_pluginTabName);
}

TSharedRef<SDockTab> FTest_pluginModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FTest_pluginModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("Test_plugin.cpp"))
		);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(WidgetText)
			]
		];
}

void FTest_pluginModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(Test_pluginTabName);
}

void FTest_pluginModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FTest_pluginCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FTest_pluginCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTest_pluginModule, Test_plugin)