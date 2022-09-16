// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"

Tile::Tile()
{
	/*static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/Test_assets/M_grassMaterial.M_grassMaterial'"));
	if (Material.Object != NULL)
	{
		myMaterial = (UMaterial*)Material.Object;
		UE_LOG(LogTemp, Warning, TEXT("Value tx: %s"), *myMaterial->GetName());
	}*/
	
}

Tile::Tile(TObjectPtr<ALandscapeStreamingProxy> inProxy)
{
	streamingProxy = inProxy;

	//Setting default material
	// Try to find material named M_grassMaterial in the following path:
	UMaterial* temp = LoadObject<UMaterial>(nullptr, TEXT("Material'/Game/Test_assets/M_grassMaterial.M_grassMaterial'"));

	updateMaterial(temp);


	
	
}

void Tile::updateMaterial(UMaterial* inMaterial)
{
	if (inMaterial)
	{
		tileMaterial = inMaterial;
		
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

