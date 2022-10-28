// Fill out your copyright notice in the Description page of Project Settings.


#include "CRSpline.h"


CRSpline::CRSpline(ControlPoint p0, ControlPoint p1, ControlPoint p2, ControlPoint p3)
{
	points.Add(p0);
	points.Add(p1);
	points.Add(p2);
	points.Add(p3);


}

CRSpline::CRSpline()
{

}

CRSpline::~CRSpline()
{
}

ControlPoint CRSpline::GetSplinePoint(float t)
{
	int i0, i1, i2, i3;

	i1 = (int)t + 1; // 3
	i2 = i1 + 1;	//4
	i3 = i2 + 1;	//5
	i0 = i1 - 1;	//2

	t = t - (int)t;
	float tt = t * t;
	float ttt = tt * t;

	float q1 = -ttt + 2.0f * tt - t;
	float q2 = 3.0f * ttt - 5.0f * tt + 2.0f;
	float q3 = -3.0f * ttt + 4.0f * tt + t;
	float q4 = ttt - tt;

	ControlPoint res;

	res.pos.X = tension * (points[i0].pos.X * q1 + points[i1].pos.X * q2 + points[i2].pos.X * q3 + points[i3].pos.X * q4);
	res.pos.Y = tension * (points[i0].pos.Y * q1 + points[i1].pos.Y * q2 + points[i2].pos.Y * q3 + points[i3].pos.Y * q4);
	res.pos.Z = tension * (points[i0].pos.Z * q1 + points[i1].pos.Z * q2 + points[i2].pos.Z * q3 + points[i3].pos.Z * q4);

	//UE_LOG(LogTemp, Warning, TEXT("res.pos.X :  %f"), res.pos.X);
	//UE_LOG(LogTemp, Warning, TEXT("res.pos.Y :  %f"), res.pos.Y);
	//UE_LOG(LogTemp, Warning, TEXT("res.pos.Z :  %f"), res.pos.Z);

	return res;
}

ControlPoint CRSpline::GetSplineGradient(float t)
{
	int i0, i1, i2, i3;

	i1 = (int)t + 1;
	i2 = i1 + 1;
	i3 = i2 + 1;
	i0 = i1 - 1;

	t = t - (int)t;
	float tt = t * t;
	float ttt = tt * t;

	float q1 = -3.0f * tt + 4.0f * t - 1;
	float q2 = 9.0f * tt - 10.0f * t;
	float q3 = -9.0f * tt + 8.0f * t + 1.0f;
	float q4 = 3.0f * tt - 2.0f * t;

	ControlPoint res;

	res.pos.X = tension * (points[i0].pos.X * q1 + points[i1].pos.X * q2 + points[i2].pos.X * q3 + points[i3].pos.X * q4);
	res.pos.Y = tension * (points[i0].pos.Y * q1 + points[i1].pos.Y * q2 + points[i2].pos.Y * q3 + points[i3].pos.Y * q4);
	res.pos.Z = tension * (points[i0].pos.Z * q1 + points[i1].pos.Z * q2 + points[i2].pos.Z * q3 + points[i3].pos.Z * q4);

	return res;
}

float CRSpline::calcSegmentLength(int cp_index, float stepSize = 0.005f)
{
	float resLength = 0.0f;

	ControlPoint old_point, new_point;
	old_point = GetSplinePoint((float)cp_index);
	UE_LOG(LogTemp, Warning, TEXT("CP assignment (old_point)  %s"), *old_point.pos.ToString());

	for (float t = 0; t < 1.0f; t += stepSize)
	{
		new_point = GetSplinePoint((float)cp_index + t);
		resLength += sqrtf((new_point.pos.X - old_point.pos.X) * (new_point.pos.X - old_point.pos.X)
			+ (new_point.pos.Y - old_point.pos.Y) * (new_point.pos.Y - old_point.pos.Y)
			+ (new_point.pos.Z - old_point.pos.Z) * (new_point.pos.Z - old_point.pos.Z));
		old_point = new_point;
	}


	return resLength;
}

float CRSpline::GetNormalisedOffset(float p)
{
	int i = 0;
	while (p > points[i].length)
	{
		p -= points[i].length;
		i++;
	}
	UE_LOG(LogTemp, Warning, TEXT("Output  %f"), (float)i + (p / points[i].length));
	//The fraction is the offset 
	return (float)i + (p / points[i].length);
}

void CRSpline::calcLengths()
{
	TotalLength = 0;
	UE_LOG(LogTemp, Warning, TEXT("Number of control points:  %d"), points.Num());
	for (size_t i = 0; i < points.Num()-3; i++)
	{
		points[i].length = calcSegmentLength(i);
		TotalLength += points[i].length;
	}
}

void CRSpline::addControlPoint(const ControlPoint& cp)
{
	points.Add(cp);
}

void CRSpline::visualizeSpline(const FVector &inLandscapeScale)
{
	UWorld* World = nullptr;
	FWorldContext& EditorWorldContext = GEditor->GetEditorWorldContext();
	World = EditorWorldContext.World();

	FVector Location;
	FRotator Rotation;
	FVector assetScale;
	FActorSpawnParameters SpawnInfo;

	float scaleValue = 0.25;

	for (int i = 0; i < points.Num(); i++) //Control Points
	{

		Location = points[i].pos;
		float temp = Location.X * inLandscapeScale.X;
		Location.X = Location.Y * inLandscapeScale.Y;
		Location.Y = temp;
		Location.Z = (Location.Z - 32768) * (100.0f / 128.0f);
		//UE_LOG(LogTemp, Warning, TEXT("Location (CP): %s"), *Location.ToString());

		AStaticMeshActor* CP_cube = World->SpawnActor<AStaticMeshActor>(Location, Rotation, SpawnInfo);
		assetScale = { scaleValue ,scaleValue ,scaleValue };
		CP_cube->SetActorScale3D(assetScale);

		UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/Test_assets/Control_Point.Control_Point'"));

		UStaticMeshComponent* MeshComponent = CP_cube->GetStaticMeshComponent();
		if (MeshComponent)
		{
			MeshComponent->SetStaticMesh(Mesh);
		}

	}
	scaleValue = 0.15;

	for (float i = 0; i <= TotalLength; i += 100.0f) //On line points 
	{
		Location = GetSplinePoint(GetNormalisedOffset(i)).pos;
		UE_LOG(LogTemp, Warning, TEXT("Location (SP): %s"), *Location.ToString());
		float temp = Location.X;
		Location.X = Location.Y * 100.0f;
		Location.Y = temp * 100.0f;
		Location.Z = (Location.Z - 32768) * (100.0f / 128.0f);
		AStaticMeshActor* CP_cube = World->SpawnActor<AStaticMeshActor>(Location, Rotation, SpawnInfo);
		assetScale = { scaleValue ,scaleValue ,scaleValue };
		CP_cube->SetActorScale3D(assetScale);

		UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/Test_assets/Cube_Ghost.Cube_Ghost'"));

		UStaticMeshComponent* MeshComponent = CP_cube->GetStaticMeshComponent();
		if (MeshComponent)
		{
			MeshComponent->SetStaticMesh(Mesh);
		}
	}
}

	
