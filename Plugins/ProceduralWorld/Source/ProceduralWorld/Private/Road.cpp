// Fill out your copyright notice in the Description page of Project Settings.


#include "Road.h"

Road::Road(const CRSpline& inSpline) : splinePaths{inSpline} {

}
void Road::extend(const ControlPoint &endPoint) {
	
	
	//Ugly ass temp fix for tangent
	ControlPoint lastTangent(endPoint.pos.X + 20, endPoint.pos.Y + 20, endPoint.pos.Z + 20);

	uint32 nmbr_of_CPs = splinePaths[splinePaths.Num() - 1].points.Num();
	CRSpline nextRoadSegment(splinePaths[splinePaths.Num() - 1].points[nmbr_of_CPs -3], splinePaths[splinePaths.Num() - 1].points[nmbr_of_CPs -2], endPoint, lastTangent
		);

	splinePaths.Add(nextRoadSegment);
}

void Road::generateRoad(const TArray<UTile*>& inTiles, const uint32& inGridSize)
{
	FMath math;
	int tileIndex = 0;
	do{
		tileIndex = math.RandRange(0, inGridSize * inGridSize - 1);

	} while (inTiles[tileIndex]->biotope == 2);
	//Now we have found a sutiable tile to start with
	CRSpline spline;

	int32 X = inTiles[tileIndex]->index % inGridSize * (inTiles[tileIndex]->tileSize - 1);
	int32 Y = FMath::Floor(inTiles[tileIndex]->index / inGridSize) * (inTiles[tileIndex]->tileSize - 1);

	//tangent
	spline.addControlPoint( { (float)math.RandRange(X, X + inTiles[tileIndex]->tileSize - 1), (float)math.RandRange(Y, Y + inTiles[tileIndex]->tileSize - 1),(float)33000 });
	//first control point
	spline.addControlPoint({ (float)math.RandRange(X, X + inTiles[tileIndex]->tileSize - 1),(float)math.RandRange(Y, Y + inTiles[tileIndex]->tileSize - 1),(float)33000 });

	//Now we iterate through all adjacent tiles until the road is maxRoadTiles long
	bool IsStuck{ false };
	int maxRoadTiles{ 6 };
		while(maxRoadTiles > 0 && !IsStuck) {

			for (auto& it : inTiles[tileIndex]->adjacentTiles)
			{

				if(it && it->biotope != 2){

					X = it->index % inGridSize * (it->tileSize - 1);
					Y = FMath::Floor(it->index / inGridSize) * (it->tileSize - 1);

					spline.addControlPoint({ (float)math.RandRange(X,X + it->tileSize - 1),(float)math.RandRange(Y,Y + it->tileSize - 1),(float)33000 });
					spline.addControlPoint({ (float)math.RandRange(X,X + it->tileSize - 1),(float)math.RandRange(Y,Y + it->tileSize - 1),(float)33000 });
					
					tileIndex = it->index;
					break;
				}
			}
			maxRoadTiles--;
		}

		//If 4 cp are added, this will construct a spline segment in splinePaths
	if(spline.points.Num() >= 4){
		splinePaths.Add(spline);
	}else{
		UE_LOG(LogTemp, Warning, TEXT("Could not Generate Road, not enough of rechable tiles "));
	}
	
}

void Road::calcLengthsSplines()
{
	for (auto& it : splinePaths)
	{
		it.calcLengths();
	}

}

void Road::vizualizeRoad(const FVector& inLandscapeScale)
{

	for (auto& it : splinePaths)
	{
		it.visualizeSpline(inLandscapeScale);

	}
}

Road::Road()
{
}

Road::~Road()
{
}
