//-----------------------------------------------------------------------------------------------
// Vector3.hpp
// Vector with float X, Y, and Z components


#include <cmath>

#include "Engine/Math/Vector3.hpp"


//-----------------------------------------------------------------------------------------------
Vector3::Vector3()
{
}


//-----------------------------------------------------------------------------------------------
Vector3::Vector3( float initialX, float initialY, float initialZ )
	: x( initialX )
	, y( initialY )
	, z( initialZ )
{
}


//-----------------------------------------------------------------------------------------------
float Vector3::Length() const
{
	return sqrt( ( x * x ) + ( y * y ) + ( z * z ) );
}


//-----------------------------------------------------------------------------------------------
Vector3& Vector3::Normalize()
{
	float length = Length();
	if ( length != 0.f )
	{
		float oneOverLength = ( 1.f / length );
		x *= oneOverLength;
		y *= oneOverLength;
		z *= oneOverLength;
	}

	return *this;
}

//-----------------------------------------------------------------------------------------------
Vector3& Vector3::Negate()
{
	x = -x;
	y = -y;
	z = -z;

	return *this;
}


//-----------------------------------------------------------------------------------------------
Vector3 Vector3::operator*( float scalar )
{
	Vector3 result;
	result.x = x * scalar;
	result.y = y * scalar;
	result.z = z * scalar;
	return result;
}


//-----------------------------------------------------------------------------------------------
Vector3 operator*( float scalar, const Vector3& vec )
{
	Vector3 result;
	result.x = vec.x * scalar;
	result.y = vec.y * scalar;
	result.z = vec.z * scalar;
	return result;
}


//-----------------------------------------------------------------------------------------------
const Vector3 Vector3::ZERO( 0.0f, 0.0f, 0.0f );
const Vector3 Vector3::ONE( 1.0f, 1.0f, 1.0f );
const Vector3 Vector3::UP( 0.0f, 0.0f, 1.0f );