// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralAssetDistribution.h"

ProceduralAssetDistribution::ProceduralAssetDistribution()
{
}

ProceduralAssetDistribution::~ProceduralAssetDistribution()
{
}

void ProceduralAssetDistribution::spawnActorObjectsCity(UTile* t, const int32 ComponentSizeQuads, const int32 ComponentsPerProxy, const int32 GridSizeOfProxies, int32 assetCount, float scaleVar) {
	UWorld* World = nullptr;

	FWorldContext& EditorWorldContext = GEditor->GetEditorWorldContext();
	World = EditorWorldContext.World();
	FVector proxyLocation;
	FVector proxyScale;
	FVector Location;

	//Asset specific
	FVector assetScale;
	FMath mathInstance;
	float minPos = 0.1f; //not zero or 1 to reduce chance for houses to spawn on cliffsides/inside mountain
	float maxPos = 0.9f;
	float minScale;
	float maxScale;
	float RotationAngle;

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

		Location.Z = t->streamingProxy->GetHeightAtLocation(Location).GetValue();

		FVector UpVector = FVector(0, 0, 1);

		RotationAngle = acosf(FVector::DotProduct(UpVector, tri.normal));

		//Check if angle is acceptable to spawn the object within
		if (RotationAngle > 0.3) {
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

		//For scale variance (ugly implementation, assumes uniform scale on all axises which "should" be true)
		FVector defaultScale = MyNewActor->GetActorScale3D();
		minScale = defaultScale.X - scaleVar;
		maxScale = defaultScale.X + scaleVar;

		float scaleValue = mathInstance.FRandRange(minScale, maxScale);

		assetScale = { scaleValue ,scaleValue ,scaleValue };

		MyNewActor->SetActorScale3D(assetScale);
		
		UStaticMesh* Mesh1 = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/Test_assets/Cube_City.Cube_City'"));

		//Check if location is suitable and not inside existing house
		FVector newSize = scaleValue * Mesh1->GetBounds().GetBox().GetSize();
		/*UE_LOG(LogTemp, Warning, TEXT("Size of house : %s"), *newSize.ToString());*/
	
		Point2D currentHouseTL = { Location.X - newSize.X, Location.Y + newSize.Y };
		//UE_LOG(LogTemp, Warning, TEXT("TL.x : %f"), currentHouseTL.x);
		//UE_LOG(LogTemp, Warning, TEXT("TL.y : %f"), currentHouseTL.y);
		Point2D currentHouseBR = { Location.X + newSize.X, Location.Y - newSize.Y };
	/*	UE_LOG(LogTemp, Warning, TEXT("BR.x : %f"), currentHouseBR.x);
		UE_LOG(LogTemp, Warning, TEXT("BR.y : %f"), currentHouseBR.y);*/
		Point2D prevHouseTL;
		Point2D prevHouseBL; 

		for (int k = 0; k < t->tileAssets.Num(); k++) {
			prevHouseTL = { t->tileAssets[k]->GetActorLocation().X , t->tileAssets[k]->GetActorLocation().Y};
		/*	if (Intersecting()) {

			}*/
			//Use this
			//+t->tileAssets[k]->GetStaticMeshComponent()->GetStaticMesh()->GetBounds()->GetSize().X

		}

		UStaticMeshComponent* MeshComponent = MyNewActor->GetStaticMeshComponent();
		if (MeshComponent)
		{
			MeshComponent->SetStaticMesh(Mesh1);
		}

		t->tileAssets.Add(MyNewActor);

		
			FVector locationHouse = t->tileAssets[t->tileAssets.Num()-1].Get()->GetActorLocation();
			UE_LOG(LogTemp, Warning, TEXT("Position of house : %s"), *locationHouse.ToString());
		
	}
}


void ProceduralAssetDistribution::spawnActorObjectsForest(UTile* t, const int32 ComponentSizeQuads, const int32 ComponentsPerProxy, const int32 GridSizeOfProxies, int32 assetCount, float scaleVar) {

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
	float RotationAngle;

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

			Location.Z = t->streamingProxy->GetHeightAtLocation(Location).GetValue();

			FVector UpVector = FVector(0, 0, 1);

			RotationAngle = acosf(FVector::DotProduct(UpVector, tri.normal));

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

			/*if (t->biotope == 0) {
				Mesh1 = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/Test_assets/Cube_City.Cube_City'"));
			}*/
			 //tree + grass
				Mesh1 = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/Test_assets/Tree.Tree'"));

			

			UStaticMeshComponent* MeshComponent = MyNewActor->GetStaticMeshComponent();
			if (MeshComponent)
			{
				MeshComponent->SetStaticMesh(Mesh1);
			}

			t->tileAssets.Add(MyNewActor);
		}
	

}

bool ProceduralAssetDistribution::Intersecting(Point2D tl1, Point2D br1, Point2D tl2, Point2D br2)
{
	// if rectangle has area 0, no overlap
	if (tl1.x == br1.x || tl1.y == br1.y || br2.x == tl2.x || tl2.y == br2.y)
		return false;

	// If one rectangle is on left side of other
	if (tl1.x > br2.x || tl2.x > br1.x)
		return false;

	// If one rectangle is above other
	if (br1.y > tl2.y || br2.y > tl1.y)
		return false;

	return true;
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
