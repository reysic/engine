//-----------------------------------------------------------------------------------------------
// IntVector2.cpp
// Vector with int X and Y components


//-----------------------------------------------------------------------------------------------
#include "Engine/Math/IntVector2.hpp"


//-----------------------------------------------------------------------------------------------
IntVector2::IntVector2()
{
}


//-----------------------------------------------------------------------------------------------
IntVector2::IntVector2( int initialX, int initialY )
	: x( initialX )
	, y( initialY )
{
}


//-----------------------------------------------------------------------------------------------
std::string IntVector2::ToString()
{
	std::string output = "";

	output += std::to_string( x );
	output += ",";
	output += std::to_string( y );

	return output;
}


//-----------------------------------------------------------------------------------------------
bool IntVector2::operator < ( const IntVector2& rhs ) const
{
	if ( y < rhs.y )
		return true;
	if ( rhs.y < y )
		return false;
	return ( x < rhs.x );
}