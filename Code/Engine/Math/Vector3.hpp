//-----------------------------------------------------------------------------------------------
// Vector3.hpp
// Vector with float X, Y, and Z components


#pragma once


//-----------------------------------------------------------------------------------------------
class Vector3
{
public:
	Vector3();
	Vector3( float initialX, float initialY, float initialZ );

	float Length() const;
	Vector3& Normalize();
	Vector3& Negate();

	Vector3 operator* ( float scalar );
	friend Vector3 operator*( float scalar, const Vector3& vec );

	inline Vector3& Vector3::operator = ( const Vector3& vect ) { x = vect.x; y = vect.y; z = vect.z; return *this; }
	inline Vector3& Vector3::operator = ( const float& scalar ) { x = scalar; y = scalar; z = scalar; return *this; }
	inline Vector3& Vector3::operator - ( void ) { x = -x; y = -y; z = -z;  return *this; }
	inline bool Vector3::operator == ( const Vector3& vect ) const { return ( x == vect.x ) && ( y == vect.y ) && ( z == vect.z ); }
	inline bool Vector3::operator != ( const Vector3& vect ) const { return ( x != vect.x ) || ( y != vect.y ) || ( z != vect.z ); }

	inline const Vector3 Vector3::operator + ( const Vector3& vect ) const { return Vector3( x + vect.x, y + vect.y, z + vect.z ); }
	inline const Vector3 Vector3::operator - ( const Vector3& vect ) const { return Vector3( x - vect.x, y - vect.y, z - vect.z ); }

	inline Vector3& Vector3::operator += ( const Vector3& vect ) { x += vect.x; y += vect.y; z += vect.z; return *this; }
	inline Vector3& Vector3::operator -= ( const Vector3& vect ) { x -= vect.x; y -= vect.y; z -= vect.z; return *this; }

	inline const Vector3 Vector3::operator * ( float scalar ) const { return Vector3( x * scalar, y * scalar, z * scalar ); }
	inline const Vector3 Vector3::operator / ( float scalar ) const { return Vector3( x / scalar, y / scalar, z / scalar ); }

	inline Vector3& Vector3::operator += ( float scalar ) { x += scalar; y += scalar; z += scalar; return *this; }
	inline Vector3& Vector3::operator -= ( float scalar ) { x -= scalar; y -= scalar; z -= scalar; return *this; }
	inline Vector3& Vector3::operator *= ( float scalar ) { x *= scalar; y *= scalar; z *= scalar; return *this; }

	static const Vector3 ZERO;
	static const Vector3 ONE;
	static const Vector3 UP;

public:
	float x;
	float y;
	float z;
};