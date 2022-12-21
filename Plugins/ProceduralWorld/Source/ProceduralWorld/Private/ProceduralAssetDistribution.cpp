// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralAssetDistribution.h"

ProceduralAssetDistribution::ProceduralAssetDistribution()
{
}

ProceduralAssetDistribution::~ProceduralAssetDistribution()
{
}

void ProceduralAssetDistribution::spawnAssets(TArray<TSharedPtr<biomeAssets>> biomeSettings, TArray<UTile*> tiles, const int32 ComponentSizeQuads, const int32 ComponentsPerProxy,
	const int32 GridSizeOfProxies, const TArray<ControlPoint>& inRoadCoords, const TArray<Road>& roads, const int32& landscapeScale)
{
	UWorld* World = nullptr;
	FWorldContext& EditorWorldContext = GEditor->GetEditorWorldContext();
	World = EditorWorldContext.World();
	
	FVector proxyLocation;
	FVector proxyScale;
	FVector Location;
	FVector assetScale;
	FMath mathInstance;

	float minPos = 0.05f;
	float maxPos = 0.95f;
	float minRot = 0.0f;
	float maxRot = 2.0f * PI;
	float minScale;
	float maxScale;
	float RotationAngle;
	int AssetCount = 0; //used to count per tile assets

	//Iterate through all tiles
	for (size_t i = 0; i < tiles.Num(); i++)
	{
		//Check which type the tile is in terms of biotope
		for (size_t j = 0; j < biomeSettings.Num(); j++) {
			if (tiles[i]->biotope == biomeSettings[j]->biotopeIndex) {

				//Make sure the tiles has properly setup a streaming proxy for world partitioning
				if (tiles[i]->streamingProxy != nullptr)
				{
					//Fetch a location within the proxy
					proxyLocation = tiles[i]->streamingProxy->GetActorLocation();
					proxyScale = tiles[i]->streamingProxy->GetActorScale();

				}
				else {
					UE_LOG(LogTemp, Warning, TEXT("Something went wrong, tiles are missing streamingproxies"));
					break;
				}

				//Iterate through the number of assets types this tiles should contain
				//UE_LOG(LogTemp, Warning, TEXT("Number of assetstypes : %d"), biomeSettings[j]->AssetSettings.Num());
				for (size_t k = 0; k < biomeSettings[j]->AssetSettings.Num(); k++)
				{
					//Iterate through the number of instances of this specific asset the tile should countain
					AssetCount = 0;
					while (AssetCount < biomeSettings[j]->AssetSettings[k]->assetCount) {

						//Random coordinates for X,Y within the bounds of the tiles
						float randomValX = mathInstance.FRandRange(minPos, maxPos);
						float randomValY = mathInstance.FRandRange(minPos, maxPos);

						//Find position in tile, scales based on perProxy
						Location = proxyLocation + (ComponentSizeQuads * proxyScale) * (ComponentsPerProxy / 2.0);

						//Set position for X,Y
						Location.X = proxyLocation.X + (ComponentSizeQuads * proxyScale.X) * (ComponentsPerProxy * randomValX);
						Location.Y = proxyLocation.Y + (ComponentSizeQuads * proxyScale.Y) * (ComponentsPerProxy * randomValY);

						//Create triangle for normal calculations to match ground tilt
						Triangle tri(tiles[i], Location.X, Location.Y);

						Location.Z = tiles[i]->streamingProxy->GetHeightAtLocation(Location).GetValue();

						FVector UpVector = FVector(0, 0, 1);

						RotationAngle = acosf(FVector::DotProduct(UpVector, tri.normal));

						//Check if angle is acceptable to spawn the object within
						if (RotationAngle > biomeSettings[j]->AssetSettings[k]->angleThreshold) {
							AssetCount++;
							continue;
						}

						FVector RotationAxis = FVector::CrossProduct(UpVector, tri.normal);
						RotationAxis.Normalize();

						float randomZRotation = mathInstance.FRandRange(minRot, maxRot);

						FQuat Quat = FQuat(RotationAxis, RotationAngle);
						FQuat quatRotZ = FQuat(tri.normal, randomZRotation);
						Quat = quatRotZ * Quat;

						FRotator Rotation(tri.normal.Rotation());
						FActorSpawnParameters SpawnInfo;

						//Specify where in the world it will spawn, using ground tilt
						AStaticMeshActor* MyNewActor = World->SpawnActor<AStaticMeshActor>(Location, Quat.Rotator(), SpawnInfo);

						//For scale variance (assumes uniform scale on all axises which "should" be true)
						FVector defaultScale = MyNewActor->GetActorScale3D();
						minScale = defaultScale.X - biomeSettings[j]->AssetSettings[k]->scaleVar;
						maxScale = defaultScale.X + biomeSettings[j]->AssetSettings[k]->scaleVar;

						float scaleValue = mathInstance.FRandRange(minScale, maxScale);

						assetScale = { scaleValue ,scaleValue ,scaleValue };

						MyNewActor->SetActorScale3D(assetScale);

						UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, *biomeSettings[j]->AssetSettings[k]->ObjectPath);

						//If noCollide is true, we have to check bounding box to all previous spawned objects
						if (biomeSettings[j]->AssetSettings[k]->noCollide && !biomeSettings[j]->AssetSettings[k]->considerRoad) {
							spawnWithNoCollide(tiles[i], Location, scaleValue, biomeSettings[j]->AssetSettings[k]->density, MyNewActor, Mesh, AssetCount);
						}
						//If considerRoad is true, we have to check range to road spline points and see if it is above threshold
						else if (biomeSettings[j]->AssetSettings[k]->considerRoad && !biomeSettings[j]->AssetSettings[k]->noCollide) {
							if (roadConsiderCheck(inRoadCoords, roads, landscapeScale, Location)) {
								AssetCount++;
								culledAssets.Add(MyNewActor);
							}
							else {
								UStaticMeshComponent* MeshComponent = MyNewActor->GetStaticMeshComponent();
								if (MeshComponent)
								{
									MeshComponent->SetStaticMesh(Mesh);
								}
								tiles[i]->tileAssets.Add(MyNewActor);
								AssetCount++;
							}
						}
						//If both true, check first road dist, then if check if it collides with other object
						else if (biomeSettings[j]->AssetSettings[k]->considerRoad && biomeSettings[j]->AssetSettings[k]->noCollide) {
							if (roadConsiderCheck(inRoadCoords, roads, landscapeScale, Location)) {
								AssetCount++;
								culledAssets.Add(MyNewActor);
							}
							else {
								spawnWithNoCollide(tiles[i], Location, scaleValue, biomeSettings[j]->AssetSettings[k]->density, MyNewActor, Mesh, AssetCount);
							}
						}
						//Both false, spawn without any criterion
						else if (!biomeSettings[j]->AssetSettings[k]->considerRoad && !biomeSettings[j]->AssetSettings[k]->noCollide) {
							UStaticMeshComponent* MeshComponent = MyNewActor->GetStaticMeshComponent();
							if (MeshComponent)
							{
								MeshComponent->SetStaticMesh(Mesh);
							}
							tiles[i]->tileAssets.Add(MyNewActor);
							AssetCount++;
						}
						else {
							UE_LOG(LogTemp, Warning, TEXT("This should never happen PA.cpp 163"));
						}
					}
					for (auto& t : culledAssets)
					{
						if (t.IsValid())
						{
							t->Destroy();
						}
					}
					culledAssets.Empty();
				}
				
			}
			else {
				/*UE_LOG(LogTemp, Warning, TEXT("Something went wrong, no biome type of this tile exists (!)"));*/
			}
		}

		
	}
	
}

void ProceduralAssetDistribution::spawnWithNoCollide(UTile* tile, const FVector& Location, const float& scaleValue, const float& density, AStaticMeshActor* MyNewActor , UStaticMesh* Mesh, int &AssetCount)
{
	//Check if location is suitable and not inside existing house
	FVector newSize = density * scaleValue * Mesh->GetBounds().GetBox().GetSize();

	Point2D currentObjectTL = { Location.X - (newSize.X / 2), Location.Y + (newSize.Y / 2) };
	Point2D currentObjectBR = { Location.X + (newSize.X / 2), Location.Y - (newSize.Y / 2) };

	Point2D prevObjectTL;
	Point2D prevObjectBL;
	bool shouldSpawn = true;

	if (tile->tileAssets.IsEmpty()) { //first house in tile, no need to check intersect
		UStaticMeshComponent* MeshComponent = MyNewActor->GetStaticMeshComponent();
		if (MeshComponent)
		{
			MeshComponent->SetStaticMesh(Mesh);
		}

		tile->tileAssets.Add(MyNewActor);
		AssetCount++;
	}
	else { //need to check intersect between other houses, as there is houses already in the tile
		for (int l = 0; l < tile->tileAssets.Num(); l++) {
			FVector scale = tile->tileAssets[l].Get()->GetStaticMeshComponent()->GetRelativeScale3D();

			FVector prevSize = density * scale * tile->tileAssets[l].Get()->GetStaticMeshComponent()->GetStaticMesh()->GetBounds().GetBox().GetSize();
			prevObjectTL = { tile->tileAssets[l]->GetActorLocation().X - (prevSize.X / 2) , tile->tileAssets[l]->GetActorLocation().Y + (prevSize.Y / 2) };
			prevObjectBL = { tile->tileAssets[l]->GetActorLocation().X + (prevSize.X / 2) , tile->tileAssets[l]->GetActorLocation().Y - (prevSize.Y / 2) };

			if (Intersecting(currentObjectTL, currentObjectBR, prevObjectTL, prevObjectBL)) {
				shouldSpawn = false;
				AssetCount++;
				culledAssets.Add(MyNewActor);
				break;
			}
		}
		if (shouldSpawn) {
			UStaticMeshComponent* MeshComponent = MyNewActor->GetStaticMeshComponent();
			if (MeshComponent)
			{
				MeshComponent->SetStaticMesh(Mesh);
			}
			tile->tileAssets.Add(MyNewActor); 
			AssetCount++;
		}

	}
}

bool ProceduralAssetDistribution::roadConsiderCheck(const TArray<ControlPoint>& inRoadCoords, const TArray<Road>& roads, const int32& landscapeScale, const FVector& Location)
{
	bool toCloseToRoad{ false };
	//UE_LOG(LogTemp, Warning, TEXT("Number of roads: %d"), roads.Num());
	for (size_t i = 0; i < roads.Num(); i++)
	{
		
		uint32 maxDistToRoad{ roads[i].Width * landscapeScale };
		float Xdiff, Ydiff;
	/*	UE_LOG(LogTemp, Warning, TEXT("maxDistToRoad: %d"), maxDistToRoad);*/
		
		for (size_t j = 0; j < inRoadCoords.Num(); j++)
		{
			//Inverted beacause inverted devolopers (world map coords and height map coords are "mirrored")
			Xdiff = abs(inRoadCoords[j].pos.Y - Location.X);
			Ydiff = abs(inRoadCoords[j].pos.X - Location.Y);
			if (Xdiff <= maxDistToRoad && Ydiff <= maxDistToRoad) {
				toCloseToRoad = true;
				break;
			}
		}

		if (toCloseToRoad) {
		/*	UE_LOG(LogTemp, Warning, TEXT("Asset to close to span to road, aborting!"));*/
			continue;
		}
	}
	return toCloseToRoad;
	
}

void ProceduralAssetDistribution::spawnActorObjectsCity(UTile* t, const int32 ComponentSizeQuads, const int32 ComponentsPerProxy, const int32 GridSizeOfProxies, int32 assetCount, float spread, float scaleVar, const TArray<ControlPoint>& inRoadCoords, const int &roadWidth) {
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
	float minRot = 0.0f;
	float maxRot = 2.0f * PI;
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
		//UE_LOG(LogTemp, Warning, TEXT("Asset location.X %f"), Location.X);
		//UE_LOG(LogTemp, Warning, TEXT("Asset location.Y %f"), Location.Y);

		//Position check to avoid collison with road
		if (t->biotope != 2) {
			int maxDistToRoad{ roadWidth };
			float Xdiff, Ydiff;
			bool toCloseToRoad{ false };
			for (size_t j = 0; j < inRoadCoords.Num(); j++)
			{
				Xdiff = abs(inRoadCoords[j].pos.Y -Location.X);
				Ydiff = abs(inRoadCoords[j].pos.X - Location.Y);
				//UE_LOG(LogTemp, Warning, TEXT("Diff x %f"), Xdiff);
				//UE_LOG(LogTemp, Warning, TEXT("Diff y %f"), Ydiff);
				if (Xdiff <= maxDistToRoad && Ydiff <= maxDistToRoad) {
					toCloseToRoad = true;
					break;
				}
			}

			if (toCloseToRoad) {
				UE_LOG(LogTemp, Warning, TEXT("Asset to close to span to road, aborting!"));
				continue;
			}
		
		}

		//Create triangle for normal calculations
		Triangle tri(t, Location.X, Location.Y);

		Location.Z = t->streamingProxy->GetHeightAtLocation(Location).GetValue();

		FVector UpVector = FVector(0, 0, 1);

		RotationAngle = acosf(FVector::DotProduct(UpVector, tri.normal));

		//Check if angle is acceptable to spawn the object within
		if (RotationAngle > 0.1) {
			//UE_LOG(LogTemp, Warning, TEXT("Rotation Angle for asset was to steep with id: %d"), i);
			continue;
		}

		FVector RotationAxis = FVector::CrossProduct(UpVector, tri.normal);
		RotationAxis.Normalize();

		float randomZRotation = mathInstance.FRandRange(minRot, maxRot);

		FQuat Quat = FQuat(RotationAxis, RotationAngle);
		FQuat quatRotZ = FQuat(tri.normal, randomZRotation);

		Quat = quatRotZ * Quat;

		FRotator Rotation(tri.normal.Rotation());

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
		
		UStaticMesh* Mesh1 = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/Test_assets/house.house'"));
		//temp mesh to identify culled houses
	/*	UStaticMesh* Mesh2 = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/Test_assets/Cube_Ghost.Cube_Ghost'"));*/

		//Check if location is suitable and not inside existing house
		FVector newSize =  spread*scaleValue * Mesh1->GetBounds().GetBox().GetSize();
		//UE_LOG(LogTemp, Warning, TEXT("Size of house : %s"), *newSize.ToString());
	
		Point2D currentHouseTL = { Location.X - (newSize.X/2), Location.Y + (newSize.Y/2) };
		Point2D currentHouseBR = { Location.X + (newSize.X/2), Location.Y - (newSize.Y/2) };

		Point2D prevHouseTL;
		Point2D prevHouseBL; 
		bool shouldSpawn = true;

		if(t->tileAssets.IsEmpty()){ //first house in tile, no need to check intersect
			//UE_LOG(LogTemp, Warning, TEXT("House should spawn, no other house spawned in yet in this tile"));
			UStaticMeshComponent* MeshComponent = MyNewActor->GetStaticMeshComponent();
			if (MeshComponent)
			{
				MeshComponent->SetStaticMesh(Mesh1);
			}

			t->tileAssets.Add(MyNewActor);
		}
		else { //need to check intersect between other houses, as there is houses already in the tile
			for (int k = 0; k < t->tileAssets.Num(); k++) {
				FVector scale = t->tileAssets[k].Get()->GetStaticMeshComponent()->GetRelativeScale3D();
				
				FVector prevSize = spread * scale * t->tileAssets[k].Get()->GetStaticMeshComponent()->GetStaticMesh()->GetBounds().GetBox().GetSize();
				prevHouseTL = { t->tileAssets[k]->GetActorLocation().X - (prevSize.X/2) , t->tileAssets[k]->GetActorLocation().Y + (prevSize.Y/2) };
				prevHouseBL = { t->tileAssets[k]->GetActorLocation().X + (prevSize.X/2) , t->tileAssets[k]->GetActorLocation().Y - (prevSize.Y/2) };

				if (Intersecting(currentHouseTL, currentHouseBR, prevHouseTL, prevHouseBL)) {
					//UE_LOG(LogTemp, Warning, TEXT("WILL NOT SPAWN HOUSE, IT IS WITHIN BOUNDS OF OTHER"));
					shouldSpawn = false;
					culledAssets.Add(MyNewActor);
					break;
				}
				//else {
				//	UE_LOG(LogTemp, Warning, TEXT("House didnt spawn, other house in bounds [SHOWING GHOST HOUSE]"));
				//	UStaticMeshComponent* MeshComponent = MyNewActor->GetStaticMeshComponent();
				//	if (MeshComponent)
				//	{
				//		MeshComponent->SetStaticMesh(Mesh2);
				//	}

				//	//t->tileAssets.Add(MyNewActor);
				//}
				

			}
			if (shouldSpawn) {
				//UE_LOG(LogTemp, Warning, TEXT("House should spawn, no other house in bounds"));
				UStaticMeshComponent* MeshComponent = MyNewActor->GetStaticMeshComponent();
				if (MeshComponent)
				{
					MeshComponent->SetStaticMesh(Mesh1);
				}

				t->tileAssets.Add(MyNewActor);
			}
		
		}
		
	}
	//removal of all the culled houses
	for (auto& i : culledAssets)
	{
		if (i.IsValid())
		{
			i->Destroy();
		}
	}
	culledAssets.Empty();
}


void ProceduralAssetDistribution::spawnActorObjectsPlains(UTile* t, const int32 ComponentSizeQuads, const int32 ComponentsPerProxy, const int32 GridSizeOfProxies, int32 assetCount, float scaleVar, const TArray<ControlPoint>& inRoadCoords, const int& roadWidth, bool withGrass) {

	UWorld* World = nullptr;

	FWorldContext& EditorWorldContext = GEditor->GetEditorWorldContext();
	World = EditorWorldContext.World();
	FVector proxyLocation;
	FVector proxyScale;
	FVector Location, Locationl;

	//Asset specific
	FVector assetScale;
	FMath mathInstance;
	float minPos = 0.0f;
	float maxPos = 1.0f;
	float minRot = 0.0f;
	float maxRot = 360.0f;
	float minScale;
	float maxScale;
	float RotationAngle, RotationAnglel;

	//temp counter for grass
	int grassDensity = 2; //scales with assetCount * grassDensity

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

			//Position check to avoid collison with road
			if (t->biotope != 2) {

				int maxDistToRoad{ roadWidth};
				float Xdiff, Ydiff;
				bool toCloseToRoad{ false };
				for (size_t j = 0; j < inRoadCoords.Num(); j++)
				{
					Xdiff = abs(inRoadCoords[j].pos.Y - Location.X);
					Ydiff = abs(inRoadCoords[j].pos.X - Location.Y);
	/*				UE_LOG(LogTemp, Warning, TEXT("Diff x %f"), Xdiff);
					UE_LOG(LogTemp, Warning, TEXT("Diff y %f"), Ydiff);*/
					if (Xdiff <= maxDistToRoad && Ydiff <= maxDistToRoad) {
						toCloseToRoad = true;
						break;
					}
				}

				if (toCloseToRoad) {
					UE_LOG(LogTemp, Warning, TEXT("Asset to close to road to spawn, aborting!"));
					continue;
				}
				 
			}

			//Create triangle for normal calculations
			Triangle tri(t, Location.X, Location.Y);

			Location.Z = t->streamingProxy->GetHeightAtLocation(Location).GetValue();

			FVector UpVector = FVector(0, 0, 1);

			RotationAngle = acosf(FVector::DotProduct(UpVector, tri.normal));


			if (RotationAngle > 0.5) {
				//UE_LOG(LogTemp, Warning, TEXT("Rotation Angle for asset was to steep with id: %d"), i);
				continue;
			}

			FVector RotationAxis = FVector::CrossProduct(UpVector, tri.normal);
			RotationAxis.Normalize();

			FQuat Quat = FQuat(RotationAxis, RotationAngle);

			//UE_LOG(LogTemp, Warning, TEXT("Rotation of normal : %s"), *tri.normal.Rotation().ToString());
			//UE_LOG(LogTemp, Warning, TEXT("Rotation Angle is fine, spawns actor in tile: %d"), i);

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

			//For rotation
			float randomZRotation = mathInstance.FRandRange(minRot, maxRot);
			//UE_LOG(LogTemp, Warning, TEXT("Rotation of Z is : %f"), randomZRotation);
			FRotator rotation{ Quat.Rotator().Pitch,randomZRotation,Quat.Rotator().Roll };
			/*UE_LOG(LogTemp, Warning, TEXT("Rotation of plain trees after update : %s"), *rotation.ToString());*/

			MyNewActor->SetActorRotation(rotation);

			//Mesh binding
			UStaticMesh* Mesh1;
			
			//srand(time(NULL)); // Seed the time
			int randNum = rand() % (5 - 1 + 1) + 1; //random number between 1-5 (20 % chance for rocks)

			if (randNum == 5) {
				Mesh1 = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/Test_assets/Rocks/TinyRock/TinyRockLowPoly01.TinyRockLowPoly01'"));
				
			}
			else if (randNum == 4) {
				Mesh1 = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/Test_assets/Tree.Tree'"));
			}
			else {
				Mesh1 = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/_GENERATED/viksj950/temp_tree02.temp_tree02'"));
			}

			UStaticMeshComponent* MeshComponent = MyNewActor->GetStaticMeshComponent();

			if (MeshComponent)
			{
				MeshComponent->SetStaticMesh(Mesh1);
			}
			t->tileAssets.Add(MyNewActor);

			//Leaf/grass spawning
			if (randNum >= 2) {
				Mesh1 = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/Test_assets/Quixel/Var9/Var9_LOD3.Var9_LOD3'"));
			}
			else {
				Mesh1 = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/Test_assets/Quixel/Var15/Var15_LOD0.Var15_LOD0'"));
			}

			if (withGrass) {
				for (size_t k = 0; k < grassDensity; k++)
				{
					//Temp for leaf
					 randomValX = mathInstance.FRandRange(minPos, maxPos);
					 randomValY = mathInstance.FRandRange(minPos, maxPos);

					Location = proxyLocation + (ComponentSizeQuads * proxyScale) * (ComponentsPerProxy / 2.0); //scales based on perProxy

					Location.X = proxyLocation.X + (ComponentSizeQuads * proxyScale.X) * (ComponentsPerProxy * randomValX);
					Location.Y = proxyLocation.Y + (ComponentSizeQuads * proxyScale.Y) * (ComponentsPerProxy * randomValY);

					Triangle tril(t, Location.X, Location.Y);
					Location.Z = t->streamingProxy->GetHeightAtLocation(Location).GetValue();


					RotationAxis = FVector::CrossProduct(UpVector, tril.normal);
					RotationAxis.Normalize();
					Quat = FQuat(RotationAxis, RotationAnglel);

					MyNewActor = World->SpawnActor<AStaticMeshActor>(Location, Quat.Rotator(), SpawnInfo);

					RotationAngle = acosf(FVector::DotProduct(UpVector, tril.normal));
					MyNewActor->SetActorRotation(rotation);

					UStaticMeshComponent* MeshComponent1 = MyNewActor->GetStaticMeshComponent();
					if (MeshComponent1)
					{
						MeshComponent1->SetStaticMesh(Mesh1);
					}
					//Add to tileassets for deletion if needed
					t->tileAssets.Add(MyNewActor);

				}
			}
		
		}
	

}

void ProceduralAssetDistribution::spawnActorObjectsMountains(UTile* t, const int32 ComponentSizeQuads, const int32 ComponentsPerProxy, const int32 GridSizeOfProxies, int32 assetCount, float scaleVar)
{
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
	float minRot = 0.0f;
	float maxRot = 2.0f*PI;
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

		if (RotationAngle > 0.8) {
		/*	UE_LOG(LogTemp, Warning, TEXT("Rotation Angle for rock was to steep with id: %d"), i);*/
			continue;
		}

		FVector RotationAxis = FVector::CrossProduct(UpVector, tri.normal);
		RotationAxis.Normalize();

		float randomZRotation = mathInstance.FRandRange(minRot, maxRot);
		

		FQuat Quat = FQuat(RotationAxis, RotationAngle);
		FQuat quatRotZ = FQuat(tri.normal, randomZRotation);

		Quat = quatRotZ * Quat;

		FActorSpawnParameters SpawnInfo;

		//Specify where in the world it will spawn, using ground tilt
		AStaticMeshActor* MyNewActor = World->SpawnActor<AStaticMeshActor>(Location, /*Quat.Rotator()*/ Quat.Rotator(), SpawnInfo);

		//For scale variance (Super ugly implementation, assumes uniform scale on all axises)
		FVector defaultScale = MyNewActor->GetActorScale3D();
		minScale = defaultScale.X - scaleVar;
		maxScale = defaultScale.X + scaleVar;

		float scaleValue = mathInstance.FRandRange(minScale, maxScale);

		assetScale = { scaleValue ,scaleValue ,scaleValue };

		MyNewActor->SetActorScale3D(assetScale);
		
		UStaticMesh* Mesh1;

		if (Location.Z > 2000) {
			Mesh1 = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/Test_assets/Rocks/LargeRock/LargerockLowpoly01.LargerockLowpoly01'"));
		}
		else if (Location.Z <= 2000 && Location.Z >= 500) {
			Mesh1 = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/Test_assets/Tree/TreeTrunk01.TreeTrunk01'"));
		}
		else {
			Mesh1 = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/_GENERATED/viksj950/temp_tree02.temp_tree02'"));
		}
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

	if (!heightOptP0.IsSet()) {
		heightOptP0 = tile->streamingProxy->GetHeightAtLocation(FVector(x, y, 0.0));
	}
	if (!heightOptP1.IsSet()) {
		heightOptP1 = tile->streamingProxy->GetHeightAtLocation(FVector(x, y , 0.0));
	}
	if (!heightOptP2.IsSet()) {
		heightOptP2 = tile->streamingProxy->GetHeightAtLocation(FVector(x, y , 0.0));
	}
	
	//Assigning triangle corners
	p0 = FVector(x, y + (i / 2), heightOptP0.GetValue());
	p1 = FVector(x + (edgeLength / 2), y - (i / 2), heightOptP1.GetValue());
	p2 = FVector(x - (edgeLength / 2), y - (i / 2), heightOptP2.GetValue());

	//Calc normal
	normal =  FVector::CrossProduct((p2 - p0), (p1 - p2));
	normal.Normalize();

	centroid.X = x;
	centroid.Y = y;
	centroid.Z = (p0.Z + p1.Z + p2.Z) / 3;

}
