// Fill out your copyright notice in the Description page of Project Settings.


#include "Road.h"

Road::Road(const CRSpline& inSpline) : splinePaths{inSpline} {

}
void Road::extend(const ControlPoint &endPoint) {
	
	
	//Ugly ass temp fix for tangent
	ControlPoint lastTangent(endPoint.pos.X + 20, endPoint.pos.Y + 20, endPoint.pos.Z + 20);

	//nextRoadSegment.points[0] = splinePaths[splinePaths.Num() - 1].points.Last();
	//nextRoadSegment.points[1] = splinePaths[splinePaths.Num() - 1].points[3];
	//nextRoadSegment.points[2] = endPoint;
	//nextRoadSegment.points[3] = lastTangent;

	uint32 nmbr_of_CPs = splinePaths[splinePaths.Num() - 1].points.Num();
	CRSpline nextRoadSegment(splinePaths[splinePaths.Num() - 1].points[nmbr_of_CPs -3], splinePaths[splinePaths.Num() - 1].points[nmbr_of_CPs -2], endPoint, lastTangent
		);
	/*nextRoadSegment.points.Add(splinePaths[splinePaths.Num() - 1].points.Last());
	nextRoadSegment.points.Add(splinePaths[splinePaths.Num() - 1].points[3]);
	nextRoadSegment.points.Add(endPoint);
	nextRoadSegment.points.Add(lastTangent);*/

	splinePaths.Add(nextRoadSegment);
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
