// Fill out your copyright notice in the Description page of Project Settings.


#include "Road.h"

Road::Road(const CRSpline& inSpline, const uint32 inWidth /*= 10*/) : splinePaths{inSpline}, Width{inWidth}
{

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
	spline.addControlPoint( { (float)math.RandRange(X, X + inTiles[tileIndex]->tileSize - 1), (float)math.RandRange(Y, Y + inTiles[tileIndex]->tileSize - 1),(float)35000 });
	//first control point
	spline.addControlPoint({ (float)math.RandRange(X, X + inTiles[tileIndex]->tileSize - 1),(float)math.RandRange(Y, Y + inTiles[tileIndex]->tileSize - 1),(float)35000 });

	//Now we iterate through all adjacent tiles until the road is maxRoadTiles long
	int tries{30};
	int maxRoadTiles{ 6 };
	int32 tileSize;
		while(maxRoadTiles > 0 && tries > 0) {
			tries--;

			/*for (auto& it : inTiles[tileIndex]->adjacentTiles)
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
			maxRoadTiles--;*/

				int32 adjIndex = math.RandRange(0, 7);
				if (inTiles[tileIndex]->adjacentTiles[adjIndex] && inTiles[tileIndex]->adjacentTiles[adjIndex]->biotope != 2)
				{
					tileIndex = inTiles[tileIndex]->adjacentTiles[adjIndex]->index;
					tileSize = inTiles[tileIndex]->tileSize;
					X = tileIndex % inGridSize * ( tileSize - 1);
					Y = FMath::Floor(tileIndex / inGridSize) * (tileSize - 1);

					spline.addControlPoint({ (float)math.RandRange(X,X + tileSize - 1),(float)math.RandRange(Y,Y + tileSize - 1),(float)35000 });
					//tileIndex = adjIndex;
					maxRoadTiles--;
					

				}
			
		}
		//Last tangent
		spline.addControlPoint({ (float)math.RandRange(X,X + tileSize - 1),(float)math.RandRange(Y,Y + tileSize - 1),(float)35000 });

		//If 4 cp are added, this will construct a spline segment in splinePaths
	if(spline.points.Num() >= 4){
		splinePaths.Add(spline);
	}else{
		UE_LOG(LogTemp, Warning, TEXT("Could not Generate Road, not enough of reachable tiles "));
	}
	
}

//void Road::generateRoadSmart(const TArray<UTile*>& inTiles, const uint32& inGridSize)
//{
//	FMath math;
//	int tileIndex = 0;
//
//	do {
//		tileIndex = math.RandRange(0, inGridSize * inGridSize - 1);
//
//	} while (inTiles[tileIndex]->biotope == 2);
//	//Now we have found a sutiable tile to start with
//	CRSpline spline;
//
//	int32 X = inTiles[tileIndex]->index % inGridSize * (inTiles[tileIndex]->tileSize - 1);
//	int32 Y = FMath::Floor(inTiles[tileIndex]->index / inGridSize) * (inTiles[tileIndex]->tileSize - 1);
//
//	//tangent
//	spline.addControlPoint({ (float)math.RandRange(X, X + inTiles[tileIndex]->tileSize - 1), (float)math.RandRange(Y, Y + inTiles[tileIndex]->tileSize - 1),(float)35000 });
//	//first control point
//	spline.addControlPoint({ (float)math.RandRange(X, X + inTiles[tileIndex]->tileSize - 1),(float)math.RandRange(Y, Y + inTiles[tileIndex]->tileSize - 1),(float)35000 });
//
//	//Move to random adjacent tiles but also check if the control point is in a "good" location, meaning check that its not on a hill
//	//Can be done by randomly place the CP but then iterate the segment and check the height of the heightmap, thus detecting hills and such
//	int maxRoadTiles{ 6 };
//	int AdjTries{ 30 };
//	int randomPointTries = 10;
//	int32 tileSize;
//	while (maxRoadTiles > 0 && AdjTries > 0) {
//		AdjTries--;
//		int32 adjIndex = math.RandRange(0, 7);
//		if (inTiles[tileIndex]->adjacentTiles[adjIndex] && inTiles[tileIndex]->adjacentTiles[adjIndex]->biotope != 2)
//		{
//
//			tileIndex = inTiles[tileIndex]->adjacentTiles[adjIndex]->index;
//			tileSize = inTiles[tileIndex]->tileSize;
//			X = tileIndex % inGridSize * (tileSize - 1);
//			Y = FMath::Floor(tileIndex / inGridSize) * (tileSize - 1);
//			//Random cp
//			spline.addControlPoint({ (float)math.RandRange(X,X + tileSize - 1),(float)math.RandRange(Y,Y + tileSize - 1),(float)35000 });
//			//Random Tangent
//			spline.addControlPoint({ (float)math.RandRange(X,X + tileSize - 1),(float)math.RandRange(Y,Y + tileSize - 1),(float)35000 });
//
//			//Iterate the new curve segment
//			spline.calcLengths();
//			float heightvariance = 0;
//			ControlPoint lastSP;
//			ControlPoint nextSP;
//			for (float i = 0; i < spline.TotalLength; i+=2) {
//				nextSP = spline.GetSplinePoint(spline.GetNormalisedOffset(i));
//
//			}
//
//
//
//
//
//			maxRoadTiles--;
//		}
//
//
//
//
//
//	}
//}

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
