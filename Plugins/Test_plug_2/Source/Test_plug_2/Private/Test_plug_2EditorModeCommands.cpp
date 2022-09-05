// Copyright Epic Games, Inc. All Rights Reserved.

#include "Test_plug_2EditorModeCommands.h"
#include "Test_plug_2EditorMode.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "Test_plug_2EditorModeCommands"

FTest_plug_2EditorModeCommands::FTest_plug_2EditorModeCommands()
	: TCommands<FTest_plug_2EditorModeCommands>("Test_plug_2EditorMode",
		NSLOCTEXT("Test_plug_2EditorMode", "Test_plug_2EditorModeCommands", "Test_plug_2 Editor Mode"),
		NAME_None,
		FEditorStyle::GetStyleSetName())
{
}

void FTest_plug_2EditorModeCommands::RegisterCommands()
{
	TArray <TSharedPtr<FUICommandInfo>>& ToolCommands = Commands.FindOrAdd(NAME_Default);

	UI_COMMAND(SimpleTool, "Show Actor Info", "Opens message box with info about a clicked actor", EUserInterfaceActionType::Button, FInputChord());
	ToolCommands.Add(SimpleTool);

	UI_COMMAND(InteractiveTool, "Measure Distance", "Measures distance between 2 points (click to set origin, shift-click to set end point)", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(InteractiveTool);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> FTest_plug_2EditorModeCommands::GetCommands()
{
	return FTest_plug_2EditorModeCommands::Get().Commands;
}

#undef LOCTEXT_NAMESPACE
