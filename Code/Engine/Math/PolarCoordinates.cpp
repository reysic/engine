//-----------------------------------------------------------------------------------------------
// PolarCoordinates.cpp
// Coordinate with float r and theta components


//-----------------------------------------------------------------------------------------------
#include "Engine/Math/PolarCoordinates.hpp"


//-----------------------------------------------------------------------------------------------
PolarCoordinates::PolarCoordinates()
{
}


//-----------------------------------------------------------------------------------------------
PolarCoordinates::PolarCoordinates( float initialRadius, float initialTheta )
	: radius( initialRadius )
	, theta( initialTheta )
{
}


//-----------------------------------------------------------------------------------------------
void PolarCoordinates::SetRadius( float newRadius )
{
	radius = newRadius;
}


//-----------------------------------------------------------------------------------------------
void PolarCoordinates::SetTheta( float newTheta )
{
	theta = newTheta;
}


//-----------------------------------------------------------------------------------------------
float PolarCoordinates::GetRadius() const
{
	return radius;
}


//-----------------------------------------------------------------------------------------------
float PolarCoordinates::GetTheta() const
{
	return theta;
}