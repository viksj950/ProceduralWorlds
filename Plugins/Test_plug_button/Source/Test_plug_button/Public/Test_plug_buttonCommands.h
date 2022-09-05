// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "Test_plug_buttonStyle.h"

class FTest_plug_buttonCommands : public TCommands<FTest_plug_buttonCommands>
{
public:

	FTest_plug_buttonCommands()
		: TCommands<FTest_plug_buttonCommands>(TEXT("Test_plug_button"), NSLOCTEXT("Contexts", "Test_plug_button", "Test_plug_button Plugin"), NAME_None, FTest_plug_buttonStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
