// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "ProceduralWorldStyle.h"

class FProceduralWorldCommands : public TCommands<FProceduralWorldCommands>
{
public:

	FProceduralWorldCommands()
		: TCommands<FProceduralWorldCommands>(TEXT("ProceduralWorld"), NSLOCTEXT("Contexts", "ProceduralWorld", "ProceduralWorld Plugin"), NAME_None, FProceduralWorldStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};