#pragma once

#include "Engine/Math/IntVector2.hpp"


//-----------------------------------------------------------------------------------------------
class IntVector2;


//-----------------------------------------------------------------------------------------------
class IntAABB2
{
public:
	IntAABB2();
	IntAABB2( const IntVector2& initialMins, const IntVector2& initialMaxs );
	void ShiftX( int val );
	void ShiftY( int val );

public:
	IntVector2 mins;
	IntVector2 maxs;
};