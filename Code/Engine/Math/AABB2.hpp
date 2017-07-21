//-----------------------------------------------------------------------------------------------
// AABB2.hpp
// Axis-Aligned Bounding Box


#pragma once

#include "Engine/Math/Vector2.hpp"


//-----------------------------------------------------------------------------------------------
class Vector2; // forward declaration of Vector2


//-----------------------------------------------------------------------------------------------
class AABB2
{
public:
	AABB2();
	AABB2( const Vector2& initialMins, const Vector2& initialMaxs );

public:
	Vector2 mins;
	Vector2 maxs;
};