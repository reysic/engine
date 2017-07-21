#include "Engine/Math/IntAABB2.hpp"


//-----------------------------------------------------------------------------------------------
IntAABB2::IntAABB2()
	: mins( IntVector2( 0, 0 ) )
	, maxs( IntVector2( 0, 0 ) )
{
}


//-----------------------------------------------------------------------------------------------
IntAABB2::IntAABB2( const IntVector2& initialMins, const IntVector2& initialMaxs )
	: mins( initialMins )
	, maxs( initialMaxs )
{
}


//-----------------------------------------------------------------------------------------------
void IntAABB2::ShiftX( int val )
{
	mins.x += val;
	maxs.x += val;
}


//-----------------------------------------------------------------------------------------------
void IntAABB2::ShiftY( int val )
{
	mins.y += val;
	maxs.y += val;
}