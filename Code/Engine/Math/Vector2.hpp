//-----------------------------------------------------------------------------------------------
// Vector2.hpp
// Vector with float X and Y components


#pragma once


//-----------------------------------------------------------------------------------------------
class Vector2
{
public:
	Vector2();
	Vector2( float initialX, float initialY );
	Vector2( const Vector2& vec );
	Vector2( const Vector2* vec );
	~Vector2();

	bool Vector2::operator < ( const Vector2& rhs ) const;

	void SetXY( float newX, float newY );
	float Length() const;
	float LengthSquared() const;
	Vector2& Normalize();
	Vector2& Negate();

	inline Vector2& Vector2::operator = ( const Vector2& vect ) { x = vect.x; y = vect.y; return *this; }
	inline Vector2& Vector2::operator = ( const float& scalar ) { x = scalar; y = scalar; return *this; }
	inline Vector2& Vector2::operator - ( void ) { x = -x; y = -y; return *this; }
	inline bool Vector2::operator == ( const Vector2& vect ) const { return ( x == vect.x ) && ( y == vect.y ); }
	inline bool Vector2::operator != ( const Vector2& vect ) const { return ( x != vect.x ) || ( y != vect.y ); }

	inline const Vector2 Vector2::operator + ( const Vector2& vect ) const { return Vector2( x + vect.x, y + vect.y ); }
	inline const Vector2 Vector2::operator - ( const Vector2& vect ) const { return Vector2( x - vect.x, y - vect.y ); }
	inline const Vector2 Vector2::operator * ( const Vector2& vect ) const { return Vector2( x * vect.x, y * vect.y ); }
	inline const Vector2 Vector2::operator / ( const Vector2& vect ) const { return Vector2( x / vect.x, y / vect.y ); }

	inline Vector2& Vector2::operator += ( const Vector2& vect ) { x += vect.x; y += vect.y; return *this; }

	inline const Vector2 Vector2::operator + ( float scalar ) const { return Vector2( x + scalar, y + scalar ); }
	inline const Vector2 Vector2::operator - ( float scalar ) const { return Vector2( x - scalar, y - scalar ); }
	inline const Vector2 Vector2::operator * ( float scalar ) const { return Vector2( x * scalar, y * scalar ); }
	inline const Vector2 Vector2::operator / ( float scalar ) const { return Vector2( x / scalar, y / scalar ); }

	inline Vector2& Vector2::operator += ( float scalar ) { x += scalar; y += scalar; return *this; }
	inline Vector2& Vector2::operator -= ( float scalar ) { x -= scalar; y -= scalar; return *this; }
	inline Vector2& Vector2::operator *= ( float scalar ) { x *= scalar; y *= scalar; return *this; }
	inline Vector2& Vector2::operator /= ( float scalar ) { x /= scalar; y /= scalar; return *this; }

	static const Vector2 ZERO;
	static const Vector2 ONE;

public:
	float x;
	float y;
};