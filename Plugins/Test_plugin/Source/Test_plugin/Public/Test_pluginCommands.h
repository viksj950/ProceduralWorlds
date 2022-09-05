// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "Test_pluginStyle.h"

class FTest_pluginCommands : public TCommands<FTest_pluginCommands>
{
public:

	FTest_pluginCommands()
		: TCommands<FTest_pluginCommands>(TEXT("Test_plugin"), NSLOCTEXT("Contexts", "Test_plugin", "Test_plugin Plugin"), NAME_None, FTest_pluginStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};