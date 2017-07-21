//-----------------------------------------------------------------------------------------------
// IntVector3.hpp
// Vector with int X, Y, and Z components


//-----------------------------------------------------------------------------------------------
#pragma once


//-----------------------------------------------------------------------------------------------
class IntVector3
{
public:
	// Constructors & Destructors
	IntVector3();
	IntVector3( int initialX, int initialY, int initialZ );

public:
	// Members
	int x;
	int y;
	int z;
};