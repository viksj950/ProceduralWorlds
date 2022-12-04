// Fill out your copyright notice in the Description page of Project Settings.


#include "S2DPreviewWindow.h"

S2DPreviewWindow::S2DPreviewWindow(const int32& inSizeX, const int32& inSizeY, const int32& inQuadsPerComponent,
	const int32& inComponentsPerProxy, const int32& inSectionsPerComponent, const int32& inTilesSize): SizeX{inSizeX}, SizeY{inSizeY}, QuadsPerComponent{inQuadsPerComponent},
	ComponentsPerProxy{inComponentsPerProxy}, SectionsPerComponent{inSectionsPerComponent}, TileSize{inTilesSize}
{
	//First two slots are always populated by default? heightmap and grid:
	textures.Add(nullptr);
	textures.Add(nullptr);
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
		textures[1] = tempTexturePtr;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Default pointer for gridTexture in texture array is missing"));
	}

}

void S2DPreviewWindow::AssembleWidget()
{
	//TSharedPtr<FSlateImageBrush> tempImageBrush = MakeShared<FSlateImageBrush>(textures[0], FVector2D(textures[0]->GetSizeX(), textures[0]->GetSizeY()), FLinearColor(1.0f, 1.0f, 1.0f, 1.0f), ESlateBrushTileType::NoTile);
	brushes[0] = MakeShared<FSlateImageBrush>(textures[0], FVector2D(textures[0]->GetSizeX(), textures[0]->GetSizeY()), FLinearColor(1.0f, 1.0f, 1.0f, 1.0f), ESlateBrushTileType::NoTile);
	brushes[1] = MakeShared<FSlateImageBrush>(textures[1], FVector2D(textures[1]->GetSizeX(), textures[1]->GetSizeY()), FLinearColor(1.0f, 1.0f, 1.0f, 1.0f), ESlateBrushTileType::NoTile);
	
	//brushes.Add(MakeShared<FSlateImageBrush>(textures[1], FVector2D(textures[1]->GetSizeX(), textures[1]->GetSizeY()), FLinearColor(1.0f, 1.0f, 1.0f, 1.0f), ESlateBrushTileType::NoTile));

	previewContext->SetContent(
		
		
		SNew(SOverlay)
		.Cursor(EMouseCursor::Crosshairs)
		
		+SOverlay::Slot()
		[
			SNew(SBox)
			
			.MinAspectRatio(1)
			[
			SNew(SImage)

			.Image(brushes[0].Get())
			]
		]
		+SOverlay::Slot()
		[
			SNew(SBox)
			.MinAspectRatio(1)
			[
				SNew(SImage)

				.Image(brushes[1].Get())
			]

		]
		

		);

}


S2DPreviewWindow::~S2DPreviewWindow()
{
}
