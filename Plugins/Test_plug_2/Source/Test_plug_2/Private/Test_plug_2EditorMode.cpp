// Copyright Epic Games, Inc. All Rights Reserved.

#include "Test_plug_2EditorMode.h"
#include "Test_plug_2EditorModeToolkit.h"
#include "EdModeInteractiveToolsContext.h"
#include "InteractiveToolManager.h"
#include "Test_plug_2EditorModeCommands.h"


//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
// AddYourTool Step 1 - include the header file for your Tools here
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
#include "Tools/Test_plug_2SimpleTool.h"
#include "Tools/Test_plug_2InteractiveTool.h"

// step 2: register a ToolBuilder in FTest_plug_2EditorMode::Enter() below


#define LOCTEXT_NAMESPACE "Test_plug_2EditorMode"

const FEditorModeID UTest_plug_2EditorMode::EM_Test_plug_2EditorModeId = TEXT("EM_Test_plug_2EditorMode");

FString UTest_plug_2EditorMode::SimpleToolName = TEXT("Test_plug_2_ActorInfoTool");
FString UTest_plug_2EditorMode::InteractiveToolName = TEXT("Test_plug_2_MeasureDistanceTool");


UTest_plug_2EditorMode::UTest_plug_2EditorMode()
{
	FModuleManager::Get().LoadModule("EditorStyle");

	// appearance and icon in the editing mode ribbon can be customized here
	Info = FEditorModeInfo(UTest_plug_2EditorMode::EM_Test_plug_2EditorModeId,
		LOCTEXT("ModeName", "Test_plug_2"),
		FSlateIcon(),
		true);
}


UTest_plug_2EditorMode::~UTest_plug_2EditorMode()
{
}


void UTest_plug_2EditorMode::ActorSelectionChangeNotify()
{
}

void UTest_plug_2EditorMode::Enter()
{
	UEdMode::Enter();

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// AddYourTool Step 2 - register the ToolBuilders for your Tools here.
	// The string name you pass to the ToolManager is used to select/activate your ToolBuilder later.
	//////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////// 
	const FTest_plug_2EditorModeCommands& SampleToolCommands = FTest_plug_2EditorModeCommands::Get();

	RegisterTool(SampleToolCommands.SimpleTool, SimpleToolName, NewObject<UTest_plug_2SimpleToolBuilder>(this));
	RegisterTool(SampleToolCommands.InteractiveTool, InteractiveToolName, NewObject<UTest_plug_2InteractiveToolBuilder>(this));

	// active tool type is not relevant here, we just set to default
	GetToolManager()->SelectActiveToolType(EToolSide::Left, SimpleToolName);
}

void UTest_plug_2EditorMode::CreateToolkit()
{
	Toolkit = MakeShareable(new FTest_plug_2EditorModeToolkit);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> UTest_plug_2EditorMode::GetModeCommands() const
{
	return FTest_plug_2EditorModeCommands::Get().GetCommands();
}

#undef LOCTEXT_NAMESPACE
