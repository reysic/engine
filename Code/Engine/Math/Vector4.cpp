//-----------------------------------------------------------------------------------------------
// Vector4.hpp
// Vector with float X, Y, and Z components


#include <cmath>

#include "Engine/Math/Vector4.hpp"


//-----------------------------------------------------------------------------------------------
Vector4::Vector4()
{
}


//-----------------------------------------------------------------------------------------------
Vector4::Vector4( float initialX, float initialY, float initialZ, float initialW )
	: x( initialX )
	, y( initialY )
	, z( initialZ )
	, w( initialW )
{
}


//-----------------------------------------------------------------------------------------------
Vector4::Vector4( const Vector3& v, const float newW )
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = newW;
}


//-----------------------------------------------------------------------------------------------
void Vector4::SetXYZ( float newX, float newY, float newZ, float newW )
{
	x = newX;
	y = newY;
	z = newZ;
	w = newW;
}


//-----------------------------------------------------------------------------------------------
Vector3 Vector4::GetXYZ()
{
	Vector3 result;
	result.x = x;
	result.y = y;
	result.z = z;
	return result;
}


//-----------------------------------------------------------------------------------------------
float Vector4::Length() const
{
	return sqrt( ( x * x ) + ( y * y ) + ( z * z ) + ( w * w ) );
}


//-----------------------------------------------------------------------------------------------
Vector4& Vector4::Normalize()
{
	float length = Length();
	if ( length != 0.f )
	{
		float oneOverLength = ( 1.f / length );
		x *= oneOverLength;
		y *= oneOverLength;
		z *= oneOverLength;
		w *= oneOverLength;
	}

	return *this;
}

//-----------------------------------------------------------------------------------------------
Vector4& Vector4::Negate()
{
	x = -x;
	y = -y;
	z = -z;
	w = -w;

	return *this;
}


//-----------------------------------------------------------------------------------------------
const Vector4 Vector4::ZERO( 0.0f, 0.0f, 0.0f, 0.0f );