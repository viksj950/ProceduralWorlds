// Copyright Epic Games, Inc. All Rights Reserved.

#include "Test_plug_2Module.h"
#include "Test_plug_2EditorModeCommands.h"

#define LOCTEXT_NAMESPACE "Test_plug_2Module"

void FTest_plug_2Module::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FTest_plug_2EditorModeCommands::Register();
}

void FTest_plug_2Module::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	FTest_plug_2EditorModeCommands::Unregister();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTest_plug_2Module, Test_plug_2EditorMode)