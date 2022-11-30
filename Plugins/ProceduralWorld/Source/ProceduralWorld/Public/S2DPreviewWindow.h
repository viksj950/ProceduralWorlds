// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Brushes/SlateImageBrush.h"

/**
 * 
 */
class PROCEDURALWORLD_API S2DPreviewWindow
{
public:
	S2DPreviewWindow(const int32 &inSizeX,const int32 &inSizeY, const int32 &inQuadsPerComponent,
		const int32 &inComponentsPerProxy, const int32& inSectionsPerComponent,const int32 &inTilesSize);

	//Landscape properties
	int32 SizeX{ 505 };
	int32 SizeY{ 505 };
	int32 QuadsPerComponent{ 63 };
	int32 ComponentsPerProxy{ 1 };
	int32 SectionsPerComponent{ 1 };
	int32 TileSize{ 64 };

	//ment to be returned as the whole context of the 2D previes
	TSharedPtr<SBorder> previewContext;

	TArray<UTexture2D*> textures;
	TArray<TSharedPtr<FSlateImageBrush>> brushes;

	void UpdateLandscapeSettings(const int32& inSizeX, const int32& inSizeY, const int32& inQuadsPerComponent, const int32& inComponentsPerProxy, const int32& inSectionsPerComponent, const int32& inTilesSize);
	void CreateHeightmapTexture(const TArray<uint16>& inData);
	void CreateGridTexture();
	void AssembleWidget();

	~S2DPreviewWindow();
};
