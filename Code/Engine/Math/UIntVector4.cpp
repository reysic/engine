#include "Engine/Math/UIntVector4.hpp"


//-----------------------------------------------------------------------------------------------
UIntVector4::UIntVector4()
{
}


//-----------------------------------------------------------------------------------------------
UIntVector4::UIntVector4( unsigned int initialX, unsigned int initialY, unsigned int initialZ, unsigned int initialW )
	: x( initialX )
	, y( initialY )
	, z( initialZ )
	, w( initialW )
{
}


//-----------------------------------------------------------------------------------------------
const UIntVector4 UIntVector4::ZERO( 0, 0, 0, 0 );