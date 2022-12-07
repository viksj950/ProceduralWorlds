// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Brushes/SlateImageBrush.h"
#include "Containers/Map.h"

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
	TSharedPtr<SOverlay> previewOverlay;

	TArray<UTexture2D*> textures;
	TArray<TSharedPtr<FSlateImageBrush>> brushes;

	TArray<FVector3f> colors = {
	FVector3f(137, 49, 239),
	FVector3f(242, 202, 25),
	FVector3f(255, 0, 189),
	FVector3f(0, 87, 233),
	FVector3f(135, 233, 17),
	FVector3f(225, 24, 69),
	

	
	};

	TMap<int32, int32> markedTiles;

	void UpdateLandscapeSettings(const int32& inSizeX, const int32& inSizeY, const int32& inQuadsPerComponent, const int32& inComponentsPerProxy, const int32& inSectionsPerComponent, const int32& inTilesSize);
	void CreateHeightmapTexture(const TArray<uint16>& inData);
	void CreateGridTexture();
	void CreateBiotopeTexture();
	void AssembleWidget();
	void MarkTile(int32 selectedBiotope, FVector2D inCoords);
	int32 FromCoordToTileIndex(FVector2D inCoords);
	

	~S2DPreviewWindow();

private:
	uint32 gridSizeOfProxies{ 0 };
};
