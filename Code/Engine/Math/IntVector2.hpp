//-----------------------------------------------------------------------------------------------
// IntVector2.hpp
// Vector with int X and Y components


//-----------------------------------------------------------------------------------------------
#pragma once

#include <string>


//-----------------------------------------------------------------------------------------------
class IntVector2
{
public:
	IntVector2();
	IntVector2( int initialX, int initialY );

	std::string ToString();

	bool IntVector2::operator < ( const IntVector2& rhs ) const;

	inline bool IntVector2::operator == ( const IntVector2& vect ) const { return ( x == vect.x ) && ( y == vect.y ); }
	inline bool IntVector2::operator != ( const IntVector2& vect ) const { return ( x != vect.x ) || ( y != vect.y ); }

public:
	int x;
	int y;
};