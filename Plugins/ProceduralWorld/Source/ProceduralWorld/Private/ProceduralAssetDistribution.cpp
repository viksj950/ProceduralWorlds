// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralAssetDistribution.h"

ProceduralAssetDistribution::ProceduralAssetDistribution()
{
}

ProceduralAssetDistribution::~ProceduralAssetDistribution()
{
}


void ProceduralAssetDistribution::spawnActorObject(const UTile* t) {

	UWorld* World = nullptr;

	FWorldContext& EditorWorldContext = GEditor->GetEditorWorldContext();
	World = EditorWorldContext.World();

	FVector proxyLocation = t->streamingProxy->GetActorLocation();

	FVector Location(proxyLocation); 
	//FRotator Rotation(0.0f, 0.0f, 0.0f);

	FMath mathInstance;
	float min = 0.0f;
	float max = 90.0f;
	float maxalt = 65000;

	float randomVal = mathInstance.FRandRange(min, max);
	//float randomVal2 = mathInstance.FRandRange(min, maxalt);

	FRotator Rotation(randomVal, randomVal, randomVal);
	//FVector Location(randomVal2, randomVal2, randomVal2);
	
	FActorSpawnParameters SpawnInfo;
	//SpawnInfo.Name = "Tile_asset";

	//Places at 0,0 world coords
	//AStaticMeshActor* MyNewActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass());

	//Specify where in the world it will spawn
	AStaticMeshActor* MyNewActor = World->SpawnActor<AStaticMeshActor>(Location, Rotation, SpawnInfo);
	
	UStaticMesh* Mesh;
	if (t->isCity) {
		Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/Test_assets/Cube_City.Cube_City'"));
	}
	else { //tree
		Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/Test_assets/Cylinder_Tree.Cylinder_Tree'"));
	}

	UStaticMeshComponent* MeshComponent = MyNewActor->GetStaticMeshComponent();
	if (MeshComponent)
	{
		MeshComponent->SetStaticMesh(Mesh);
	}

}
