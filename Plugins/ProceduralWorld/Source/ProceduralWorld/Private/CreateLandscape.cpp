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

uint32 CreateLandscape::GetVertexIndex(int32 dataDimension, int32 inX, int32 inY) const
{
	if (inX <= dataDimension && inY <= dataDimension)
	{
		
		return inX * dataDimension + inY;

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Coordinates do not fit"));
		return -1;
	}
	

	
}

void CreateLandscape::concatHeightData(const TArray<UTile*> &inTiles)
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
					concatedHeightData.RemoveAt(heightDataIndex,1,true);
					
				}

				concatedHeightData.Append(GetColumnOfHeightData(tilePtr->tileHeightData,TileSize,i));
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

void CreateLandscape::roadAnamarphosis(const TArray<Road>& roads, float const sigma, int kernelSize, int interpolationPadding)
{
	//Create kernel
	TArray<kernelElement> kernel;

	int firstIndex = floor(kernelSize / 2);

	//create weights
	float weight;
	float sumWeights = 0;
	//float sigma = 0.3;

	for (int x = -firstIndex; x <= firstIndex; x++) {

		for (int y = -firstIndex; y <= firstIndex; y++) {

			weight = (1 / (2 * PI * pow(sigma, 2)) * pow(EULERS_NUMBER, -(pow(abs(x), 2) + pow(abs(y), 2)) / 2 * pow(sigma, 2)));
			kernel.Add(kernelElement(weight, FVector2D(x, y)));
			sumWeights += weight;
		}

	}

	//Iterate through the road(s) and its internal splines (double foor loop)
	//sp is NOT a control point, but rather a point on the spline curve
	ControlPoint sp;
	int X;
	int Y;
	float weightedKernelVertex;
		for(auto& r: roads)
		{
			for(auto& s: r.splinePaths )
			{
				for (float t = 0; t < s.TotalLength; t++) {
					sp = s.GetSplinePoint(s.GetNormalisedOffset(t));
					X = FGenericPlatformMath::RoundToInt(sp.pos.X);
					Y = FGenericPlatformMath::RoundToInt(sp.pos.Y);
					//Check that the road is not outside of the landscape
					if(X < 0 || X >= SizeX || Y < 0 || Y >= SizeY){
						break;
					}

					//Iterate through road kernel
					for (size_t xRoad = (X-(r.Width-1)/2 + interpolationPadding); xRoad < (X + (r.Width - 1) / 2 + interpolationPadding); xRoad++)
					{
						for (size_t yRoad = (Y - (r.Width - 1) / 2 + interpolationPadding); yRoad < (Y + (r.Width - 1) / 2 + interpolationPadding); yRoad++)
						{
							//Iterate through Gauss kernel
							if (xRoad >= 0 && xRoad < SizeX && yRoad >= 0 && yRoad < SizeX) {
								weightedKernelVertex = 0;
								//Iterate through Gauss kernel
								for (int j = 0; j < kernelSize * kernelSize; j++) {
									if (0 <= (kernel[j].coords.X + xRoad) && (kernel[j].coords.X + xRoad) < SizeX && 0 <= (kernel[j].coords.Y + yRoad) && (kernel[j].coords.Y + yRoad) < SizeY)
									{
										weightedKernelVertex += (kernel[j].weight / sumWeights) * concatedHeightData[GetVertexIndex(SizeX, kernel[j].coords.X + xRoad, kernel[j].coords.Y + yRoad)];
									}
									else {
										weightedKernelVertex += (kernel[j].weight / sumWeights) * concatedHeightData[GetVertexIndex(SizeX, X, Y)]; //This can crash
									}
								}
								concatedHeightData[GetVertexIndex(SizeX, xRoad, yRoad)] = weightedKernelVertex;
							}
						}
					}
				}
				////kenrel for pass 2
				//kernel.Empty();
				//sumWeights = 0;
				//kernelSize += 4;
				//firstIndex = floor((kernelSize) / 2);
				//sigma = 0.8;
				//for (int x = -firstIndex; x <= firstIndex; x++) {

				//	for (int y = -firstIndex; y <= firstIndex; y++) {

				//		weight = (1 / (2 * PI * pow(sigma, 2)) * pow(EULERS_NUMBER, -(pow(abs(x), 2) + pow(abs(y), 2)) / 2 * pow(sigma, 2)));
				//		kernel.Add(kernelElement(weight, FVector2D(x, y)));
				//		sumWeights += weight;
				//	}

				//}
				////pass 2
				//for (float t = 0.5; t < s.TotalLength; t++) {
				//	sp = s.GetSplinePoint(s.GetNormalisedOffset(t));
				//	X = FGenericPlatformMath::RoundToInt(sp.pos.X);
				//	Y = FGenericPlatformMath::RoundToInt(sp.pos.Y);
				//	//Check that the road is not outside of the landscape
				//	if (X < 0 || X > SizeX || Y < 0 || Y > SizeY) {
				//		break;
				//	}
				//	//Iterate through road kernel
				//	for (size_t xRoad = (X - (r.Width - 1)); xRoad < (X + (r.Width - 1)); xRoad++)
				//	{
				//		for (size_t yRoad = (Y - (r.Width - 1)); yRoad < (Y + (r.Width - 1)); yRoad++)
				//		{
				//			if (xRoad >= 0 && xRoad < SizeX && yRoad >= 0 && yRoad < SizeX) {
				//					weightedKernelVertex = 0;
				//			//Iterate through Gauss kernel
				//			for (int j = 0; j < kernelSize * kernelSize; j++) {
				//				if (0 <= (kernel[j].coords.X + xRoad) && (kernel[j].coords.X + xRoad) < SizeX && 0 <= (kernel[j].coords.Y + yRoad) && (kernel[j].coords.Y + yRoad) < SizeY)
				//				{
				//					weightedKernelVertex += (kernel[j].weight / sumWeights) * concatedHeightData[GetVertexIndex(SizeX, kernel[j].coords.X + xRoad, kernel[j].coords.Y + yRoad)];
				//				}
				//				else {
				//					weightedKernelVertex += (kernel[j].weight / sumWeights) * concatedHeightData[GetVertexIndex(SizeX, X, Y)];
				//				}
				//			}
				//			concatedHeightData[GetVertexIndex(SizeX, xRoad, yRoad)] = weightedKernelVertex;
				//			}
						
				//		}
				//	}
				//}
				////kenrel for pass 3
				//kernel.Empty();
				//sumWeights = 0;
				//kernelSize += 1;
				//firstIndex = floor((kernelSize) / 2);
				//sigma = 1;
				//for (int x = -firstIndex; x <= firstIndex; x++) {

				//	for (int y = -firstIndex; y <= firstIndex; y++) {

				//		weight = (1 / (2 * PI * pow(sigma, 2)) * pow(EULERS_NUMBER, -(pow(abs(x), 2) + pow(abs(y), 2)) / 2 * pow(sigma, 2)));
				//		kernel.Add(kernelElement(weight, FVector2D(x, y)));
				//		sumWeights += weight;
				//	}

				//}
				////pass 3
				//for (float t = 0; t < s.TotalLength; t++) {
				//	sp = s.GetSplinePoint(s.GetNormalisedOffset(t));
				//	X = FGenericPlatformMath::RoundToInt(sp.pos.X);
				//	Y = FGenericPlatformMath::RoundToInt(sp.pos.Y);
				//	//Check that the road is not outside of the landscape
				//	if (X < 0 || X > SizeX || Y < 0 || Y > SizeY) {
				//		break;
				//	}
				//	//Iterate through road kernel
				//	for (size_t xRoad = (X - (r.Width - 1)+1); xRoad < (X + (r.Width - 1)+1); xRoad++)
				//	{
				//		for (size_t yRoad = (Y - (r.Width - 1)+1); yRoad < (Y + (r.Width - 1)+1); yRoad++)
				//		{
				//			if (xRoad >= 0 && xRoad < SizeX && yRoad >= 0 && yRoad < SizeX) {
				//				weightedKernelVertex = 0;
				//				//Iterate through Gauss kernel
				//				for (int j = 0; j < kernelSize * kernelSize; j++) {
				//					if (0 <= (kernel[j].coords.X + xRoad) && (kernel[j].coords.X + xRoad) < SizeX && 0 <= (kernel[j].coords.Y + yRoad) && (kernel[j].coords.Y + yRoad) < SizeY)
				//					{
				//						weightedKernelVertex += (kernel[j].weight / sumWeights) * concatedHeightData[GetVertexIndex(SizeX, kernel[j].coords.X + xRoad, kernel[j].coords.Y + yRoad)];
				//					}
				//					else {
				//						weightedKernelVertex += (kernel[j].weight / sumWeights) * concatedHeightData[GetVertexIndex(SizeX, X, Y)];
				//					}
				//				}
				//				concatedHeightData[GetVertexIndex(SizeX, xRoad, yRoad)] = weightedKernelVertex;
				//			}
				//		}
				//	}
				//}
				
			}
		}
}

void CreateLandscape::generateRoadSmart(const TArray<UTile*>& inTiles, TArray<Road> &inRoads)
{
	FMath math;
	uint16 tileIndex = 0;

	do {
		tileIndex = math.RandRange(0, gridSizeOfProxies * gridSizeOfProxies - 1);

	} while (inTiles[tileIndex]->biotope == 2);
	//Now we have found a sutiable tile to start with
	CRSpline spline;

	int32 X = inTiles[tileIndex]->index % gridSizeOfProxies * (inTiles[tileIndex]->tileSize - 1);
	int32 Y = FMath::Floor(inTiles[tileIndex]->index / gridSizeOfProxies) * (inTiles[tileIndex]->tileSize - 1);

	//tangent
	spline.addControlPoint({ (float)math.RandRange(X, X + inTiles[tileIndex]->tileSize - 1), (float)math.RandRange(Y, Y + inTiles[tileIndex]->tileSize - 1),(float)35000 });
	//first control point
	spline.addControlPoint({ (float)math.RandRange(X, X + inTiles[tileIndex]->tileSize - 1),(float)math.RandRange(Y, Y + inTiles[tileIndex]->tileSize - 1),(float)35000 });

	ControlPoint startCP = spline.points.Last();

	uint16 startHeight = concatedHeightData[GetVertexIndex(SizeX, FGenericPlatformMath::RoundToInt(startCP.pos.X), FGenericPlatformMath::RoundToInt(startCP.pos.Y))];

	//Move to random adjacent tiles but also check if the control point is in a "good" location, meaning check that its not on a hill
	//Can be done by randomly place the CP but then iterate the segment and check the height of the heightmap, thus detecting hills and such
	int maxRoadTiles{ 24 };
	int AdjTries{ 100 };
	int randomPointTries = 200;
	int32 tileSize;
	int oldTileIndex = 0;
	TArray<uint16> visitedTiles{tileIndex}; //used to not iterate to the same again
	
	while (maxRoadTiles > 0 && AdjTries > 0) {
		AdjTries--;
		
		int32 adjIndex = math.RandRange(0, 7);
		if (inTiles[tileIndex]->adjacentTiles[adjIndex] && inTiles[tileIndex]->adjacentTiles[adjIndex]->biotope != 2 && randomPointTries > 0 && !(visitedTiles.Contains(inTiles[tileIndex]->adjacentTiles[adjIndex]->index)))
		{
			randomPointTries--;
			oldTileIndex = tileIndex;
			tileIndex = inTiles[tileIndex]->adjacentTiles[adjIndex]->index;
			tileSize = inTiles[tileIndex]->tileSize;
			X = tileIndex % gridSizeOfProxies * (tileSize - 1);
			Y = FMath::Floor(tileIndex / gridSizeOfProxies) * (tileSize - 1);
			
			//Random cp
			spline.addControlPoint({ (float)math.RandRange(X,X + tileSize - 1),(float)math.RandRange(Y,Y + tileSize - 1),(float)35000 });
			//Random Tangent
			spline.addControlPoint({ (float)math.RandRange(X,X + tileSize - 1),(float)math.RandRange(Y,Y + tileSize - 1),(float)35000 });

			//Iterate the new curve segment
			spline.calcLengths();
			float heightDifference = 0;
			float startLength = 0;
			if(spline.points.Num() > 4)
			{
				for (int i = 0; i > spline.points.Num()-2; i++) {
					startLength += spline.points[i].length;
				}

			}
			int threshold = 1300;
			bool canSpawn = true;
			ControlPoint nextSP;
			for (; startLength < spline.TotalLength; startLength += 2) {
				nextSP = spline.GetSplinePoint(spline.GetNormalisedOffset(startLength));
				
				//Check so that the point exists within the map boundaries
				if(0 > FGenericPlatformMath::RoundToInt(nextSP.pos.X) || SizeX <= FGenericPlatformMath::RoundToInt(nextSP.pos.X) || 
				   0 > FGenericPlatformMath::RoundToInt(nextSP.pos.Y) || SizeY <= FGenericPlatformMath::RoundToInt(nextSP.pos.Y))
				{
					canSpawn = false;
					break;
				}
				heightDifference = abs(startHeight -
					concatedHeightData[GetVertexIndex(SizeX, FGenericPlatformMath::RoundToInt(nextSP.pos.X), FGenericPlatformMath::RoundToInt(nextSP.pos.Y))])*(100.0f/128.0f);
			/*	UE_LOG(LogTemp, Warning, TEXT("heightDiff = %f"), heightDifference);*/
				if (heightDifference > threshold) {
					UE_LOG(LogTemp, Warning, TEXT("Attempt at creating spline failed, to much height difference!"));
					canSpawn = false;
					spline.points.RemoveAt(spline.points.Num() - 1);
					spline.points.RemoveAt(spline.points.Num() - 1);
					tileIndex = oldTileIndex;
					break;
				}

			}
			if(canSpawn) {
				UE_LOG(LogTemp, Warning, TEXT("Added a spline segment succesfully"));
				visitedTiles.Add(tileIndex);
				spline.points.RemoveAt(spline.points.Num() - 1); //remove tangent
				startHeight = concatedHeightData[GetVertexIndex(SizeX, FGenericPlatformMath::RoundToInt(spline.points[spline.points.Num() - 2].pos.X), FGenericPlatformMath::RoundToInt(spline.points[spline.points.Num() - 2].pos.Y))];
				maxRoadTiles--;
				randomPointTries = 200;
				AdjTries = 100;
			}
			
			
		}

	}
	
	if(spline.points.Num() >= 4){
		inRoads.Add(spline);
	}


}

void CreateLandscape::interpBiomes(TArray<UTile*>& inTiles, int kernelSize, float sigma, int32 interpWidth, int32 passes)
{
	int dynamicStep = interpWidth;
	for (int i = 0; i < passes; i++) {

		interpGaussianBlur(inTiles, kernelSize, sigma / (i + 1), dynamicStep);
		dynamicStep -= dynamicStep / passes;

	}

	interpGaussianBlur(inTiles, kernelSize, sigma/interpWidth, interpWidth+1);
}


//sigma (deviation)
void CreateLandscape::interpGaussianBlur(TArray<UTile*>& inTiles, int kernelSize, float sigma, int32 interpWidth)
{
	
	TArray<kernelElement> kernel;
	//kernel.SetNum(kernelSize * kernelSize);

	int firstIndex = floor(kernelSize / 2);

	//create weights
	float weight;
	int index = 0;
	float sumWeights = 0;

	for (int x = -firstIndex; x <= firstIndex; x++) {

		for (int y = -firstIndex; y <= firstIndex; y++) {
			
			weight = (1 / (2 * PI * pow(sigma, 2)) * pow(EULERS_NUMBER, -(pow(abs(x), 2) + pow(abs(y), 2)) / 2 * pow(sigma, 2)));
			//kernel[index] = kernelElement(weight,FVector2D(x,y));
			kernel.Add(kernelElement(weight, FVector2D(x, y)));
			sumWeights += weight;
		}

	} 

	//(Only check every other tile)
	int rowLength = GetGridSizeOfProxies();
	int rowCount = 0;
	int X;
	int Y;
	int yStart;
	int xStart;
	float weightedKernelVertex;
	
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
		

		//Find biomes edges that needs interpolation 
		for (int i = 0; i < 8; i++) {
			if (t->adjacentTiles[i] != nullptr && t->biotope != t->adjacentTiles[i]->biotope) {

				//if (i == 0 && t->adjacentTiles[0]->biotope == t->adjacentTiles[1]->biotope && t->adjacentTiles[0]->biotope == t->adjacentTiles[3]->biotope ) { //top right corner (all 3 other biotopes are different to t but of same different type) 
				//	UE_LOG(LogTemp, Warning, TEXT("Interpolating top corner of tile : %d"), t->index);
				//	X = t->index % gridSizeOfProxies * (TileSize - 1);
				//	Y = FMath::Floor(t->index / gridSizeOfProxies) * (TileSize - 1);

				//	//Iteratethrough all interpolation points columns/rows 
				//	yStart = Y - interpWidth;
				//	xStart = X - interpWidth;
				//	for (int c = xStart; c < X; c++) {	//Iterate X (Rolumn)
				//		for (int r = yStart; r < Y; r++) { //Iterate Y (Row)
				//			weightedKernelVertex = 0;

				//			for (int j = 0; j < kernelSize * kernelSize; j++) {

				//				if ((kernel[j].coords.X + c) < 0 || (kernel[j].coords.Y + r) < 0 || (kernel[j].coords.X + c) >= SizeX || (kernel[j].coords.Y + r) >= SizeX)	//NEEDS TO BE FIXED, LAZY PADDING WITH HARD CODED VALUE
				//				{
				//					weightedKernelVertex += (kernel[j].weight / sumWeights) * concatedHeightData[GetVertexIndex(concatedHeightData, SizeX, c, r)];
				//				}
				//				else
				//				{
				//					weightedKernelVertex += (kernel[j].weight / sumWeights) * concatedHeightData[GetVertexIndex(concatedHeightData, SizeX, kernel[j].coords.X + c, kernel[j].coords.Y + r)];
				//				}
				//			}

				//			concatedHeightData[GetVertexIndex(concatedHeightData, SizeX, c, r)] = weightedKernelVertex;
				//		}
				//	}
				//}

				if(i == 1){ //top 
				/*	UE_LOG(LogTemp, Warning, TEXT("Interpolating from tile : %d"), t->index);
					UE_LOG(LogTemp, Warning, TEXT("Interpolating adjacent tile : : %d"), i); */
					X = t->index % gridSizeOfProxies * (TileSize -1);
					Y = FMath::Floor(t->index / gridSizeOfProxies) * (TileSize - 1);
					/*UE_LOG(LogTemp, Warning, TEXT("Gauss X: %d"), X);
					UE_LOG(LogTemp, Warning, TEXT("Gauss Y: %d"), Y);*/

					//Iteratethrough all interpolation points columns/rows 
					yStart = Y - interpWidth;

					for (int c = X; c <= X + (TileSize -1); c++) {	//Iterate X (Rolumn)
						for (int r = yStart; r <= yStart + (interpWidth*2) ; r++) { //Iterate Y (Row)
							weightedKernelVertex = 0;

							for (int j = 0; j < kernelSize * kernelSize; j++) {

								//kernel[j].coords.X = X;
								//kernel[j].coords.Y = yStart;
								if ((kernel[j].coords.X + c) < 0 || (kernel[j].coords.Y + r) < 0 || (kernel[j].coords.X + c) >= SizeX || (kernel[j].coords.Y + r) >= SizeX)	//NEEDS TO BE FIXED, LAZY PADDING WITH HARD CODED VALUE
								{
									weightedKernelVertex += (kernel[j].weight / sumWeights) * concatedHeightData[GetVertexIndex( SizeX, c, r)];
								}
								else
								{
									weightedKernelVertex += (kernel[j].weight / sumWeights) * concatedHeightData[GetVertexIndex( SizeX, kernel[j].coords.X + c, kernel[j].coords.Y + r)];
								}
							}
							
							concatedHeightData[GetVertexIndex(SizeX, c, r)] = weightedKernelVertex;
						}
					}

				}
				if (i == 3) { //right
				/*	UE_LOG(LogTemp, Warning, TEXT("Interpolating from tile : %d"), t->index);
					UE_LOG(LogTemp, Warning, TEXT("Interpolating adjacent tile : : %d"), i);*/
					X = t->index % gridSizeOfProxies * (TileSize - 1);
					Y = FMath::Floor(t->index / gridSizeOfProxies) * (TileSize - 1);
					
					//Iteratethrough all interpolation points columns/rows 
					yStart = Y;
					xStart = X - interpWidth;
					for (int c = xStart; c <= xStart + (interpWidth *2); c++) {	//Iterate X (Rolumn)
						for (int r = Y; r <= Y + (TileSize - 1); r++) { //Iterate Y (Row)
							weightedKernelVertex = 0;

							for (int j = 0; j < kernelSize * kernelSize; j++) {

								//kernel[j].coords.X = X;
								//kernel[j].coords.Y = yStart;
								if ((kernel[j].coords.X + c) < 0 || (kernel[j].coords.Y + r) < 0 || (kernel[j].coords.X + c) >= SizeX || (kernel[j].coords.Y + r) >= SizeX)	//NEEDS TO BE FIXED, LAZY PADDING WITH HARD CODED VALUE
								{
									weightedKernelVertex += (kernel[j].weight / sumWeights) * concatedHeightData[GetVertexIndex(SizeX, c, r)];
								}
								else
								{
									weightedKernelVertex += (kernel[j].weight / sumWeights) * concatedHeightData[GetVertexIndex(SizeX, kernel[j].coords.X + c, kernel[j].coords.Y + r)];
								}
							}

							concatedHeightData[GetVertexIndex(SizeX, c, r)] = weightedKernelVertex;
						}
					}

				}
				if (i == 4) { //left
			/*		UE_LOG(LogTemp, Warning, TEXT("Interpolating from tile : %d"), t->index);
					UE_LOG(LogTemp, Warning, TEXT("Interpolating adjacent tile : : %d"), i);*/
					X = t->index % gridSizeOfProxies * (TileSize - 1);
					Y = FMath::Floor(t->index / gridSizeOfProxies) * (TileSize - 1);

					//Iteratethrough all interpolation points columns/rows 
					yStart = Y;
					xStart = X - interpWidth + TileSize - 1;
					for (int c = xStart; c <= X + TileSize - 1 + interpWidth; c++) {	//Iterate X (Rolumn)
						for (int r = yStart; r <= Y + (TileSize - 1); r++) { //Iterate Y (Row)
							weightedKernelVertex = 0;

							for (int j = 0; j < kernelSize * kernelSize; j++) {

								//kernel[j].coords.X = X;
								//kernel[j].coords.Y = yStart;
								if ((kernel[j].coords.X + c) < 0 || (kernel[j].coords.Y + r) < 0 || (kernel[j].coords.X + c) >= SizeX || (kernel[j].coords.Y + r) >= SizeX)	//NEEDS TO BE FIXED, LAZY PADDING WITH HARD CODED VALUE
								{
									weightedKernelVertex += (kernel[j].weight / sumWeights) * concatedHeightData[GetVertexIndex(SizeX,  c, r)];
								}
								else
								{
									weightedKernelVertex += (kernel[j].weight / sumWeights) * concatedHeightData[GetVertexIndex(SizeX, kernel[j].coords.X + c, kernel[j].coords.Y + r)];
								}
							}

							concatedHeightData[GetVertexIndex(SizeX, c, r)] = weightedKernelVertex;
						}
					}

				}
				if (i == 6) { //bottom
	/*				UE_LOG(LogTemp, Warning, TEXT("Interpolating from tile : %d"), t->index);
					UE_LOG(LogTemp, Warning, TEXT("Interpolating adjacent tile : : %d"), i);*/
					X = t->index % gridSizeOfProxies * (TileSize - 1);
					Y = FMath::Floor(t->index / gridSizeOfProxies) * (TileSize - 1);

					//Iteratethrough all interpolation points columns/rows 
					yStart = Y - interpWidth + (TileSize - 1);
					//xStart = X - interpWidth + TileSize - 1;
					for (int c = X; c <= X + (TileSize - 1); c++) {	//Iterate X (Rolumn)
						for (int r = yStart; r <= Y + (TileSize - 1) + (interpWidth *2); r++) { //Iterate Y (Row)
							weightedKernelVertex = 0;

							for (int j = 0; j < kernelSize * kernelSize; j++) {

								//kernel[j].coords.X = X;
								//kernel[j].coords.Y = yStart;
								if ((kernel[j].coords.X + c) < 0 || (kernel[j].coords.Y + r) < 0 || (kernel[j].coords.X + c) >= SizeX || (kernel[j].coords.Y + r) >= SizeX)	//NEEDS TO BE FIXED, LAZY PADDING WITH HARD CODED VALUE
								{
									weightedKernelVertex += (kernel[j].weight / sumWeights) * concatedHeightData[GetVertexIndex(SizeX, c, r)];
								}
								else
								{
									weightedKernelVertex += (kernel[j].weight / sumWeights) * concatedHeightData[GetVertexIndex(SizeX, kernel[j].coords.X + c, kernel[j].coords.Y + r)];
								}
							}

							concatedHeightData[GetVertexIndex(SizeX, c, r)] = weightedKernelVertex;
						}
					}

				}

			}
		}
	}

	//New loop for all corner tiles
	for (auto& t : inTiles)
	{
		//Top right adjacent corner
		if (t->adjacentTiles[0] != nullptr && t->biotope != t->adjacentTiles[0]->biotope && t->adjacentTiles[0]->biotope == t->adjacentTiles[1]->biotope && t->adjacentTiles[0]->biotope == t->adjacentTiles[3]->biotope) {

			UE_LOG(LogTemp, Warning, TEXT("Interpolating top right corner of tile : %d"), t->index);
			X = t->index % gridSizeOfProxies * (TileSize - 1);
			Y = FMath::Floor(t->index / gridSizeOfProxies) * (TileSize - 1);

			//Iteratethrough all interpolation points columns/rows 
			yStart = Y - interpWidth;
			xStart = X - interpWidth;
			for (int c = xStart; c < X; c++) {	//Iterate X (Rolumn)
				for (int r = yStart; r < Y; r++) { //Iterate Y (Row)
					weightedKernelVertex = 0;

					for (int j = 0; j < kernelSize * kernelSize; j++) {

						if ((kernel[j].coords.X + c) < 0 || (kernel[j].coords.Y + r) < 0 || (kernel[j].coords.X + c) >= SizeX || (kernel[j].coords.Y + r) >= SizeX)	//NEEDS TO BE FIXED, LAZY PADDING WITH HARD CODED VALUE
						{
							weightedKernelVertex += (kernel[j].weight / sumWeights) * concatedHeightData[GetVertexIndex(SizeX, c, r)];
						}
						else
						{
							weightedKernelVertex += (kernel[j].weight / sumWeights) * concatedHeightData[GetVertexIndex(SizeX, kernel[j].coords.X + c, kernel[j].coords.Y + r)];
						}
					}

					concatedHeightData[GetVertexIndex(SizeX, c, r)] = weightedKernelVertex;
				}
			}

		}
		//top left adjacent corner
		if (t->adjacentTiles[2] != nullptr && t->biotope != t->adjacentTiles[2]->biotope && t->adjacentTiles[2]->biotope == t->adjacentTiles[1]->biotope && t->adjacentTiles[2]->biotope == t->adjacentTiles[4]->biotope) {

			UE_LOG(LogTemp, Warning, TEXT("Interpolating top left corner of tile : %d"), t->index);
			X = t->index % gridSizeOfProxies * (TileSize - 1);
			Y = FMath::Floor(t->index / gridSizeOfProxies) * (TileSize - 1);

			//Iteratethrough all interpolation points columns/rows 
			yStart = Y - interpWidth;
			xStart = X + TileSize;
			for (int c = xStart + interpWidth; c < X; c++) {	//Iterate X (Rolumn)
				for (int r = yStart; r < Y; r++) { //Iterate Y (Row)
					weightedKernelVertex = 0;

					for (int j = 0; j < kernelSize * kernelSize; j++) {

						if ((kernel[j].coords.X + c) < 0 || (kernel[j].coords.Y + r) < 0 || (kernel[j].coords.X + c) >= SizeX || (kernel[j].coords.Y + r) >= SizeX)	//NEEDS TO BE FIXED, LAZY PADDING WITH HARD CODED VALUE
						{
							weightedKernelVertex += (kernel[j].weight / sumWeights) * concatedHeightData[GetVertexIndex(SizeX, c, r)];
						}
						else
						{
							weightedKernelVertex += (kernel[j].weight / sumWeights) * concatedHeightData[GetVertexIndex(SizeX, kernel[j].coords.X + c, kernel[j].coords.Y + r)];
						}
					}

					concatedHeightData[GetVertexIndex(SizeX, c, r)] = weightedKernelVertex;
				}
			}

		}
		//Bottom right adjacent corner
		if (t->adjacentTiles[5] != nullptr && t->biotope != t->adjacentTiles[5]->biotope && t->adjacentTiles[5]->biotope == t->adjacentTiles[3]->biotope && t->adjacentTiles[5]->biotope == t->adjacentTiles[6]->biotope) {

			UE_LOG(LogTemp, Warning, TEXT("Interpolating bottom right corner of tile : %d"), t->index);
			X = t->index % gridSizeOfProxies * (TileSize - 1);
			Y = FMath::Floor(t->index / gridSizeOfProxies) * (TileSize - 1);

			//Iteratethrough all interpolation points columns/rows 
			yStart = Y + TileSize;
			xStart = X - interpWidth;
			for (int c = xStart; c < X; c++) {	//Iterate X (Rolumn)
				for (int r = yStart; r < yStart + interpWidth; r++) { //Iterate Y (Row)
					weightedKernelVertex = 0;

					for (int j = 0; j < kernelSize * kernelSize; j++) {

						if ((kernel[j].coords.X + c) < 0 || (kernel[j].coords.Y + r) < 0 || (kernel[j].coords.X + c) >= SizeX || (kernel[j].coords.Y + r) >= SizeX)	//NEEDS TO BE FIXED, LAZY PADDING WITH HARD CODED VALUE
						{
							weightedKernelVertex += (kernel[j].weight / sumWeights) * concatedHeightData[GetVertexIndex( SizeX, c, r)];
						}
						else
						{
							weightedKernelVertex += (kernel[j].weight / sumWeights) * concatedHeightData[GetVertexIndex(SizeX, kernel[j].coords.X + c, kernel[j].coords.Y + r)];
						}
					}

					concatedHeightData[GetVertexIndex(SizeX, c, r)] = weightedKernelVertex;
				}
			}

		}
		//Bottom left adjacent corner
		if (t->adjacentTiles[7] != nullptr && t->biotope != t->adjacentTiles[7]->biotope && t->adjacentTiles[7]->biotope == t->adjacentTiles[4]->biotope && t->adjacentTiles[7]->biotope == t->adjacentTiles[6]->biotope) {

			UE_LOG(LogTemp, Warning, TEXT("Interpolating bottom left corner of tile : %d"), t->index);
			X = t->index % gridSizeOfProxies * (TileSize - 1);
			Y = FMath::Floor(t->index / gridSizeOfProxies) * (TileSize - 1);

			//Iteratethrough all interpolation points columns/rows 
			yStart = Y + TileSize;
			xStart = X + TileSize;
			for (int c = xStart; c < xStart + interpWidth; c++) {	//Iterate X (Rolumn)
				for (int r = yStart; r < yStart + interpWidth; r++) { //Iterate Y (Row)
					weightedKernelVertex = 0;

					for (int j = 0; j < kernelSize * kernelSize; j++) {

						if ((kernel[j].coords.X + c) < 0 || (kernel[j].coords.Y + r) < 0 || (kernel[j].coords.X + c) >= SizeX || (kernel[j].coords.Y + r) >= SizeX)	//NEEDS TO BE FIXED, LAZY PADDING WITH HARD CODED VALUE
						{
							weightedKernelVertex += (kernel[j].weight / sumWeights) * concatedHeightData[GetVertexIndex(SizeX, c, r)];
						}
						else
						{
							weightedKernelVertex += (kernel[j].weight / sumWeights) * concatedHeightData[GetVertexIndex(SizeX, kernel[j].coords.X + c, kernel[j].coords.Y + r)];
						}
					}

					concatedHeightData[GetVertexIndex( SizeX, c, r)] = weightedKernelVertex;
				}
			}

		}


		
	}
}

void CreateLandscape::AssignBiotopesToTiles(TArray<UTile*>& inTiles, const int &nmbrOfBiomes, const TArray<TSharedPtr<BiotopePerlinNoiseSetting>>& BiotopeSettings) const
{

	//3 default: city,plains,mountains
	int nmbrOfDifferentBiotopes = BiotopeSettings.Num();
	int nmbrOfTiles = inTiles.Num();
	float X;
	float Y;
	float maxDistance;
	
	TArray<BiomeOriginInformation> biomes;
	FMath mathInstance;
	for (size_t i = 0; i < nmbrOfBiomes; i++)
	{
		int32 biotope = mathInstance.RandRange(0, nmbrOfDifferentBiotopes - 1);	//random type of biotope (0-2)
		int32 tileIndex = mathInstance.RandRange(0, nmbrOfTiles - 1); //Random tile as origin (0-7)

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

		
	}
	//Assign placed biomes to all tiles
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
	FTransform LandscapeTransform{ InRotation, InTranslation, LandscapeScale };
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

	

	HeightDataPerLayers.Add(FGuid(), MoveTemp(rawConcatData));
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
	FVector InTranslation{ 0,0,0 };
	FTransform LandscapeTransform{ InRotation, InTranslation, LandscapeScale };

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

	Landscape->StaticLightingLOD = FMath::DivideAndRoundUp(FMath::CeilLogTwo((SizeX * SizeY) / (2048 * 2048) + 1), (uint32)2);
	
	Landscape->SetActorTransform(LandscapeTransform);
	Landscape->Import(FGuid::NewGuid(), 0, 0, SizeX - 1, SizeY - 1, SectionsPerComponent, QuadsPerComponent,
		HeightDataPerLayers, nullptr, MaterialLayerDataPerLayers, ELandscapeImportAlphamapType::Additive);

	
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

FVector CreateLandscape::GetWorldCoordinates(const TArray<uint16>& inData, int32 inX, int32 inY) const
{
	FVector temp;
	if(inX >= 0 && inX < SizeX && inY >= 0 && inY < SizeY){
		/*temp = { inX * LandscapeScale.X, inY * LandscapeScale.Y, ((float)inData[GetVertexIndex(SizeX, inX, inY)] - 32768) / LandscapeScale.Z };*/
		temp = { inX * LandscapeScale.X, inY * LandscapeScale.Y, (inData[GetVertexIndex(SizeX, inX, inY)]-32768)*(100.0f/128.0f)};
		UE_LOG(LogTemp, Warning, TEXT("HeightData at position inX and inY : %d"), inData[GetVertexIndex(SizeX, inX, inY)]);
		UE_LOG(LogTemp, Warning, TEXT("Computed height : %f"), (inData[GetVertexIndex(SizeX, inX, inY)] - 32768) * (100.0f / 128.0f));
		return temp;
	}else{
		UE_LOG(LogTemp, Warning, TEXT("GetWorldCoordinates: coordinates are out of range! "));
		return { 0.0f, 0.0f, 0.0f };
	}


}

const uint32 CreateLandscape::GetGridSizeOfProxies() const
{
	
	return gridSizeOfProxies;
}
