//-----------------------------------------------------------------------------------------------
// AABB3.cpp
// Axis-Aligned Bounding Box


//-----------------------------------------------------------------------------------------------
#include "Engine/Math/AABB3.hpp"


//-----------------------------------------------------------------------------------------------
AABB3::AABB3()
	: mins( Vector3( 0.f, 0.f, 0.f ) )
	, maxs( Vector3( 0.f, 0.f, 0.f ) )
{
}


//-----------------------------------------------------------------------------------------------
AABB3::AABB3( const Vector3& initialMins, const Vector3& initialMaxs )
	: mins( initialMins )
	, maxs( initialMaxs )
{
}


//-----------------------------------------------------------------------------------------------
bool DoBoxesOverlap( const AABB3& a, const AABB3& b )
{
	if ( a.maxs.x < b.mins.x )
		return false;

	if ( b.maxs.x < a.mins.x )
		return false;

	if ( a.maxs.y < b.mins.y )
		return false;

	if ( b.maxs.y < a.mins.y )
		return false;

	if ( a.maxs.z < b.mins.z )
		return false;

	if ( b.maxs.z < a.mins.z )
		return false;

	return true;
}