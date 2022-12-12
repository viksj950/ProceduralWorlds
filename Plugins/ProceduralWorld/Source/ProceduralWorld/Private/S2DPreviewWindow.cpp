// Fill out your copyright notice in the Description page of Project Settings.


#include "S2DPreviewWindow.h"

S2DPreviewWindow::S2DPreviewWindow(const int32& inSizeX, const int32& inSizeY, const int32& inQuadsPerComponent,
	const int32& inComponentsPerProxy, const int32& inSectionsPerComponent, const int32& inTilesSize): SizeX{inSizeX}, SizeY{inSizeY}, QuadsPerComponent{inQuadsPerComponent},
	ComponentsPerProxy{ inComponentsPerProxy }, SectionsPerComponent{ inSectionsPerComponent }, TileSize{ inTilesSize }, displayGrid{true}, displayBiotopes{true}
{
	gridSizeOfProxies = (SizeX - 1) / ((QuadsPerComponent * ComponentsPerProxy));
	//First two slots are always populated by default? heightmap and grid:
	textures.Add(nullptr);
	textures.Add(nullptr);
	textures.Add(nullptr);

	brushes.Add(nullptr);
	brushes.Add(nullptr);
	brushes.Add(nullptr);
}

void S2DPreviewWindow::UpdateLandscapeSettings(const int32& inSizeX, const int32& inSizeY, const int32& inQuadsPerComponent, const int32& inComponentsPerProxy, const int32& inSectionsPerComponent, const int32& inTilesSize)
{
	SizeX = inSizeX;
	SizeY = inSizeY;
	QuadsPerComponent = inQuadsPerComponent;
	ComponentsPerProxy = inComponentsPerProxy;
	SectionsPerComponent = SectionsPerComponent;
	TileSize = inTilesSize;

	gridSizeOfProxies = (SizeX - 1) / ((QuadsPerComponent * ComponentsPerProxy));
}

void S2DPreviewWindow::CreateHeightmapTexture(const TArray<uint16>& inData)
{

	uint8* pixels = (uint8*)malloc(SizeX * SizeY* 4); // x4 because it's RGBA. 4 integers, one for Red, one for Green, one for Blue, one for Alpha

	for (int y = 0; y < SizeY; y++)
	{
		for (int x = 0; x < SizeX; x++)
		{

			
				pixels[x * 4 * SizeX + (SizeY - y - 1) * 4 + 0] = (uint8)(inData[y * SizeX + x] / 255); // R
				pixels[x * 4 * SizeX + (SizeY - y - 1) * 4 + 1] = (uint8)(inData[y * SizeX + x] / 255);  // G
				pixels[x * 4 * SizeX + (SizeY - y - 1) * 4 + 2] = (uint8)(inData[y * SizeX + x] / 255);   // B
				pixels[x * 4 * SizeX + (SizeY - y - 1) * 4 + 3] = 255; // A

		}
	}


	UTexture2D *tempTexturePtr = UTexture2D::CreateTransient(SizeX, SizeY, PF_R8G8B8A8);

	// Passing the pixels information to the texture
	FTexture2DMipMap* Mip = &tempTexturePtr->GetPlatformData()->Mips[0];
	Mip->SizeX = SizeX;
	Mip->SizeY = SizeY;
	Mip->BulkData.Lock(LOCK_READ_WRITE);
	uint8* TextureData = (uint8*)Mip->BulkData.Realloc(SizeY * SizeX * sizeof(uint8) * 4);
	FMemory::Memcpy(TextureData, pixels, sizeof(uint8) * SizeY * SizeX * 4);
	Mip->BulkData.Unlock();

	// Updating Texture & mark it as unsaved
	tempTexturePtr->AddToRoot();
	tempTexturePtr->UpdateResource();
	//Package->MarkPackageDirty();

	free(pixels);
	pixels = NULL;

	//add texture to array of textures //TEMPORARY TO SEE UPDATES!!

	if (textures.IsValidIndex(0))
	{
		textures[0] = tempTexturePtr;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Default pointer for heightmap in texture array is missing"));
	}



}

void S2DPreviewWindow::CreateGridTexture()
{

	/*int32 gridTextureSize = 505;
	int32 tileScale = SizeX - ( SizeX % gridTextureSize)*/

	uint8* pixels = (uint8*)malloc(SizeX * SizeY * 4); // x4 because it's RGBA. 4 integers, one for Red, one for Green, one for Blue, one for Alpha

	for (int y = 0; y < SizeY; y++)
	{
		for (int x = 0; x < SizeX; x++)
		{

			//3 pixel thick grid
			if (x % (TileSize - 1) == 0 || y % (TileSize - 1) == 0 || x % (TileSize - 1) == 1 || y % (TileSize - 1) == 1 || x % (TileSize - 1) == (TileSize - 2) || y % (TileSize - 1) == (TileSize - 2))
			{
				
				

						pixels[x * 4 * SizeX + (SizeY - y - 1) * 4 + 0] = 0;
						pixels[x * 4 * SizeX + (SizeY - y - 1) * 4 + 1] = 0;
						pixels[x * 4 * SizeX + (SizeY - y - 1) * 4 + 2] = 255;
						pixels[x * 4 * SizeX + (SizeY - y - 1) * 4 + 3] = 255;

					

					
			}
			else {

				pixels[x * 4 * SizeX + (SizeY - y - 1) * 4 + 0] = 0;
				pixels[x * 4 * SizeX + (SizeY - y - 1) * 4 + 1] = 0;
				pixels[x * 4 * SizeX + (SizeY - y - 1) * 4 + 2] = 0;
				pixels[x * 4 * SizeX + (SizeY - y - 1) * 4 + 3] = 0;

			}

		}
	}


	UTexture2D* tempTexturePtr = UTexture2D::CreateTransient(SizeX, SizeY, PF_R8G8B8A8);

	// Passing the pixels information to the texture
	FTexture2DMipMap* Mip = &tempTexturePtr->GetPlatformData()->Mips[0];
	Mip->SizeX = SizeX;
	Mip->SizeY = SizeY;
	Mip->BulkData.Lock(LOCK_READ_WRITE);
	uint8* TextureData = (uint8*)Mip->BulkData.Realloc(SizeY * SizeX * sizeof(uint8) * 4);
	FMemory::Memcpy(TextureData, pixels, sizeof(uint8) * SizeY * SizeX * 4);
	Mip->BulkData.Unlock();

	// Updating Texture & mark it as unsaved
	tempTexturePtr->AddToRoot();
	tempTexturePtr->UpdateResource();
	//Package->MarkPackageDirty();

	free(pixels);
	pixels = NULL;

	//add texture to array of textures
	if (textures.IsValidIndex(1))
	{
		textures[2] = tempTexturePtr;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Default pointer for gridTexture in texture array is missing"));
	}

}

void S2DPreviewWindow::CreateBiotopeTexture()
{
	uint8* pixels = (uint8*)malloc(SizeX * SizeY * 4); // x4 because it's RGBA. 4 integers, one for Red, one for Green, one for Blue, one for Alpha


	

	for (int y = 0; y < SizeY; y++)
	{
		for (int x = 0; x < SizeX; x++)
		{

			



				pixels[x * 4 * SizeX + (SizeY - y - 1) * 4 + 0] = 0;
				pixels[x * 4 * SizeX + (SizeY - y - 1) * 4 + 1] = 0;
				pixels[x * 4 * SizeX + (SizeY - y - 1) * 4 + 2] = 0;
				pixels[x * 4 * SizeX + (SizeY - y - 1) * 4 + 3] = 0;




				

			

		}
	}

	for (auto& it : markedTiles)
	{
		for (uint32 y = FMath::Floor(it.Key / gridSizeOfProxies) * (TileSize - 1); y < (FMath::Floor(it.Key / gridSizeOfProxies) * (TileSize - 1) + (TileSize -1)); y++)
		{
			for (uint32 x = (it.Key % gridSizeOfProxies) * (TileSize - 1); x < (it.Key % gridSizeOfProxies) * (TileSize - 1) + (TileSize - 1); x++)
			{


				


				pixels[y * 4 * SizeX + (SizeX - x) * 4+ 0] = colors[it.Value % colors.Num()].X;
				pixels[y * 4 * SizeX + (SizeX - x) * 4 + 1] = colors[it.Value % colors.Num()].Y;
				pixels[y * 4 * SizeX + (SizeX - x) * 4 + 2] = colors[it.Value % colors.Num()].Z;
				pixels[y * 4 * SizeX + (SizeX - x) * 4 + 3] = 25;

			}
		}
	}

	for (auto& it : markedTilesVoronoi)
	{
		for (uint32 y = FMath::Floor(it.Key / gridSizeOfProxies) * (TileSize - 1); y < (FMath::Floor(it.Key / gridSizeOfProxies) * (TileSize - 1) + (TileSize - 1)); y++)
		{
			for (uint32 x = (it.Key % gridSizeOfProxies) * (TileSize - 1); x < (it.Key % gridSizeOfProxies) * (TileSize - 1) + (TileSize - 1); x++)
			{

				pixels[y * 4 * SizeX + (SizeX - x) * 4 + 3] += 80;

			}
		}
	}

	UTexture2D* tempTexturePtr = UTexture2D::CreateTransient(SizeX, SizeY, PF_R8G8B8A8);

	// Passing the pixels information to the texture
	FTexture2DMipMap* Mip = &tempTexturePtr->GetPlatformData()->Mips[0];
	Mip->SizeX = SizeX;
	Mip->SizeY = SizeY;
	Mip->BulkData.Lock(LOCK_READ_WRITE);
	uint8* TextureData = (uint8*)Mip->BulkData.Realloc(SizeY * SizeX * sizeof(uint8) * 4);
	FMemory::Memcpy(TextureData, pixels, sizeof(uint8) * SizeY * SizeX * 4);
	Mip->BulkData.Unlock();

	// Updating Texture & mark it as unsaved
	tempTexturePtr->AddToRoot();
	tempTexturePtr->UpdateResource();
	//Package->MarkPackageDirty();

	free(pixels);
	pixels = NULL;

	//add texture to array of textures
	if (textures.IsValidIndex(1))
	{
		textures[1] = tempTexturePtr;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Default pointer for BiotopeTexture in texture array is missing"));
	}
}

void S2DPreviewWindow::CreateRoadMarkTexture() 
{
	

}

void S2DPreviewWindow::AssembleWidget()
{
	//TSharedPtr<FSlateImageBrush> tempImageBrush = MakeShared<FSlateImageBrush>(textures[0], FVector2D(textures[0]->GetSizeX(), textures[0]->GetSizeY()), FLinearColor(1.0f, 1.0f, 1.0f, 1.0f), ESlateBrushTileType::NoTile);
	brushes[0] = MakeShared<FSlateImageBrush>(textures[0], FVector2D(textures[0]->GetSizeX(), textures[0]->GetSizeY()), FLinearColor(1.0f, 1.0f, 1.0f, 1.0f), ESlateBrushTileType::NoTile);
	brushes[1] = MakeShared<FSlateImageBrush>(textures[1], FVector2D(textures[1]->GetSizeX(), textures[1]->GetSizeY()), FLinearColor(1.0f, 1.0f, 1.0f, 1.0f), ESlateBrushTileType::NoTile);
	
	brushes[2] = MakeShared<FSlateImageBrush>(textures[2], FVector2D(textures[2]->GetSizeX(), textures[2]->GetSizeY()), FLinearColor(1.0f, 1.0f, 1.0f, 1.0f), ESlateBrushTileType::NoTile);
	//brushes.Add(MakeShared<FSlateImageBrush>(textures[1], FVector2D(textures[1]->GetSizeX(), textures[1]->GetSizeY()), FLinearColor(1.0f, 1.0f, 1.0f, 1.0f), ESlateBrushTileType::NoTile));

		
	/*previewOverlay = SNew(SOverlay);

		for (auto& it: brushes)
		{
			previewOverlay->AddSlot()
				[
					SNew(SBox)

					.MinAspectRatio(1)
				[
					SNew(SImage)

					.Image(it.Get())
				]
				];

		}*/
		previewOverlay = SNew(SOverlay);

		for (size_t i = 0; i < brushes.Num(); i++)
		{
			if (i == 2 && !displayGrid) //Skip drawing grid
			{
				continue;
			}
			if (i == 1 && !displayBiotopes) //Skip drawing biotopes
			{
				continue;
			}
			previewOverlay->AddSlot()
				[
					SNew(SBox)

					.MinAspectRatio(1)
				[
					SNew(SImage)

					.Image(brushes[i].Get())
				]
				];
		}

		previewContext->SetContent(previewOverlay.ToSharedRef());
	

}
void S2DPreviewWindow::MarkTile(int32 selectedBiotope, FVector2D inCoords)
{
	
	//Tmap can only store unique keys, thus will replace if we add an already existing key, which is what we want, we want to override 
	markedTiles.Add(FromCoordToTileIndex(inCoords),selectedBiotope);
	UE_LOG(LogTemp, Log, TEXT("Added a new pair to markedTiles"));
	UE_LOG(LogTemp, Log, TEXT("Marked tiles now contains : %d"), markedTiles.Num());
}
void S2DPreviewWindow::MarkTileVoronoi(int32 selectedBiotope, FVector2D inCoords)
{
	/*int32 X = FromCoordToTileIndex(inCoords) % gridSizeOfProxies;
	int32 Y = FMath::Floor(FromCoordToTileIndex(inCoords) / gridSizeOfProxies);*/

	//Marked Origins of biotopes
	markedTilesVoronoi.Add(FromCoordToTileIndex(inCoords), selectedBiotope);
	markedTiles.Add(FromCoordToTileIndex(inCoords), selectedBiotope);


	for (uint32 i = 0; i < gridSizeOfProxies * gridSizeOfProxies; i++)
	{
		//check so that the tile to be assigned is not marked as an origin of a biome
		if (!markedTilesVoronoi.Contains(i))
		{



			FVector2f currTileCoords(i % gridSizeOfProxies, FMath::Floor(i / gridSizeOfProxies));
			float distance = 200000; //Just a large number 
			int32 biotope = -1;

			for (auto& it : markedTilesVoronoi)
			{
				float temp = FVector2f::Distance(currTileCoords, FVector2f(it.Key % gridSizeOfProxies, FMath::Floor(it.Key / gridSizeOfProxies)));
				if (distance >= temp )
				{
					//markedTiles.Add(i,it.Value);
					biotope = it.Value;
					//it->biotope = b.biomeType;
					distance = temp;
				}

			}
			markedTiles.Add(i, biotope);
		}
	}
}
void S2DPreviewWindow::AddRoadPoint(FVector2D inCoords)
{

	roadCoords.Add(FVector(inCoords.X, inCoords.Y, 0.0));
}
int32 S2DPreviewWindow::FromCoordToTileIndex(FVector2D inCoords)
{
	int32 x_floor = floor(inCoords.X / TileSize);
	int32 y_floor = floor(inCoords.Y / TileSize);

	return (y_floor * gridSizeOfProxies) + x_floor;
}


S2DPreviewWindow::~S2DPreviewWindow()
{
}
