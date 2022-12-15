// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Brushes/SlateImageBrush.h"
#include "Containers/Map.h"

//#include "Widgets/Views/SListView.h"
//Struct for storing a roads coordinates
struct RoadCoords
{
	//RoadCoords();
	uint32 roadID;
	TArray<FVector> Points;
};

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

	bool displayGrid;
	bool displayBiotopes;
	bool displayRoads;


	TMap<int32, int32> markedTiles;

	//Marked Origins of biotopes
	TMap<int32,int32> markedTilesVoronoi;

	

	void UpdateLandscapeSettings(const int32& inSizeX, const int32& inSizeY, const int32& inQuadsPerComponent, const int32& inComponentsPerProxy, const int32& inSectionsPerComponent, const int32& inTilesSize);
	void CreateHeightmapTexture(const TArray<uint16>& inData);
	void CreateGridTexture();
	void CreateBiotopeTexture();
	void CreateRoadMarkTexture();
	void AssembleWidget();
	void MarkTile(int32 selectedBiotope, FVector2D inCoords);
	void MarkTileVoronoi(int32 selectedBiotope, FVector2D inCoords);

	//Road functionality---------------------------------------------------------

	//Road Coordinates
	//TArray<FVector> roadCoords;
	TArray<TSharedPtr<RoadCoords>> roadsData;
	uint32 roadIndex;

	TSharedPtr<SListView< TSharedPtr<RoadCoords>>> roadsDataList;
	//TSharedRef<STableRow<TSharedPtr<RoadCoords>>> OnGenerateWidgetForList(TSharedPtr<RoadCoords> inItem, const TSharedRef<STableViewBase>& OwnerTable);


	void AddRoad();
	void AddRoadPoint(FVector2D inCoords);
	void UpdateRoadIDs();
	void AssembleRoadListWidget();
	int32 FromCoordToTileIndex(FVector2D inCoords);
	

	~S2DPreviewWindow();

	

private:
	uint32 gridSizeOfProxies{ 0 };
};
