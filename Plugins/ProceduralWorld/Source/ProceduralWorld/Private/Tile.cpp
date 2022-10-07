// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"

UTile::UTile(int32 NumOfQuads, int32 ComponentsPerProxy)
{
	tileSize = ((NumOfQuads +1) * (NumOfQuads+1) * ComponentsPerProxy);
	tileHeightData.Init(0,tileSize);
	adjacentTiles.Init(nullptr, 8);

}

//old
UTile::UTile(TObjectPtr<ALandscapeStreamingProxy> inProxy, int32 NumOfQuads, int32 ComponentsPerProxy)
{
	tileSize = ((NumOfQuads +1) * (NumOfQuads+1) * ComponentsPerProxy);
	tileHeightData.Init(32768,tileSize);

	streamingProxy = inProxy;
	adjacentTiles.Init(nullptr,8);

	//Setting default material
	// Try to find material named M_grassMaterial in the following path:
	UMaterial* temp = LoadObject<UMaterial>(nullptr, TEXT("Material'/Game/Test_assets/M_grassMaterial.M_grassMaterial'"));

	updateMaterial(temp);
	
}


void UTile::updateMaterial(UMaterial* inMaterial)
{
	if (inMaterial)
	{
		tileMaterial = inMaterial;

		//streamingProxy.Get()->LandscapeMaterial = tileMaterial;

		streamingProxy->LandscapeMaterial = tileMaterial;

		FPropertyChangedEvent MaterialPropertyChangedEvent(FindFieldChecked< FProperty >(streamingProxy->GetClass(), FName("LandscapeMaterial")));
		streamingProxy->PostEditChangeProperty(MaterialPropertyChangedEvent);
		streamingProxy->PostEditChange();
	}
	else
	{
		tileMaterial = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("DID NOT FIND MATERIAL "));
	}
}

void UTile::updateAdjacentTiles(TArray<UTile*>& inTiles, const uint32 gridSizeProxy)
{
	//Conveion from unsigned to signed, for correct comparisons
	int32 numOfTiles = inTiles.Num();
	int32 gridSize = gridSizeProxy;

	//UE_LOG(LogTemp, Warning, TEXT("gridSizeProxy : %d"), gridSizeProxy);
	//UE_LOG(LogTemp, Warning, TEXT("Nummer of tiles : %d"), numOfTiles);	

	if ((index - gridSize - 1) >= 0 && !(index < gridSize) && index % gridSize != 0) //Top right
	{
		adjacentTiles[0] = inTiles[(index - gridSize - 1)];
	}

	if ((index - gridSize) >= 0) // Top straight
	{
		adjacentTiles[1] = inTiles[index - gridSize];
	}

	if ((index - gridSize + 1) >= 0 && !(index < gridSize) && (index + 1) % gridSize != 0) // Top left
	{
		adjacentTiles[2] = inTiles[index - gridSize + 1];
	}
	if ((index - 1) >= 0 && !(index % gridSize == 0)) // right
	{
		adjacentTiles[3] = inTiles[index - 1];
	}
	if ((index + 1) < numOfTiles && (index + 1) % gridSize != 0) // left
	{
		adjacentTiles[4] = inTiles[index + 1];
	}
	if ((index + gridSize - 1) < numOfTiles && index < numOfTiles - gridSize && index % gridSize != 0) // bottom right
	{
		adjacentTiles[5] = inTiles[index + gridSize - 1];
	}
	if ((index + gridSize) < numOfTiles) // bottom straight
	{
		adjacentTiles[6] = inTiles[index + gridSize];
	}
	if ((index + gridSize + 1) < numOfTiles && !(index >= (numOfTiles - gridSize)) && (index + 1) % gridSize != 0) // bottom left
	{
		adjacentTiles[7] = inTiles[index + gridSize + 1];
	}

}
