// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"

UTile::UTile()
{
	
	//adjacentTiles.Init(nullptr, 8);
	/*static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/Test_assets/M_grassMaterial.M_grassMaterial'"));
	if (Material.Object != NULL)
	{
		myMaterial = (UMaterial*)Material.Object;
		UE_LOG(LogTemp, Warning, TEXT("Value tx: %s"), *myMaterial->GetName());
	}*/
	//tileMaterial = LoadObject<UMaterial>(nullptr, TEXT("Material'/Game/Test_assets/M_grassMaterial.M_grassMaterial'"));
	//updateMaterial(temp);
}

UTile::UTile(TObjectPtr<ALandscapeStreamingProxy> inProxy)
{
	streamingProxy = inProxy;
	//streamingProxy->
	//adjacentTiles.Init(nullptr,8);

	//Setting default material
	// Try to find material named M_grassMaterial in the following path:
	UMaterial* temp = LoadObject<UMaterial>(nullptr, TEXT("Material'/Game/Test_assets/M_grassMaterial.M_grassMaterial'"));

	updateMaterial(temp);
	
}

//Tile::~Tile()
//{
//	delete streamingProxy;
//	//delete tileMaterial;
//
//	/*for(int i = 0; i < adjacentTiles.Num(); i++){
//		delete adjacentTiles[i];
//	}*/
//	
//}

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

void UTile::updateAdjacentTiles(TArray<UTile>& inTiles, const uint32 gridSizeProxy)
{
	//Conveion from unsigned to signed, for correct comparisons
	int32 numOfTiles = inTiles.Num();
	int32 gridSize = gridSizeProxy;

	/*UE_LOG(LogTemp, Warning, TEXT("gridSizeProxy : %d"), gridSizeProxy);
	UE_LOG(LogTemp, Warning, TEXT("Nummer of tiles : %d"), numOfTiles);*/
	//int temp;
	

	//UE_LOG(LogTemp, Warning, TEXT("Case 1 : %d"),(index - gridSize - 1) );
	if ((index - gridSize - 1) >= 0) //Top left
	{
		//UE_LOG(LogTemp, Warning, TEXT("Came in"));
		adjacentTiles[0] = &inTiles[(index - gridSize - 1)];
	}
	//UE_LOG(LogTemp, Warning, TEXT("Case 2 : %d"),(index - gridSize) );
	if ((index - gridSize) >= 0) // Top straight
	{
		//UE_LOG(LogTemp, Warning, TEXT("Came in"));
		adjacentTiles[1] = &inTiles[index - gridSize];
	}
	//UE_LOG(LogTemp, Warning, TEXT("Case 3 : %d"), (index - gridSize + 1));
	if ((index - gridSize + 1) >= 0) // Top right
	{
		adjacentTiles[2] = &inTiles[index - gridSize + 1];
	}
	//UE_LOG(LogTemp, Warning, TEXT("Case 4 : %d"), (index - 1));
	//if ((index - 1) >= 0) // left
	//{
	//	adjacentTiles[3] = &inTiles[index - 1];
	//}
	//if ((index + 1) < numOfTiles) // right
	//{
	//	adjacentTiles[4] = &inTiles[index + 1];
	//}
	//if ((index + gridSize - 1) < numOfTiles) // bottom left
	//{
	//	adjacentTiles[5] = &inTiles[index + gridSize - 1];
	//}
	//if ((index + gridSize) < numOfTiles) // bottom straight
	//{
	//	adjacentTiles[6] = &inTiles[index + gridSize];
	//}
	//if ((index + gridSize + 1) < numOfTiles) // bottom right
	//{
	//	adjacentTiles[7] = &inTiles[index + gridSize + 1];
	//	UE_LOG(LogTemp, Warning, TEXT("Case 8 : %d"), (index + gridSize + 1));
	//}


}
