#pragma once


//-----------------------------------------------------------------------------------------------
class UIntVector4
{
public:
	UIntVector4();
	UIntVector4( unsigned int initialX, unsigned int initialY, unsigned int initialZ, unsigned int initialW );

	inline UIntVector4& UIntVector4::operator = ( const UIntVector4& vect ) { x = vect.x; y = vect.y; z = vect.z; w = vect.w; return *this; }
	inline UIntVector4& UIntVector4::operator = ( const unsigned int& scalar ) { x = scalar; y = scalar; z = scalar; w = scalar;  return *this; }
	//inline UIntVector4& UIntVector4::operator - ( void ) { x = -x; y = -y; z = -z;  w = -w;  return *this; }
	inline bool UIntVector4::operator == ( const UIntVector4& vect ) const { return ( x == vect.x ) && ( y == vect.y ) && ( z == vect.z ) && ( w == vect.w ); }
	inline bool UIntVector4::operator != ( const UIntVector4& vect ) const { return ( x != vect.x ) || ( y != vect.y ) || ( z != vect.z ) || ( w != vect.w ); }

	inline const UIntVector4 UIntVector4::operator + ( const UIntVector4& vect ) const { return UIntVector4( x + vect.x, y + vect.y, z + vect.z, w + vect.w ); }
	inline const UIntVector4 UIntVector4::operator - ( const UIntVector4& vect ) const { return UIntVector4( x - vect.x, y - vect.y, z - vect.z, w - vect.w ); }

	inline UIntVector4& UIntVector4::operator += ( const UIntVector4& vect ) { x += vect.x; y += vect.y; z += vect.z; w += vect.w; return *this; }
	inline UIntVector4& UIntVector4::operator -= ( const UIntVector4& vect ) { x -= vect.x; y -= vect.y; z -= vect.z; w -= vect.w; return *this; }

	inline const UIntVector4 UIntVector4::operator * ( unsigned int scalar ) const { return UIntVector4( x * scalar, y * scalar, z * scalar, w * scalar ); }

	inline UIntVector4& UIntVector4::operator += ( unsigned int scalar ) { x += scalar; y += scalar; z += scalar; w += scalar; return *this; }
	inline UIntVector4& UIntVector4::operator -= ( unsigned int scalar ) { x -= scalar; y -= scalar; z -= scalar; w -= scalar;  return *this; }
	inline UIntVector4& UIntVector4::operator *= ( unsigned int scalar ) { x *= scalar; y *= scalar; z *= scalar; w *= scalar; return *this; }

	static const UIntVector4 ZERO;

public:
	unsigned int x;
	unsigned int y;
	unsigned int z;
	unsigned int w;
};