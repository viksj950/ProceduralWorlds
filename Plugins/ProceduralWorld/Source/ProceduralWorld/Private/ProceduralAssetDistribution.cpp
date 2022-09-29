// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralAssetDistribution.h"

ProceduralAssetDistribution::ProceduralAssetDistribution()
{
}

ProceduralAssetDistribution::~ProceduralAssetDistribution()
{
}


void ProceduralAssetDistribution::spawnActorObjects(TArray<UTile*> &inTiles, const int32 ComponentSizeQuads, const int32 ComponentNumSubsections, const int32 GridSizeOfProxies) {

	UWorld* World = nullptr;

	FWorldContext& EditorWorldContext = GEditor->GetEditorWorldContext();
	World = EditorWorldContext.World();
	FVector proxyLocation;
	FVector proxyScale;
	FVector Location;
	/*if (inTiles[0]->streamingProxy != nullptr)
	{
		proxyLocation = inTiles[0]->streamingProxy->GetActorLocation();
		proxyScale = inTiles[0]->streamingProxy->GetActorScale();
		
	}*/
	
	int32 yAxis{ 0 };
	int32 xAxis{ -1 }; //-1 to avoid special case for index = 0 
	for(auto& t: inTiles){

		if (t->streamingProxy != nullptr)
		{
			proxyLocation = t->streamingProxy->GetActorLocation();
			proxyScale = t->streamingProxy->GetActorScale();

		}

		if (t->index % GridSizeOfProxies == 0) { //if we are at new row   
	
			xAxis = 0;
			yAxis++;

		}
		UE_LOG(LogTemp, Warning, TEXT("ComponentNumSubsections %d"), ComponentNumSubsections);
		//find positoin in tile
		Location = proxyLocation + (ComponentNumSubsections * proxyScale);
		Location.Z = 0;
		//spawn actor
		//UE_LOG(LogTemp, Warning, TEXT("Location of proxy's origo ? %s"), *Location.ToString());

		FMath mathInstance;
		float min = 0.0f;
		float max = 90.0f;

		float randomVal = mathInstance.FRandRange(min, max);


		FRotator Rotation(randomVal, randomVal, randomVal);
		//FVector Location(randomVal2, randomVal2, randomVal2);

		FActorSpawnParameters SpawnInfo;
		//SpawnInfo.Name = "Tile_asset";


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

		xAxis++;

	}

	//FVector Location(proxyLocation); 
	//FRotator Rotation(0.0f, 0.0f, 0.0f);

	//FMath mathInstance;
	//float min = 0.0f;
	//float max = 90.0f;

	//float randomVal = mathInstance.FRandRange(min, max);


	//FRotator Rotation(randomVal, randomVal, randomVal);
	////FVector Location(randomVal2, randomVal2, randomVal2);
	//
	//FActorSpawnParameters SpawnInfo;
	////SpawnInfo.Name = "Tile_asset";


	////Specify where in the world it will spawn
	//AStaticMeshActor* MyNewActor = World->SpawnActor<AStaticMeshActor>(Location, Rotation, SpawnInfo);
	//
	//UStaticMesh* Mesh;
	//if (t->isCity) {
	//	Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/Test_assets/Cube_City.Cube_City'"));
	//}
	//else { //tree
	//	Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/Test_assets/Cylinder_Tree.Cylinder_Tree'"));
	//}

	//UStaticMeshComponent* MeshComponent = MyNewActor->GetStaticMeshComponent();
	//if (MeshComponent)
	//{
	//	MeshComponent->SetStaticMesh(Mesh);
	//}
	/*
	FVector scale = t->streamingProxy->GetActorScale();
	UE_LOG(LogTemp, Warning, TEXT("scale of proxy %s"), *scale.ToString());*/

}
