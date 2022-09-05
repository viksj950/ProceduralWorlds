// Copyright Epic Games, Inc. All Rights Reserved.

#include "Test_plug_buttonCommands.h"

#define LOCTEXT_NAMESPACE "FTest_plug_buttonModule"

void FTest_plug_buttonCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "Test_plug_button", "Execute Test_plug_button action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
