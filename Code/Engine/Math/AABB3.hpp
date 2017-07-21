//-----------------------------------------------------------------------------------------------
// AABB3.hpp
// Axis-Aligned Bounding Box


#pragma once

#include "Engine/Math/Vector3.hpp"


//-----------------------------------------------------------------------------------------------
class Vector3; // forward declaration of Vector2


//-----------------------------------------------------------------------------------------------
class AABB3
{
public:
	AABB3();
	AABB3( const Vector3& initialMins, const Vector3& initialMaxs );

public:
	Vector3 mins;
	Vector3 maxs;
};


//-----------------------------------------------------------------------------------------------
bool DoBoxesOverlap( const AABB3& a, const AABB3& b );