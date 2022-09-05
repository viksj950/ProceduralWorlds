// Copyright Epic Games, Inc. All Rights Reserved.

#include "Test_plug_2EditorModeToolkit.h"
#include "Test_plug_2EditorMode.h"
#include "Engine/Selection.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "Test_plug_2EditorModeToolkit"

FTest_plug_2EditorModeToolkit::FTest_plug_2EditorModeToolkit()
{
}

void FTest_plug_2EditorModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode)
{
	FModeToolkit::Init(InitToolkitHost, InOwningMode);
}

void FTest_plug_2EditorModeToolkit::GetToolPaletteNames(TArray<FName>& PaletteNames) const
{
	PaletteNames.Add(NAME_Default);
}


FName FTest_plug_2EditorModeToolkit::GetToolkitFName() const
{
	return FName("Test_plug_2EditorMode");
}

FText FTest_plug_2EditorModeToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("DisplayName", "Test_plug_2EditorMode Toolkit");
}

#undef LOCTEXT_NAMESPACE
