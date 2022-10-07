// Fill out your copyright notice in the Description page of Project Settings.


#include "CreateLandscape.h"


CreateLandscape::CreateLandscape(int32 inSizeX, int32 inSizeY, int32 inQuadsPerComponent, int32 inComponentPerProxy, int32 inSectionsPerComponent)
	:
	 SizeX( inSizeX ),
	 SizeY( inSizeY ),
	 QuadsPerComponent( inQuadsPerComponent ),
	 ComponentsPerProxy( inComponentPerProxy ),
	 SectionsPerComponent( inSectionsPerComponent )
{
	//Amount of components per proxy, 
	gridSizeOfProxies = (SizeX - 1) / ((QuadsPerComponent * SectionsPerComponent));
	//gridSizeOfProxies = 8;
	////DEBUGGING ------------------------------------------------------
	//heightData.Init(32768,SizeX*SizeY);
	//heightData[58] = 58;
	//heightData[520] = 520;
	//heightData[62 * 505 + 0 + 505 + 63] = 0; //31878
	//heightData[420] = 420; //31878



	//
	//heightData[(505 * 505 - 1) - 63*(504) - 63];
	//heightData[505 * 505 - 1] = 69;
	//heightData[0] = 7;

}

CreateLandscape::~CreateLandscape()
{
}

																	//0,505,63
int32 CreateLandscape::assignDataToTile(UTile* inTile, int32 startVert, int32 inSizeX, int32 inQuadsPerComponent)
{
	//Add all samples between last point and first point
	int32 middlePart = (inQuadsPerComponent - 1) * inSizeX; // (63 - 1) * 505 = 31 310
	int32 edges = startVert + inSizeX + inQuadsPerComponent; // 0 + 505 + 63 = 506; 
	int32 endVert = middlePart + edges;	// 31 310 + 506 = 31 816
	int32 vertCounter = startVert;	// 0

	int32 tileHeightDataCounter{ 0 };

	do
	{
	
		for (size_t i = 0; i <= inQuadsPerComponent; i++)
		{
			if (!inTile->isCity)
			{
				inTile->tileHeightData[tileHeightDataCounter] = heightData[vertCounter + i];
			}
			else
			{
				inTile->tileHeightData[tileHeightDataCounter] = cityHeightData[vertCounter + i];
			}
			
			tileHeightDataCounter++;
			//UE_LOG(LogTemp, Warning, TEXT("Tilearray value : %d"), heightData[vertCounter + i]);
		}
		


		vertCounter += inSizeX;
	} while (vertCounter <= endVert);

	return endVert - inQuadsPerComponent;	
}

void CreateLandscape::assignDataToAllTiles(TArray<UTile*> &inTiles, int32 startVert, int32 inSizeX, int32 inQuadsPerComponent, int32 inComponentsPerProxy)
{
	int32 rowVertCounter = startVert; //The start vertex for each row					
	int32 tilePerRow = (inSizeX - 1) / (inQuadsPerComponent * inComponentsPerProxy);
	
	int32 currentStartVert = startVert;
	for (auto& it : inTiles)
	{
		if (it->index % (tilePerRow) != 0 || it->index == 0)
		{
			currentStartVert = assignDataToTile(it,currentStartVert,inSizeX,inQuadsPerComponent); //First tile: 0,505,63
		}
		else
		{
			rowVertCounter += inQuadsPerComponent;
			//currentStartVert = rowVertCounter;
			currentStartVert = assignDataToTile(it, rowVertCounter, inSizeX, inQuadsPerComponent);
		}		

	}
	


}
void CreateLandscape::generateCityNoise()
{
	

	cityHeightData.SetNum(SizeX * SizeY);

	ValueNoiseGenerator<uint16, 64> valueGen;
	valueGen.GenerateNoiseValues();

	int heightScale = 256;
	for (size_t j = 0; j < SizeY; j++)
	{
		for (size_t i = 0; i < SizeX; i++)
		{

			cityHeightData[j * SizeX + i] = valueGen.processCoord(Vec2<float>(i, j) * 0.015625) * heightScale + 32768;
			//HeightData[j * SizeX + i] = noise.processCoord(Vec2<float>(i, j)) * heightScale + 32768;

			//if ((j * SizeX + i) == SizeX * j * 8) {
				//UE_LOG(LogTemp, Warning, TEXT("Value of heightdata: %d"), HeightData[j * SizeX + i]);
			//}

		}
	}


	




}
TArray<uint16> CreateLandscape::GetColumnOfHeightData(const TArray<uint16>& inData, int32 sizeOfSquare, int32 Column)
{
	//(sizeOfSquare = 64)
	int32 startVertex = Column * sizeOfSquare; //63 * 64 = 4032
	int32 endVertex = startVertex + sizeOfSquare - 1; // -1 since array index start at 0    //4032 + 64 - 1


	TArray<uint16> temp;

	for (size_t i = startVertex; i <= endVertex; i++)
	{
		temp.Add(inData[i]);
	}

	return temp;
}

TArray<uint16> CreateLandscape::GetRowOfHeightData(const TArray<uint16>& inData, int32 sizeOfSquare, int32 Row)
{

	TArray<uint16> temp;
	
	
	int32 VertexIterator = Row;
	int32 endVertex = (sizeOfSquare * sizeOfSquare - sizeOfSquare) + Row;
	while (VertexIterator <= endVertex)
	{

		temp.Add(inData[VertexIterator]);

		VertexIterator+= sizeOfSquare;
	}


	return temp;
}

TArray<uint16> CreateLandscape::concatHeightData(const TArray<UTile*> &inTiles)
{
	TArray<uint16> outHeightData;
	int32 heightDataIndex{ 0 };
	//iterera från tile index 0 till adjacent[6] och för varje tile lägg till en kolumn med data.  
	//(inSizeX - 1) / (inQuadsPerComponent * inComponentsPerProxy);
	int32 tileSize = (SizeX - 1) / (QuadsPerComponent * ComponentsPerProxy); //Ger 8 för 1 component per proxy 505size och 63 quadsPerComponent.


	for (size_t j = 0; j < tileSize; j++) //yttre löper igenom översta raden med tiles 0-7
	{
		size_t i = 1;
		if (j == 0)
		{
			i = 0;
		}

		bool IsStart = true;
		for (;i <= QuadsPerComponent; i++)	//löper igenom index för columner i varje tile.
		{
			UTile* tilePtr = inTiles[j];
			while (tilePtr != nullptr)		//löper igenom tiles i en column med hjälp av adjacentTiles
			{
				if ( !IsStart)	//Om det är första tilen, då tar man inte bort
				{
					outHeightData.RemoveAt(heightDataIndex,1,true);
					
					//outHeightData.Shrink();
					
				}
				/*else
				{
					heightDataIndex += 1;
				}*/

				outHeightData.Append(GetColumnOfHeightData(tilePtr->tileHeightData,QuadsPerComponent +1,i));
				//UE_LOG(LogTemp, Warning, TEXT("Num of vertices added to outHeightData: %d"), outHeightData.Num());

				heightDataIndex += QuadsPerComponent;

				tilePtr = tilePtr->adjacentTiles[6];
				IsStart = false;
				
			}
			IsStart = true;
			heightDataIndex += 1;
			//inTiles[j]->adjacentTiles[6];
		}
		//After all tiles in the first column has added its data, the remaining columns of data do not add their first column of heightdata.
		
	}


	return outHeightData;
}
void CreateLandscape::PreProcessNoise(TArray<UTile*>& inTiles)
{
	TArray<uint16> HeightData;
	HeightData.SetNum(SizeX * SizeY);

	PerlinNoiseGenerator<uint16, 64> PerlinNoise{};
	PerlinNoise.generateGradients();
	//NoiseGenerator<uint16, 64> noise{}; //N is "cell size", 127 is tiny tiles 1009 is large tiles
	//noise.GenerateNoiseValues(2016);

	int heightScale = 4192;
	for (size_t j = 0; j < SizeY; j++)
	{
		for (size_t i = 0; i < SizeX; i++)
		{

			HeightData[j * SizeX + i] = PerlinNoise.generateNoiseVal(Vec2<float>(i, j) * 0.015625) * heightScale + 32768;
			//HeightData[j * SizeX + i] = noise.processCoord(Vec2<float>(i, j)) * heightScale + 32768;

			//if ((j * SizeX + i) == SizeX * j * 8) {
				//UE_LOG(LogTemp, Warning, TEXT("Value of heightdata: %d"), HeightData[j * SizeX + i]);
			//}

		}
	}



	heightData = HeightData;

	assignDataToAllTiles(inTiles,0,SizeX,QuadsPerComponent,ComponentsPerProxy);

}

ALandscape* CreateLandscape::generate()
{
	USelection* SelectedActors = nullptr;
	FText DialogText;

	if (GEditor->GetSelectedActors() == nullptr) {
		SelectedActors = GEditor->GetSelectedActors();

	}
	else {
		DialogText = FText::FromString("Landscape plugin activated, creating landscape...");
	}

	//Add this to have confirmation window
	//FMessageDialog::Open(EAppMsgType::Ok, DialogText);

	FQuat InRotation{ 0,0,0,0 };
	FVector InTranslation{ 0,0,5 };
	FVector InScale{ 100,100,100 };
	FTransform LandscapeTransform{ InRotation, InTranslation, InScale };
	//int32 QuadsPerSection{ 63 };
	//int32 SectionsPerComponent{ 1 };
	////int32 ComponentCountX{12345};
	////int32 ComponentCountY{21123};
	//int32 QuadsPerComponent{ 63 }; 
	//int32 SizeX{ 505 };
	//int32 SizeY{ 505 }; 
	//int32 ComponentsPerProxy{ 1 };

	//Original map template:505x505 1 63x63 64 2
	//Smaller map template: 253x253 1 63x63 16 2


	TArray<FLandscapeImportLayerInfo> MaterialImportLayers;
	TMap<FGuid, TArray<uint16>> HeightDataPerLayers;
	TMap<FGuid, TArray<FLandscapeImportLayerInfo>> MaterialLayerDataPerLayers;

	

	HeightDataPerLayers.Add(FGuid(), MoveTemp(heightData));
	MaterialLayerDataPerLayers.Add(FGuid(), MoveTemp(MaterialImportLayers));

	UWorld* World = nullptr;

	// We want to create the landscape in the landscape editor mode's world
	FWorldContext& EditorWorldContext = GEditor->GetEditorWorldContext();
	World = EditorWorldContext.World();

	ALandscape* Landscape = World->SpawnActor<ALandscape>(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f)); //This is working Pog

	Landscape->bCanHaveLayersContent = true;
	Landscape->LandscapeMaterial = nullptr;
	//static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/Materials/YourMaterialName.YourMaterialName'"));
	//static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/Test_assets/M_grassMaterial.M_grassMaterial'"));

	//if (Material.Object != NULL)
	//{
	//	Landscape->LandscapeMaterial = (UMaterial*)Material.Object;
	//	//TheMaterial = (UMaterial*)Material.Object;
	//}else{ 
	//	Landscape->LandscapeMaterial = nullptr; 
	//}

	
	

	Landscape->SetActorTransform(LandscapeTransform);
	Landscape->Import(FGuid::NewGuid(), 0, 0, SizeX - 1, SizeY - 1, SectionsPerComponent, QuadsPerComponent,
		HeightDataPerLayers, nullptr, MaterialLayerDataPerLayers, ELandscapeImportAlphamapType::Additive);

	Landscape->StaticLightingLOD = FMath::DivideAndRoundUp(FMath::CeilLogTwo((SizeX * SizeY) / (2048 * 2048) + 1), (uint32)2);
	// Register all the landscape components
	ULandscapeInfo* LandscapeInfo = Landscape->GetLandscapeInfo();

	LandscapeInfo->UpdateLayerInfoMap(Landscape);

	

	Landscape->RegisterAllComponents();

	// Need to explicitly call PostEditChange on the LandscapeMaterial property or the landscape proxy won't update its material
	FPropertyChangedEvent MaterialPropertyChangedEvent(FindFieldChecked< FProperty >(Landscape->GetClass(), FName("LandscapeMaterial")));
	Landscape->PostEditChangeProperty(MaterialPropertyChangedEvent);
	Landscape->PostEditChange();

	//Changing Gridsize which will create LandscapestreamProcies, Look at file: LandscapeEditorDetailCustomization_NewLandscape.cpp line 800
	EditorWorldContext.World()->GetSubsystem<ULandscapeSubsystem>()->ChangeGridSize(LandscapeInfo, ComponentsPerProxy);

	gridSizeOfProxies = (SizeX - 1) / ((QuadsPerComponent * SectionsPerComponent) * ComponentsPerProxy);
	
	//LandscapeInfo->export
	return Landscape;

}

ALandscape* CreateLandscape::generateFromTileData(TArray<UTile*>& inTiles)
{

	FQuat InRotation{ 0,0,0,0 };
	FVector InTranslation{ 0,0,5 };
	FVector InScale{ 100,100,100 };
	FTransform LandscapeTransform{ InRotation, InTranslation, InScale };

	TArray<uint16> concatedHeightData = concatHeightData(inTiles);

	TArray<FLandscapeImportLayerInfo> MaterialImportLayers;
	TMap<FGuid, TArray<uint16>> HeightDataPerLayers;
	TMap<FGuid, TArray<FLandscapeImportLayerInfo>> MaterialLayerDataPerLayers;

	

	HeightDataPerLayers.Add(FGuid(), MoveTemp(concatedHeightData));
	MaterialLayerDataPerLayers.Add(FGuid(), MoveTemp(MaterialImportLayers));

	UWorld* World = nullptr;

	// We want to create the landscape in the landscape editor mode's world
	FWorldContext& EditorWorldContext = GEditor->GetEditorWorldContext();
	World = EditorWorldContext.World();

	ALandscape* Landscape = World->SpawnActor<ALandscape>(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f)); //This is working Pog

	Landscape->bCanHaveLayersContent = true;
	Landscape->LandscapeMaterial = nullptr;
	//static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/Materials/YourMaterialName.YourMaterialName'"));
	//static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/Test_assets/M_grassMaterial.M_grassMaterial'"));

	//if (Material.Object != NULL)
	//{
	//	Landscape->LandscapeMaterial = (UMaterial*)Material.Object;
	//	//TheMaterial = (UMaterial*)Material.Object;
	//}else{ 
	//	Landscape->LandscapeMaterial = nullptr; 
	//}

	
	

	Landscape->SetActorTransform(LandscapeTransform);
	Landscape->Import(FGuid::NewGuid(), 0, 0, SizeX - 1, SizeY - 1, SectionsPerComponent, QuadsPerComponent,
		HeightDataPerLayers, nullptr, MaterialLayerDataPerLayers, ELandscapeImportAlphamapType::Additive);

	Landscape->StaticLightingLOD = FMath::DivideAndRoundUp(FMath::CeilLogTwo((SizeX * SizeY) / (2048 * 2048) + 1), (uint32)2);
	// Register all the landscape components
	ULandscapeInfo* LandscapeInfo = Landscape->GetLandscapeInfo();

	LandscapeInfo->UpdateLayerInfoMap(Landscape);

	

	Landscape->RegisterAllComponents();

	// Need to explicitly call PostEditChange on the LandscapeMaterial property or the landscape proxy won't update its material
	FPropertyChangedEvent MaterialPropertyChangedEvent(FindFieldChecked< FProperty >(Landscape->GetClass(), FName("LandscapeMaterial")));
	Landscape->PostEditChangeProperty(MaterialPropertyChangedEvent);
	Landscape->PostEditChange();

	//Changing Gridsize which will create LandscapestreamProcies, Look at file: LandscapeEditorDetailCustomization_NewLandscape.cpp line 800
	EditorWorldContext.World()->GetSubsystem<ULandscapeSubsystem>()->ChangeGridSize(LandscapeInfo, ComponentsPerProxy);

	
	
	//LandscapeInfo->export




	return Landscape;
}

////TArray<FVector> CreateLandscape::generateNormals(const TArray<uint16>& inHeight)
////{
////	//Distance between vertices in x and y is the same for all vertices, only difference is height

////	//Find and mark all sampes wiht position
	
////	//use position to gret 1 ring
////	Tarray height getOnering()



////	return TArray<FVector>();
////}

const uint32 CreateLandscape::GetGridSizeOfProxies() const
{

	return gridSizeOfProxies;
}

//OLD ASS CODE

	//int32 numOfProxiesPerRow = (SizeX / QuadsPerComponent * ComponentsPerProxy);

	//int32 tileColumn{ 0 };
	//int32 tileCounter{ 0 };
	
	//for (size_t i = 0; i < heightData.Num(); i++)
	//{

	//	inTiles[tileCounter]->tileHeightData[i] = heightData[i];

	//	//If true on boundaries of tiles, adds same data as duplicates
	//	if (i % ((QuadsPerComponent+1) * ComponentsPerProxy) == 0)
	//	{

	//		tileCounter += numOfProxiesPerRow;
	//		inTiles[tileCounter]->tileHeightData[i] = heightData[i];
	//	}

	//	//New row
	//	if (i == SizeX * (QuadsPerComponent + 1))
	//	{
	//		tileColumn++;
	//	}
	//	//Next column of verts
	//	if (i % SizeX == 0)
	//	{
	//		tileCounter = tileColumn;
	//	}
		

	//}

	//int32 startVert{ 0 };
	//for (auto& it: inTiles)
	//{

		
	//	for (size_t i = startVert; i < heightData.Num(); i++)
	//	{
	//		for (size_t j = 0; j < it->tileHeightData.Num(); j++)
	//		{
	//			if (j % (QuadsPerComponent+1) == 0)
	//			{
	//				//iterated through the size of a tile
	//				//Right side: SízeX - (2*QuadsPerComp) 
	//				//Middle: (SizeX-2) * QuadsPerComp
	//				//Left: SizeX - (6 * QuadsPerComp)
	//			}

	//		}

	//	}

	//}