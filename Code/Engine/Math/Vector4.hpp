//-----------------------------------------------------------------------------------------------
// Vector4.hpp
// Vector with float X, Y, Z, and W components


#pragma once

#include "Engine/Math/Vector3.hpp"


//-----------------------------------------------------------------------------------------------
class Vector4
{
public:
	Vector4();
	Vector4( float initialX, float initialY, float initialZ, float initialW );
	Vector4( const Vector3& v, const float newW );
	void SetXYZ( float newX, float newY, float newZ, float newW );
	Vector3 GetXYZ();

	float Length() const;
	Vector4& Normalize();
	Vector4& Negate();

	inline Vector4& Vector4::operator = ( const Vector4& vect ) { x = vect.x; y = vect.y; z = vect.z; w = vect.w; return *this; }
	inline Vector4& Vector4::operator = ( const float& scalar ) { x = scalar; y = scalar; z = scalar; w = scalar;  return *this; }
	inline Vector4& Vector4::operator - ( void ) { x = -x; y = -y; z = -z;  w = -w;  return *this; }
	inline bool Vector4::operator == ( const Vector4& vect ) const { return ( x == vect.x ) && ( y == vect.y ) && ( z == vect.z ) && ( w == vect.w ); }
	inline bool Vector4::operator != ( const Vector4& vect ) const { return ( x != vect.x ) || ( y != vect.y ) || ( z != vect.z ) || ( w != vect.w ); }

	inline const Vector4 Vector4::operator + ( const Vector4& vect ) const { return Vector4( x + vect.x, y + vect.y, z + vect.z, w + vect.w ); }
	inline const Vector4 Vector4::operator - ( const Vector4& vect ) const { return Vector4( x - vect.x, y - vect.y, z - vect.z, w - vect.w ); }

	inline Vector4& Vector4::operator += ( const Vector4& vect ) { x += vect.x; y += vect.y; z += vect.z; w += vect.w; return *this; }
	inline Vector4& Vector4::operator -= ( const Vector4& vect ) { x -= vect.x; y -= vect.y; z -= vect.z; w -= vect.w; return *this; }

	inline const Vector4 Vector4::operator * ( float scalar ) const { return Vector4( x * scalar, y * scalar, z * scalar, w * scalar ); }
	inline const Vector4 Vector4::operator / ( float scalar ) const { return Vector4( x / scalar, y / scalar, z / scalar, w / scalar ); }

	inline Vector4& Vector4::operator += ( float scalar ) { x += scalar; y += scalar; z += scalar; w += scalar; return *this; }
	inline Vector4& Vector4::operator -= ( float scalar ) { x -= scalar; y -= scalar; z -= scalar; w -= scalar;  return *this; }
	inline Vector4& Vector4::operator *= ( float scalar ) { x *= scalar; y *= scalar; z *= scalar; w *= scalar; return *this; }

	static const Vector4 ZERO;

public:
	float x;
	float y;
	float z;
	float w;
};