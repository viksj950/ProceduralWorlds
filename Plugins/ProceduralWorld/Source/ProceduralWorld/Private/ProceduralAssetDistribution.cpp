// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralAssetDistribution.h"

ProceduralAssetDistribution::ProceduralAssetDistribution()
{
}

ProceduralAssetDistribution::~ProceduralAssetDistribution()
{
}


void ProceduralAssetDistribution::spawnActorObjects(TArray<UTile*> &inTiles, const int32 ComponentSizeQuads, const int32 ComponentsPerProxy, const int32 GridSizeOfProxies, int32 assetCount, float scaleVar) {

	UWorld* World = nullptr;

	FWorldContext& EditorWorldContext = GEditor->GetEditorWorldContext();
	World = EditorWorldContext.World();
	FVector proxyLocation;
	FVector proxyScale;
	FVector Location;

	//Asset specific
	FVector assetScale;
	FMath mathInstance;
	float minPos = 0.0f;
	float maxPos = 1.0f;
	float minScale;
	float maxScale;
	
	for(auto& t: inTiles){

		if (t->streamingProxy != nullptr)
		{
			proxyLocation = t->streamingProxy->GetActorLocation();
			proxyScale = t->streamingProxy->GetActorScale();

		}

		for (int i = 0; i < assetCount; i++) {

			//For position
			float randomValX = mathInstance.FRandRange(minPos, maxPos);
			float randomValY = mathInstance.FRandRange(minPos, maxPos);

			//Find position in tile
			Location = proxyLocation + (ComponentSizeQuads * proxyScale) * (ComponentsPerProxy / 2.0); //scales based on perProxy

			Location.X = proxyLocation.X + (ComponentSizeQuads * proxyScale.X) * (ComponentsPerProxy * randomValX);
			Location.Y = proxyLocation.Y + (ComponentSizeQuads * proxyScale.Y) * (ComponentsPerProxy * randomValY);

			//Create triangle for normal calculations
			Triangle tri(t, Location.X, Location.Y);

			TOptional<float> height = t->streamingProxy->GetHeightAtLocation(Location);
			//UE_LOG(LogTemp, Warning, TEXT("height at location : %f"), height.GetValue());

			Location.Z = height.GetValue();

			FVector UpVector = FVector(0, 0, 1);

			float dotProd = FVector::DotProduct(UpVector, tri.normal);
			float RotationAngle = acosf(dotProd);

			if (RotationAngle > 0.5) {
				UE_LOG(LogTemp, Warning, TEXT("Rotation Angle for asset was to steep with id: %d"), i);
				continue;
			}

			FVector RotationAxis = FVector::CrossProduct(UpVector, tri.normal);
			RotationAxis.Normalize();

			FQuat Quat = FQuat(RotationAxis, RotationAngle);

			FRotator Rotation(tri.normal.Rotation());

			//UE_LOG(LogTemp, Warning, TEXT("Rotation of normal : %s"), *tri.normal.Rotation().ToString());
			UE_LOG(LogTemp, Warning, TEXT("Rotation Angle is fine, spawns actor in tile: %d"), i);

			FActorSpawnParameters SpawnInfo;

			//Specify where in the world it will spawn, using ground tilt
			AStaticMeshActor* MyNewActor = World->SpawnActor<AStaticMeshActor>(Location, Quat.Rotator(), SpawnInfo);

			//For scale variance (Super ugly implementation, assumes uniform scale on all axises)
			FVector defaultScale = MyNewActor->GetActorScale3D();
			minScale = defaultScale.X - scaleVar;
			maxScale = defaultScale.X + scaleVar;
			
			float scaleValue = mathInstance.FRandRange(minScale, maxScale);

			assetScale = { scaleValue ,scaleValue ,scaleValue };

			MyNewActor->SetActorScale3D(assetScale);

			UStaticMesh* Mesh1;

			if (t->biotope == 0) {
				Mesh1 = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/Test_assets/Cube_City.Cube_City'"));
			}
			else { //tree + grass
				Mesh1 = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/Test_assets/Tree.Tree'"));

			}

			UStaticMeshComponent* MeshComponent = MyNewActor->GetStaticMeshComponent();
			if (MeshComponent)
			{
				MeshComponent->SetStaticMesh(Mesh1);
			}

			t->tileAssets.Add(MyNewActor);
		}
	}

}

Triangle::Triangle(UTile* tile, float x, float y, float edgeLength /*= 1 */)
{
	//i is lenght of triangle from top to bottom
	float i = sqrt(pow(edgeLength, 2) - pow(edgeLength / 2, 2));

	//Fetch height (Z) position 
	TOptional<float> heightOptP0 = tile->streamingProxy->GetHeightAtLocation(FVector(x,y + (i / 2),0.0));
	TOptional<float> heightOptP1 = tile->streamingProxy->GetHeightAtLocation(FVector(x+(edgeLength/2),y-(i/2),0.0));
	TOptional<float> heightOptP2 = tile->streamingProxy->GetHeightAtLocation(FVector(x - (edgeLength / 2), y - (i / 2), 0.0));

	//Get actual Z value if fetch was succesfull
	float heightP0 = heightOptP0.GetValue();
	float heightP1 = heightOptP1.GetValue();
	float heightP2 = heightOptP2.GetValue();
	
	//Assigning triangle corners
	p0 = FVector(x, y + (i / 2), heightP0);
	p1 = FVector(x + (edgeLength / 2), y - (i / 2), heightP1);
	p2 = FVector(x - (edgeLength / 2), y - (i / 2), heightP2);

	//Calc normal
	normal =  FVector::CrossProduct((p2 - p0), (p1 - p2));
	normal.Normalize();

	centroid.X = x;
	centroid.Y = y;
	centroid.Z = (p0.Z + p1.Z + p2.Z) / 3;

}
