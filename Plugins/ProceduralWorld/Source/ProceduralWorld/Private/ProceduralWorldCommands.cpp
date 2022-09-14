// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProceduralWorldCommands.h"

#define LOCTEXT_NAMESPACE "FProceduralWorldModule"

void FProceduralWorldCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "ProceduralWorld", "Bring up ProceduralWorld window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
