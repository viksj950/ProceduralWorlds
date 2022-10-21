// Fill out your copyright notice in the Description page of Project Settings.


#include "CreateLandscape.h"


CreateLandscape::CreateLandscape(int32 inSizeX, int32 inSizeY, int32 inQuadsPerComponent, int32 inComponentPerProxy, int32 inSectionsPerComponent, int32 inTileSize)
	:
	 SizeX( inSizeX ),
	 SizeY( inSizeY ),
	 QuadsPerComponent( inQuadsPerComponent ),
	 ComponentsPerProxy( inComponentPerProxy ),
	 SectionsPerComponent( inSectionsPerComponent ),
	 TileSize(inTileSize)
{
	//Amount of components per proxy, 
	gridSizeOfProxies = (SizeX - 1) / ((QuadsPerComponent * ComponentsPerProxy));

	/*heightData.SetNum(SizeX * SizeY);
	cityHeightData.SetNum(SizeX * SizeY);
	mountainHeightData.SetNum(SizeX * SizeY);*/
	//heightData.Reserve(inSizeX * inSizeY);
	////Currently city has the same size, not optimized, have to redo alot to improve it
	//cityHeightData.Reserve(inSizeX * inSizeY);

}

CreateLandscape::~CreateLandscape()
{
}

																	//0,505,63
int32 CreateLandscape::assignDataToTile(UTile* inTile, int32 startVert, int32 inSizeX, int32 inQuadsPerComponent)
{
	//Add all samples between last point and first point
	int32 middlePart = (TileSize - 2) * inSizeX; // (64 - 2) * 505 = 31 310
	int32 edges = startVert + inSizeX + (TileSize -1); // 0 + 505 + 63 = 506; 
	int32 endVert = middlePart + edges;	// 31 310 + 506 = 31 816
	int32 vertCounter = startVert;	// 0

	int32 tileHeightDataCounter{ 0 };

	do
	{
	
		for (size_t i = 0; i < TileSize; i++)
		{
			if (inTile->biotope == 1)
			{
				inTile->tileHeightData[tileHeightDataCounter] = heightData[vertCounter + i];
			}
			else if (inTile->biotope == 0)
			{
				inTile->tileHeightData[tileHeightDataCounter] = cityHeightData[vertCounter + i];
			}
			else if (inTile->biotope == 2) {
				inTile->tileHeightData[tileHeightDataCounter] = mountainHeightData[vertCounter + i];
			}
			
			tileHeightDataCounter++;
			//UE_LOG(LogTemp, Warning, TEXT("Tilearray value : %d"), heightData[vertCounter + i]);
		}

		vertCounter += inSizeX;
	} while (vertCounter <= endVert);

	return endVert - TileSize +1;	
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
			rowVertCounter += (TileSize-1);
			//currentStartVert = rowVertCounter;
			currentStartVert = assignDataToTile(it, rowVertCounter, inSizeX, inQuadsPerComponent);
		}		

	}
	heightData.Empty();
	cityHeightData.Empty();
	
}
void CreateLandscape::GenerateAndAssignHeightData(TArray<UTile*>& inTiles, const TArray<TSharedPtr<BiotopePerlinNoiseSetting>>& BiotopeSettings)
{


	/*X = tileIndex % gridSizeOfProxies;
	Y = FMath::Floor(tileIndex / gridSizeOfProxies);*/

	PerlinNoiseGenerator<uint16, 64> PerlinNoise;
	PerlinNoise.generateGradients();
	
	int X{ 0 };
	int Y{ 0 };
	int32 currentStartVert = 0;

	for (auto& it: inTiles)
	{
		if (it->index % gridSizeOfProxies == 0 && it->index != 0) //we have reached a new row
		{
			X = 0;
			Y = FMath::Floor(it->index / gridSizeOfProxies) * (TileSize-1);
		}
		else if(it->index != 0)
		{
			X += (TileSize - 1);
			Y = FMath::Floor(it->index / gridSizeOfProxies) * (TileSize - 1);
		}



		currentStartVert = PerlinNoise.GenerateAndAssignTileData(it->tileHeightData, it->tileSize, it->index, gridSizeOfProxies, X, Y, *BiotopeSettings[it->biotope]);

		
		//assign correct noise values depending on tile index and biotope 
		//Also need to have biotopeSettings
		//void func(tile, BiotopeSettings)

	}


}
void CreateLandscape::generateCityNoise()
{
	
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

void CreateLandscape::SetRowHeightData(TArray<uint16>& inData, const TArray<uint16>& inRowData, int32 sizeOfSquare, int32 Row)
{
	int32 VertexIterator = Row;
	int32 endVertex = (sizeOfSquare * sizeOfSquare - sizeOfSquare) + Row;
	int32 rowIndex{ 0 };
	while (VertexIterator <= endVertex)
	{

		inData[VertexIterator] = inRowData[rowIndex];

		VertexIterator += sizeOfSquare;
		rowIndex++;
	}

}

void CreateLandscape::SetColumnHeightData(TArray<uint16>& inData, const TArray<uint16>& inColumnData, int32 sizeOfSquare, int32 Column)
{

	//(sizeOfSquare = 64)
	int32 VertexIterator = Column * sizeOfSquare; //63 * 64 = 4032
	//int32 endVertex = VertexIterator + sizeOfSquare - 1; // -1 since array index start at 0    //4032 + 64 - 1
	int32 columnIndex{ 0 };
	while(columnIndex < sizeOfSquare)
	{
		inData[VertexIterator] = inColumnData[columnIndex];
		VertexIterator++;
		columnIndex++;
	}
	
}

uint32 CreateLandscape::GetVertexIndex(const TArray<uint16>& inData, int32 dataDimension, int32 inX, int32 inY)
{
	if (inX <= dataDimension && inY <= dataDimension)
	{
		
		return inX * dataDimension + inY;

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Coordinates do not fit"));
		return 0;
	}
	

	
}

void CreateLandscape::concatHeightData(const TArray<UTile*> &inTiles, TArray<uint16>& data)
{
	int32 heightDataIndex{ 0 };
	//iterera från tile index 0 till adjacent[6] och för varje tile lägg till en kolumn med data.  
	//(inSizeX - 1) / (inQuadsPerComponent * inComponentsPerProxy);
	int32 nmbrOfTilesPerRow = (SizeX - 1) / (QuadsPerComponent * ComponentsPerProxy); //Ger 8 för 1 component per proxy 505size och 63 quadsPerComponent.


	for (size_t j = 0; j < nmbrOfTilesPerRow; j++) //yttre löper igenom översta raden med tiles 0-7
	{
		size_t i = 1;
		if (j == 0)
		{
			i = 0;
		}

		bool IsStart = true;
		for (;i < TileSize; i++)	//löper igenom index för columner i varje tile.
		{
			UTile* tilePtr = inTiles[j];
			while (tilePtr != nullptr)		//löper igenom tiles i en column med hjälp av adjacentTiles
			{
				if ( !IsStart)	//Om det är första tilen, då tar man inte bort
				{
					data.RemoveAt(heightDataIndex,1,true);
					
				}

				data.Append(GetColumnOfHeightData(tilePtr->tileHeightData,TileSize,i));
				//UE_LOG(LogTemp, Warning, TEXT("Num of vertices added to outHeightData: %d"), outHeightData.Num());

				heightDataIndex += (TileSize - 1);

				tilePtr = tilePtr->adjacentTiles[6];
				IsStart = false;
				
			}
			IsStart = true;
			heightDataIndex += 1;
		}
		//After all tiles in the first column has added its data, the remaining columns of data do not add their first column of heightdata.
		
	}

}
void CreateLandscape::interpAllAdjTiles(TArray<UTile*>& inTiles, int32 stepAmount)
{
	int rowLength = GetGridSizeOfProxies();
	//UE_LOG(LogTemp, Warning, TEXT("rowlenght is = %d"), rowLength);
	int rowCount = 0;
	for (auto& t : inTiles) {
		
		//Do interpolation for everyother tile ish (checkerboard)
		if (t->index != 0 && t->index % rowLength == 0) {
			rowCount++;
			
		}

		if (rowCount % 2 == 0) { //even row
			
			if (t->index % 2 != 0) {
				
				continue;
			}
			
		}
		else { //odd row
			if (t->index % 2 == 0) {
				
				continue;
			}
			
		}
		TArray<uint16> currentStartPoint;
		TArray<uint16> adjacentEndPoint;
		TArray<uint16> interpDataArray;

		int32 nmbrOfIterations;//needs to be even
		float stepSize;//stepSize of interpolation
		float step;
		int currentCounter ;
		int adjCounter;

		//All cases for the 8-adjacency tiles
		for (int i = 0; i < 8; i++) {
			if (t->adjacentTiles[i] != nullptr && t->biotope != t->adjacentTiles[i]->biotope) { //adjacent is other biome
				//if (i == 0) {//top right adjacent tile
				//
				//	uint16 adjVertHeight = t->adjacentTiles[i]->tileHeightData[t->tileHeightData.Num() - 1];
				//	uint16 currentVertHeight = t->tileHeightData[0];

				//	uint16 avgHeight = (adjVertHeight + currentVertHeight) / 2;
				//	UE_LOG(LogTemp, Warning, TEXT("adjVertHeight: %d"), adjVertHeight);
				//	UE_LOG(LogTemp, Warning, TEXT("currentVertHeight : %d"), currentVertHeight);
				//	UE_LOG(LogTemp, Warning, TEXT("New height is: %d"), avgHeight);
				//	t->adjacentTiles[i]->tileHeightData[t->tileHeightData.Num() - 1] = avgHeight;
				//	t->tileHeightData[0] = avgHeight;

				//}
				if (i == 1) {//top adjacent tile
					
					nmbrOfIterations = (stepAmount * 2);
					stepSize = 1.0f / nmbrOfIterations;	
					step = stepSize;
					currentCounter = stepAmount - 1;
					adjCounter = 0;

					currentStartPoint = GetRowOfHeightData(t->tileHeightData, TileSize, stepAmount);	//Start point and MiddlerowData is end point
					adjacentEndPoint = GetRowOfHeightData(t->adjacentTiles[i]->tileHeightData, TileSize, (TileSize - 1) - stepAmount);

					nmbrOfIterations--;
					do {

						for (int j = 0; j < TileSize; j++) { //create array of interpolated data

							interpDataArray.Add(currentStartPoint[j] * (1.0f - smoothstep(step)) + adjacentEndPoint[j] * smoothstep(step));
						}

						if(nmbrOfIterations >= stepAmount){ //CURRENT
							SetRowHeightData(t->tileHeightData, interpDataArray, TileSize, currentCounter);
							currentCounter--;
						}

						if(nmbrOfIterations <= stepAmount){ //ADJACENT
							SetRowHeightData(t->adjacentTiles[i]->tileHeightData, interpDataArray, TileSize, (TileSize - 1) - adjCounter);
							adjCounter++;
						}
						interpDataArray.Empty();
						step += stepSize;
						
						nmbrOfIterations--;

					} while (nmbrOfIterations > 0);

				}
				//Right adjacent tile
				if (i == 3) {

					nmbrOfIterations = (stepAmount * 2);
					stepSize = 1.0f / nmbrOfIterations;
					step = stepSize;
					currentCounter = stepAmount - 1;
					adjCounter = 0;

					currentStartPoint = GetColumnOfHeightData(t->tileHeightData, TileSize, stepAmount);	//Start point and MiddlerowData is end point
					adjacentEndPoint = GetColumnOfHeightData(t->adjacentTiles[i]->tileHeightData, TileSize, (TileSize - 1) - stepAmount);

					nmbrOfIterations--;
					do {

						for (int j = 0; j < TileSize; j++) { //create array of interpolated data

							interpDataArray.Add(currentStartPoint[j] * (1.0f - smoothstep(step)) + adjacentEndPoint[j] * smoothstep(step));
						}

						if (nmbrOfIterations >= stepAmount) { //CURRENT
							SetColumnHeightData(t->tileHeightData, interpDataArray, TileSize, currentCounter);
							currentCounter--;
						}

						if (nmbrOfIterations <= stepAmount) { //ADJACENT
							SetColumnHeightData(t->adjacentTiles[i]->tileHeightData, interpDataArray, TileSize, (TileSize - 1) - adjCounter);
							adjCounter++;
						}
						interpDataArray.Empty();
						step += stepSize;

						nmbrOfIterations--;

					} while (nmbrOfIterations > 0);
				}
				//Left adjacent tile
				if (i == 4) {
					nmbrOfIterations = (stepAmount * 2);
					stepSize = 1.0f / nmbrOfIterations;
					step = stepSize;
					currentCounter = stepAmount - 1;
					adjCounter = 0;

					currentStartPoint = GetColumnOfHeightData(t->tileHeightData, TileSize, (TileSize - 1) - stepAmount);	//Start point and MiddlerowData is end point
					adjacentEndPoint = GetColumnOfHeightData(t->adjacentTiles[i]->tileHeightData, TileSize, stepAmount);

					nmbrOfIterations--;
					do {

						for (int j = 0; j < TileSize; j++) { //create array of interpolated data

							interpDataArray.Add(currentStartPoint[j] * (1.0f - smoothstep(step)) + adjacentEndPoint[j] * smoothstep(step));
						}

						if (nmbrOfIterations >= stepAmount) { //CURRENT
							SetColumnHeightData(t->tileHeightData, interpDataArray, TileSize, (TileSize - 1) - currentCounter);
							currentCounter--;
						}

						if (nmbrOfIterations <= stepAmount) { //ADJACENT
							SetColumnHeightData(t->adjacentTiles[i]->tileHeightData, interpDataArray,TileSize, adjCounter);
							adjCounter++;
						}
						interpDataArray.Empty();
						step += stepSize;

						nmbrOfIterations--;

					} while (nmbrOfIterations > 0);
				}
				//Bottom adjacent tile
				if (i == 6) {
					nmbrOfIterations = (stepAmount * 2);
					stepSize = 1.0f / nmbrOfIterations;
					step = stepSize;
					currentCounter = stepAmount - 1;
					adjCounter = 0;

					currentStartPoint = GetRowOfHeightData(t->tileHeightData, TileSize, (TileSize - 1) - stepAmount);	//Start point and MiddlerowData is end point
					adjacentEndPoint = GetRowOfHeightData(t->adjacentTiles[i]->tileHeightData, TileSize, stepAmount);

					nmbrOfIterations--;
					do {

						for (int j = 0; j < TileSize; j++) { //create array of interpolated data

							interpDataArray.Add(currentStartPoint[j] * (1.0f - smoothstep(step)) + adjacentEndPoint[j] * smoothstep(step));
						}

						if (nmbrOfIterations >= stepAmount) { //CURRENT
							SetRowHeightData(t->tileHeightData, interpDataArray, TileSize, (TileSize - 1) - currentCounter);
							currentCounter--;
						}

						if (nmbrOfIterations <= stepAmount) { //ADJACENT
							SetRowHeightData(t->adjacentTiles[i]->tileHeightData, interpDataArray, TileSize, adjCounter);
							adjCounter++;
						}
						interpDataArray.Empty();
						step += stepSize;

						nmbrOfIterations--;

					} while (nmbrOfIterations > 0);
				}
			}
		}


		


	}
}



void CreateLandscape::AssignBiotopesToTiles(TArray<UTile*>& inTiles, const int &nmbrOfBiomes, const TArray<TSharedPtr<BiotopePerlinNoiseSetting>>& BiotopeSettings) const
{

	//3 default: city,plains,mountains
	float nmbrOfDifferentBiotopes = BiotopeSettings.Num();
	float nmbrOfTiles = inTiles.Num();
	float X;
	float Y;
	float maxDistance;
	
	TArray<BiomeOriginInformation> biomes;
	FMath mathInstance;
	for (size_t i = 0; i < nmbrOfBiomes; i++)
	{
		int32 biotope = FGenericPlatformMath::RoundToInt32(mathInstance.FRandRange(0.0f, nmbrOfDifferentBiotopes - 1));	//random type of biotope (0-2)
		int32 tileIndex = FGenericPlatformMath::RoundToInt32(mathInstance.FRandRange(0.0f, nmbrOfTiles - 1)); //Random tile as origin (0-7)

		//convert tile index to X Y coordinates used for range computation
		X = tileIndex % gridSizeOfProxies;
		Y = FMath::Floor(tileIndex / gridSizeOfProxies);
		if (biotope == 0) //city
		{
			maxDistance = 2;
		}
		else
		{
			maxDistance = gridSizeOfProxies * gridSizeOfProxies;
		}
		biomes.Add(BiomeOriginInformation(biotope,FVector2D(X,Y),maxDistance));

		//translate tile index to X Y coordinates

		
		UE_LOG(LogTemp, Warning, TEXT("RANDOM tileIndex: %d"), tileIndex);
		UE_LOG(LogTemp, Warning, TEXT("RANDOM Biomes: %d"), biotope);
	}
	UE_LOG(LogTemp, Warning, TEXT("Tile coordinates: %s"), *biomes[0].coordinates.ToString());

	for (auto& it : inTiles)
	{
		FVector2D currTileCoords(X = it->index % gridSizeOfProxies, FMath::Floor(it->index / gridSizeOfProxies));
		float distance = 200000; //Just a large number 

		for (auto& b: biomes)
		{
			float temp = FVector2D::Distance(currTileCoords, b.coordinates);
			if (distance >= temp && temp <= b.maxBiomeSize)
			{
				it->biotope = b.biomeType;
				distance = temp;
			}
			
		}
	}
}

void CreateLandscape::GenerateHeightMapsForBiotopes(TArray<UTile*>& inTiles, const TArray<TSharedPtr<BiotopePerlinNoiseSetting>>& BiotopeSettings)
{
	//NoiseGenerator<uint16, 64> noise{}; //N is "cell size", 127 is tiny tiles 1009 is large tiles
	//noise.GenerateNoiseValues(2016);
	// 
	//CANT CHANGE CellSize as template?
	//Plains noise
	PerlinNoiseGenerator<uint16, 64> PerlinNoise{};
	PerlinNoise.generateGradients();
	PerlinNoise.generateBiotopeNoise(heightData, SizeX,*BiotopeSettings[1]);
	
	//City noise
	PerlinNoiseGenerator<uint16, 64> PerlinNoisePlains{};
	PerlinNoisePlains.generateGradients();
	PerlinNoisePlains.generateBiotopeNoise(cityHeightData, SizeX, *BiotopeSettings[0]);

	//Mountain noise
	PerlinNoiseGenerator<uint16, 64> PerlinNoiseMountains{};
	PerlinNoiseMountains.generateGradients();
	PerlinNoiseMountains.generateBiotopeNoise(mountainHeightData, SizeX, *BiotopeSettings[2]);


	UE_LOG(LogTemp, Warning, TEXT("Heigthdata value: %d"), heightData[200000]);

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

	TArray<uint16> concatedHeightData;
	//concatedHeightData.SetNum(SizeX * SizeY);
	concatHeightData(inTiles, concatedHeightData);

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


//OLD COSINE INTERP CODE
					// currentRowHeight = GetRowOfHeightData(t->tileHeightData, 64, 0 + int_steps);
					// adjRowHeight = GetRowOfHeightData(t->adjacentTiles[i]->tileHeightData, 64, 63 - int_steps);

					//TArray<uint16> interpArray;
					//mu = 1.0;
					//for (int k = int_steps -1; k > 0; k--) {	//looping rows 6-1
					//	//mu = mu / k;
					//	//for (int j = 0; j < 64; j++) {
					//	//	float mu2 = (1 - cos(mu * PI)) / 2;
					//	//	//This is for cosine interpolation
					//	//	interpArray.Add(currentRowHeight[j] * (1 - mu2) + temp[j] * mu2);
					//	//	//This is for linear interpolation
					//	//	//temp.Add((adjRowHeight[k] + currentRowHeight[k]) / 2);
					//	//}
					//	//SetRowHeightData(t->tileHeightData, temp, 64, 0 + k);


					//	currentRowHeight = GetRowOfHeightData(t->tileHeightData, 64, int_steps - k);
					//	for (int j = 0; j < 64; j++) {
					//		float mu2 = (1 - cos(mu * PI)) / 2;
					//		//This is for cosine interpolation
					//		interpArray.Add((currentRowHeight[j] + temp[j]) / 2);
					//		//This is for linear interpolation
					//		//temp.Add((adjRowHeight[k] + currentRowHeight[k]) / 2);
					//	}
					//	SetRowHeightData(t->tileHeightData, temp, 64, 0 + k);
					//	interpArray.Empty();
					//}

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