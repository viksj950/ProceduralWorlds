// Fill out your copyright notice in the Description page of Project Settings.


#include "CRSpline.h"


CRSpline::CRSpline() :p0{0,0,32768 },p1{50,50, 32768},p2{450,450, 32768},p3{ 600, 700, 32768}
{
	points.Add(p0);
	points.Add(p1);
	points.Add(p2);
	points.Add(p3);


}

CRSpline::~CRSpline()
{
}

controlPoint CRSpline::GetSplinePoint(const float t)
{


	return controlPoint(0,0,0);
}
