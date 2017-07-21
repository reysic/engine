//-----------------------------------------------------------------------------------------------
// PolarCoordinates.hpp
// Coordinate with float r and theta components


//-----------------------------------------------------------------------------------------------
#pragma once


//-----------------------------------------------------------------------------------------------
class PolarCoordinates
{
public:
	// Constructors & Destructors
	PolarCoordinates();
	PolarCoordinates( float radius, float theta );

	// Methods
	void SetRadius( float newRadius );
	void SetTheta( float newTheta );
	float GetRadius() const;
	float GetTheta() const;

public:
	// Members
	float radius;
	float theta;
};