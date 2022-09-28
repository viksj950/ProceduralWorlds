// Fill out your copyright notice in the Description page of Project Settings.


#include "CreateLandscape.h"

CreateLandscape::CreateLandscape()
{
}

CreateLandscape::~CreateLandscape()
{
}

ALandscape* CreateLandscape::generate()
{
	USelection* SelectedActors = nullptr;
	FText DialogText;

	if (GEditor->GetSelectedActors() == nullptr) {
		SelectedActors = GEditor->GetSelectedActors();

	}
	else {
		DialogText = FText::FromString("Landscape plugin activated, creating landscape...");
	}

	//Add this to have confirmation window
	//FMessageDialog::Open(EAppMsgType::Ok, DialogText);

	FQuat InRotation{ 0,0,0,0 };
	FVector InTranslation{ 0,0,5 };
	FVector InScale{ 100,100,100 };
	FTransform LandscapeTransform{ InRotation, InTranslation, InScale };
	int32 QuadsPerSection{ 63 };
	int32 SectionsPerComponent{ 1 };
	//int32 ComponentCountX{12345};
	//int32 ComponentCountY{21123};
	int32 QuadsPerComponent{ 63 }; //deaultf is 63
	int32 SizeX{ 505 };
	int32 SizeY{ 505 }; //default is 505
	int32 ComponentsPerProxy{ 2 };

	TArray<FLandscapeImportLayerInfo> MaterialImportLayers;
	TMap<FGuid, TArray<uint16>> HeightDataPerLayers;
	TMap<FGuid, TArray<FLandscapeImportLayerInfo>> MaterialLayerDataPerLayers;

	TArray<uint16> HeightData;
	HeightData.SetNum(SizeX * SizeY);

	PerlinNoiseGenerator<uint16, 64> PerlinNoise{};
	PerlinNoise.generateGradients();
	//NoiseGenerator<uint16, 64> noise{}; //N is "cell size", 127 is tiny tiles 1009 is large tiles
	//noise.GenerateNoiseValues(2016);

	int heightScale = 4192;
	for (size_t j = 0; j < SizeY; j++)
	{
		for (size_t i = 0; i < SizeX; i++)
		{

			HeightData[j * SizeX + i] = PerlinNoise.generateNoiseVal(Vec2<float>(i, j) * 0.015625) * heightScale + 32768;
			//HeightData[j * SizeX + i] = noise.processCoord(Vec2<float>(i, j)) * heightScale + 32768;

			//if ((j * SizeX + i) == SizeX * j * 8) {
				//UE_LOG(LogTemp, Warning, TEXT("Value of heightdata: %d"), HeightData[j * SizeX + i]);
			//}

		}
	}

	HeightDataPerLayers.Add(FGuid(), MoveTemp(HeightData));
	MaterialLayerDataPerLayers.Add(FGuid(), MoveTemp(MaterialImportLayers));

	UWorld* World = nullptr;

	// We want to create the landscape in the landscape editor mode's world
	FWorldContext& EditorWorldContext = GEditor->GetEditorWorldContext();
	World = EditorWorldContext.World();

	ALandscape* Landscape = World->SpawnActor<ALandscape>(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f)); //This is working Pog

	Landscape->bCanHaveLayersContent = true;
	Landscape->LandscapeMaterial = nullptr;
	//static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/Materials/YourMaterialName.YourMaterialName'"));
	//static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/Test_assets/M_grassMaterial.M_grassMaterial'"));

	//if (Material.Object != NULL)
	//{
	//	Landscape->LandscapeMaterial = (UMaterial*)Material.Object;
	//	//TheMaterial = (UMaterial*)Material.Object;
	//}else{ 
	//	Landscape->LandscapeMaterial = nullptr; 
	//}

	
	

	Landscape->SetActorTransform(LandscapeTransform);
	Landscape->Import(FGuid::NewGuid(), 0, 0, SizeX - 1, SizeY - 1, SectionsPerComponent, QuadsPerComponent,
		HeightDataPerLayers, nullptr, MaterialLayerDataPerLayers, ELandscapeImportAlphamapType::Additive);

	Landscape->StaticLightingLOD = FMath::DivideAndRoundUp(FMath::CeilLogTwo((SizeX * SizeY) / (2048 * 2048) + 1), (uint32)2);
	// Register all the landscape components
	ULandscapeInfo* LandscapeInfo = Landscape->GetLandscapeInfo();

	LandscapeInfo->UpdateLayerInfoMap(Landscape);

	Landscape->RegisterAllComponents();

	// Need to explicitly call PostEditChange on the LandscapeMaterial property or the landscape proxy won't update its material
	FPropertyChangedEvent MaterialPropertyChangedEvent(FindFieldChecked< FProperty >(Landscape->GetClass(), FName("LandscapeMaterial")));
	Landscape->PostEditChangeProperty(MaterialPropertyChangedEvent);
	Landscape->PostEditChange();

	//Changing Gridsize which will create LandscapestreamProcies, Look at file: LandscapeEditorDetailCustomization_NewLandscape.cpp line 800
	EditorWorldContext.World()->GetSubsystem<ULandscapeSubsystem>()->ChangeGridSize(LandscapeInfo, ComponentsPerProxy);
	

	//Code for assets
	FVector Location(0.0f, 0.0f, 0.0f); FRotator Rotation(0.0f, 0.0f, 0.0f); 
	FActorSpawnParameters SpawnInfo; 
	//AStaticMeshActor* MyNewActor = World->SpawnActor<AStaticMeshActor>(Location, Rotation, SpawnInfo);
	AStaticMeshActor* MyNewActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass());

	UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/Test_assets/Cube_City.Cube_City'"));

	UStaticMeshComponent* MeshComponent = MyNewActor->GetStaticMeshComponent();
	if (MeshComponent)
	{
		MeshComponent->SetStaticMesh(Mesh);
	}

	gridSizeOfProxies = (SizeX - 1) / ((QuadsPerSection * SectionsPerComponent) * ComponentsPerProxy);
	

	//LandscapeInfo->export
	return Landscape;

}

const uint32 CreateLandscape::GetGridSizeOfProxies() const
{
	return gridSizeOfProxies;
}
