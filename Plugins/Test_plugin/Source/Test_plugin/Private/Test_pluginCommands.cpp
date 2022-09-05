// Copyright Epic Games, Inc. All Rights Reserved.

#include "Test_pluginCommands.h"

#define LOCTEXT_NAMESPACE "FTest_pluginModule"

void FTest_pluginCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Test_plugin", "Bring up Test_plugin window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
