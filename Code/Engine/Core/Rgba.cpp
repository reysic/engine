#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
Rgba::Rgba()
	: r( 255 )
	, g( 255 )
	, b( 255 )
	, a( 255 )
{
}


//-----------------------------------------------------------------------------------------------
Rgba::Rgba( unsigned char r, unsigned char g, unsigned char b, unsigned char a )
	: r( r )
	, g( g )
	, b( b )
	, a( a )
{
}


//-----------------------------------------------------------------------------------------------
// Rgba::Rgba( float r, float g, float b, float a )
// {
// 	r = ( unsigned char ) RangeMap( r, 0.0f, 1.0f, 0.0f, 255.0f );
// 	g = ( unsigned char ) RangeMap( r, 0.0f, 1.0f, 0.0f, 255.0f );
// 	b = ( unsigned char ) RangeMap( r, 0.0f, 1.0f, 0.0f, 255.0f );
// 	a = ( unsigned char ) RangeMap( r, 0.0f, 1.0f, 0.0f, 255.0f );
// }


//-----------------------------------------------------------------------------------------------
Rgba::Rgba( const Rgba & rgba )
	: r( rgba.r )
	, g( rgba.g )
	, b( rgba.b )
	, a( rgba.a )
{
}


//-----------------------------------------------------------------------------------------------
Rgba::Rgba( const Rgba * rgba )
	: r( rgba->r )
	, g( rgba->g )
	, b( rgba->b )
	, a( rgba->a )
{
}


//-----------------------------------------------------------------------------------------------
Vector4 Rgba::FloatRepresentation() const
{
	float red = RangeMap( ( float ) r, 0.f, 255.f, 0.f, 1.f );
	float green = RangeMap( ( float ) g, 0.f, 255.f, 0.f, 1.f );
	float blue = RangeMap( ( float ) b, 0.f, 255.f, 0.f, 1.f );
	float alpha = RangeMap( ( float ) a, 0.f, 255.f, 0.f, 1.f );

	return Vector4( red, green, blue, alpha );
}


//-----------------------------------------------------------------------------------------------
const Rgba Rgba::WHITE( 255, 255, 255, 255 );
const Rgba Rgba::BLACK( 0, 0, 0, 255 );
const Rgba Rgba::RED( 255, 0, 0, 255 );
const Rgba Rgba::GREEN( 0, 255, 0, 255 );
const Rgba Rgba::BLUE( 0, 0, 255, 255 );
const Rgba Rgba::CYAN( 0, 255, 255, 255 );
const Rgba Rgba::PINK( 255, 102, 255, 255 );
const Rgba Rgba::GRAY( 128, 128, 128, 255 );
const Rgba Rgba::YELLOW( 255, 255, 0, 255 );
const Rgba Rgba::ORANGE( 255, 165, 0, 255 );
const Rgba Rgba::MAGENTA( 255, 0, 255, 255 );
const Rgba Rgba::NAVY( 0, 0, 32, 255 );
const Rgba Rgba::REDORANGE( 255, 69, 0, 255 );
const Rgba Rgba::LIGHTOLIVEGREEN( 15, 119, 2, 255 );
const Rgba Rgba::GOLDENROD( 255, 215, 0, 255 );
const Rgba Rgba::VIOLET( 148, 0, 211, 255 );


//-----------------------------------------------------------------------------------------------
bool operator==( const Rgba& lhs, const Rgba& rhs )
{
	if ( lhs.r != rhs.r )
	{
		return false;
	}

	if ( lhs.g != rhs.g )
	{
		return false;
	}

	if ( lhs.b != rhs.b )
	{
		return false;
	}

	if ( lhs.a != rhs.a )
	{
		return false;
	}

	return true;
}


//-----------------------------------------------------------------------------------------------
bool operator!=( const Rgba& lhs, const Rgba& rhs )
{
	if ( lhs.r == rhs.r &&
		lhs.g == rhs.g &&
		lhs.b == rhs.b &&
		lhs.a == rhs.a )
	{
		return false;
	}

	return true;
}