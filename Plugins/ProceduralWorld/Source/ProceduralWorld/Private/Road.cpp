// Fill out your copyright notice in the Description page of Project Settings.


#include "Road.h"

Road::Road(const CRSpline& inSpline) : splinePaths{inSpline} {

}
void Road::extend(const ControlPoint &endPoint) {
	
	CRSpline nextRoadSegment;
	//Ugly ass temp fix for tangent
	ControlPoint lastTangent(endPoint.pos.X + 20, endPoint.pos.Y + 20, endPoint.pos.Z + 20);

	//nextRoadSegment.points[0] = splinePaths[splinePaths.Num() - 1].points.Last();
	//nextRoadSegment.points[1] = splinePaths[splinePaths.Num() - 1].points[3];
	//nextRoadSegment.points[2] = endPoint;
	//nextRoadSegment.points[3] = lastTangent;

	nextRoadSegment.points.Add(splinePaths[splinePaths.Num() - 1].points.Last());
	nextRoadSegment.points.Add(splinePaths[splinePaths.Num() - 1].points[3]);
	nextRoadSegment.points.Add(endPoint);
	nextRoadSegment.points.Add(lastTangent);

	splinePaths.Add(nextRoadSegment);
}

Road::Road()
{
}

Road::~Road()
{
}
