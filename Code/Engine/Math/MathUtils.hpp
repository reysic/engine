//-----------------------------------------------------------------------------------------------
// MathUtils.hpp
// Math helper functions


#pragma once

#define BIT( x ) ( 1 << ( x ) ); // bit shift macro

#include <cmath>
#include <cstdlib>


//-----------------------------------------------------------------------------------------------
class Vector2; // forward declaration of Vector2
class Vector3; // forward declaration of Vector3
class Vector4; // forward declaration of Vector4


//-----------------------------------------------------------------------------------------------
const float pi = 3.1415926535897932384626433832795f;


//-----------------------------------------------------------------------------------------------
// Constants
// Code by Squirrel Eiserloh
const double TWO_PI = 6.283185307179586476925286766559;
const double PI = 3.1415926535897932384626433832795;
const double HALF_PI = 1.5707963267948966192313216916398;
const double QUARTER_PI = 0.78539816339744830961566084581988;
const double SQRT_2 = 1.4142135623730950488016887242097;
const double SQRT_3 = 1.7320508075688772935274463415059;
const double SQRT_2_OVER_2 = 0.70710678118654752440084436210485;
const double SQRT_3_OVER_2 = 0.86602540378443864676372317075294;
const double SQRT_3_OVER_3 = 0.57735026918962576450914878050196;
const double SQRT_3_OVER_6 = 0.28867513459481288225457439025098;

const float fTWO_PI = 6.283185307179586476925286766559f;
const float fPI = 3.1415926535897932384626433832795f;
const float fHALF_PI = 1.5707963267948966192313216916398f;
const float fQUARTER_PI = 0.78539816339744830961566084581988f;
const float fSQRT_2 = 1.4142135623730950488016887242097f;
const float fSQRT_3 = 1.7320508075688772935274463415059f;
const float fSQRT_2_OVER_2 = 0.70710678118654752440084436210485f;
const float fSQRT_3_OVER_2 = 0.86602540378443864676372317075294f;
const float fSQRT_3_OVER_3 = 0.57735026918962576450914878050196f;
const float fSQRT_3_OVER_6 = 0.28867513459481288225457439025098f;


//-----------------------------------------------------------------------------------------------
// Lookup tables
// Code by Squirrel Eiserloh
extern const float g_sinValuesForByteAngles[ 256 ];


//-----------------------------------------------------------------------------------------------
bool DoDiscsOverlap( const Vector2& center1, const float radius1, const Vector2& center2, const float radius2 );
float CalculateDiscOverlapDepth( const Vector2& center1, const float radius1, const Vector2& center2, const float radius2 );
float CalculateDistanceBetweenPoints( const Vector2& point1, const Vector2& point2 );
float CalculateDistanceSquaredBetweenPoints( const Vector2& point1, const Vector2& point2 );
float GetRandomFloatInRange( float lowerBound, float upperBound);
float GetRandomFloatNegativeOneToOne();
float ConvertDegreesToRadians( float degrees );
float ConvertRadiansToDegrees( float radians );
float GetVectorAngleDegrees( const Vector2& vector );
float CosDegrees( float degrees );
float SinDegrees( float degrees );
float RangeMap( float inValue, float inStart, float inEnd, float outStart, float outEnd );
float RangeMapZeroToOne( float inValue, float inStart, float inEnd );
float ClampFloat( float f, float min, float max );
float ClampFloatZeroToOne( float f );
float InterpolateBetweenFloats( float startFloat, float endFloat, float interpolationFactor );
Vector3 InterpolateBetweenVertices( const Vector3& startVertex, const Vector3& endVertex, float interpolationFactor );
float ComputeShortestSignedAngularDistance( float startDegrees, float targetDegrees );
float DotProductVector2( const Vector2& vector1, const Vector2& vector2 );
float DotProductVector3( const Vector3& vector1, const Vector3& vector2 );
float DotProductVector4( const Vector4& vector1, const Vector4& vector2 );
Vector3 CrossProductVector3( const Vector3& vector1, const Vector3& vector2 );
float SmoothStep( float inputZeroToOne );
float FastFloor( float f );
int FastFloorToInt( float f );
bool IsCollinearVector3( const Vector3& vector1, const Vector3& vector2 );


//-----------------------------------------------------------------------------------------------
// Basic math utilities
// Code by Squirrel Eiserloh
template< typename T > const T& GetMin( const T& a, const T& b );
float GetMinFloat( const float a, const float b );
float GetMinFloat( const float a, const float b, const float c );
float GetMinFloat( const float a, const float b, const float c, const float d );
float GetMaxFloat( const float a, const float b );
float GetMaxFloat( const float a, const float b, const float c );
float GetMaxFloat( const float a, const float b, const float c, const float d );
template< typename T > T Clamp( const T& originalValue, const T& minValue, const T& maxValue );
float ClampZeroToOne( float value );
float ClampNegOneToOne( float value );
float RangeMapFloat( const float inputValue, const float inputRangeStart, const float inputRangeEnd, const float outputRangeStart, const float outputRangeEnd );
const int CalcLogBase2( int x );
const bool IsPowerOfTwo( int x );
float FastFloor( float f );
int FastFloorToInt( float f );


//-----------------------------------------------------------------------------------------------
// Angle and Trig utilities
// 
float ConvertRadiansToDegrees( float radians );
float ConvertDegreesToRadians( float degrees );
double ConvertRadiansToDegrees( double radians );
double ConvertDegreesToRadians( double degrees );
float CosDegrees( float degrees );
float SinDegrees( float degrees );
double CosDegrees( double degrees );
double SinDegrees( double degrees );
float CalcShortestAngularDeltaDegrees( float startDegrees, float endDegrees );


//-----------------------------------------------------------------------------------------------
// Interpolation/easing/tweening utilities
//
float SmoothStep( float inputZeroToOne );
float SmoothStep5( float inputZeroToOne );
float SmoothStart( float inputZeroToOne );
float SmoothStop( float inputZeroToOne );


//-----------------------------------------------------------------------------------------------
// Random number utilities
//
int GetRandomIntInRange( int minValueInclusive, int maxValueInclusive );
int GetRandomIntLessThan( int maxValueNotInclusive );
float GetRandomFloatZeroToOne();
float GetRandomFloatInRange( float minimumInclusive, float maximumInclusive );
bool GetRandomChance( float probabilityOfReturningTrue );
float GetRandomBinomial(); // Returns random number between -1 and 1


/////////////////////////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------------------
// Code by Squirrel Eiserloh
template< typename T >
const T& GetMin( const T& a, const T& b )
{
	return a < b ? a : b;
}


//-----------------------------------------------------------------------------------------------
inline float GetMinFloat( const float a, const float b )
{
	return a < b ? a : b;
}


//-----------------------------------------------------------------------------------------------
inline float GetMinFloat( const float a, const float b, const float c )
{
	if ( a < b )
	{
		return a < c ? a : c;
	}
	else
	{
		return b < c ? b : c;
	}
}


//-----------------------------------------------------------------------------------------------
inline float GetMinFloat( const float a, const float b, const float c, const float d )
{
	if ( a < b )
	{
		if ( a < c )
		{
			return a < d ? a : d;
		}
		else
		{
			return c < d ? c : d;
		}
	}
	else
	{
		if ( b < c )
		{
			return b < d ? b : d;
		}
		else
		{
			return c < d ? c : d;
		}
	}
}


//-----------------------------------------------------------------------------------------------
inline float GetMaxFloat( const float a, const float b )
{
	return a > b ? a : b;
}


//-----------------------------------------------------------------------------------------------
inline float GetMaxFloat( const float a, const float b, const float c )
{
	if ( a > b )
	{
		return a > c ? a : c;
	}
	else
	{
		return b > c ? b : c;
	}
}


//-----------------------------------------------------------------------------------------------
inline float GetMaxFloat( const float a, const float b, const float c, const float d )
{
	if ( a > b )
	{
		if ( a > c )
		{
			return a > d ? a : d;
		}
		else
		{
			return c > d ? c : d;
		}
	}
	else
	{
		if ( b > c )
		{
			return b > d ? b : d;
		}
		else
		{
			return c > d ? c : d;
		}
	}
}


//-----------------------------------------------------------------------------------------------
template< typename T >
T Clamp( const T& originalValue, const T& minValue, const T& maxValue )
{
	T clampedValue = originalValue;
	if ( clampedValue < minValue )
		clampedValue = minValue;
	else if ( clampedValue > maxValue )
		clampedValue = maxValue;

	return clampedValue;
}


//-----------------------------------------------------------------------------------------------
inline float ClampZeroToOne( float value )
{
	if ( value < 0.f )
		return 0.f;
	else if ( value > 1.f )
		return 1.f;
	else
		return value;
}


//-----------------------------------------------------------------------------------------------
inline float ClampNegOneToOne( float value )
{
	if ( value < -1.f )
		return -1.f;
	else if ( value > 1.f )
		return 1.f;
	else
		return value;
}


//-----------------------------------------------------------------------------------------------
inline float RangeMapFloat( const float inputValue, const float inputRangeStart, const float inputRangeEnd, const float outputRangeStart, const float outputRangeEnd )
{
	float inputRangeWidth = inputRangeEnd - inputRangeStart;
	if ( inputRangeWidth != 0.f )
	{
		float fractionInRange = ( inputValue - inputRangeStart ) / inputRangeWidth;
		return outputRangeStart + ( fractionInRange * ( outputRangeEnd - outputRangeStart ) );
	}
	else
	{
		return 0.5f * ( outputRangeStart + outputRangeEnd );
	}
}


//-----------------------------------------------------------------------------------------------
inline float ConvertRadiansToDegrees( float radians )
{
	return radians * ( 180.f / fPI );
}


//-----------------------------------------------------------------------------------------------
inline double ConvertRadiansToDegrees( double radians )
{
	return radians * ( 180.0 / PI );
}


//-----------------------------------------------------------------------------------------------
inline float ConvertDegreesToRadians( float degrees )
{
	return degrees * ( fPI / 180.f );
}


//-----------------------------------------------------------------------------------------------
inline double ConvertDegreesToRadians( double degrees )
{
	return degrees * ( PI / 180.0 );
}


//-----------------------------------------------------------------------------------------------
inline float CosDegrees( float degrees )
{
	return cos( ConvertDegreesToRadians( degrees ) );
}


//-----------------------------------------------------------------------------------------------
inline float SinDegrees( float degrees )
{
	return sin( ConvertDegreesToRadians( degrees ) );
}


//-----------------------------------------------------------------------------------------------
inline double CosDegrees( double degrees )
{
	return cos( ConvertDegreesToRadians( degrees ) );
}


//-----------------------------------------------------------------------------------------------
inline double SinDegrees( double degrees )
{
	return sin( ConvertDegreesToRadians( degrees ) );
}


//-----------------------------------------------------------------------------------------------
inline float CalcShortestAngularDeltaDegrees( float startDegrees, float endDegrees )
{
	float angularDisplacementDegrees = endDegrees - startDegrees;
	while ( angularDisplacementDegrees > 180.f )
		angularDisplacementDegrees -= 360.f;
	while ( angularDisplacementDegrees < -180.f )
		angularDisplacementDegrees += 360.f;

	return angularDisplacementDegrees;
}


//-----------------------------------------------------------------------------------------------
// For a given positive power-of-two x, find L = CalcLogBase2( x ) such that 2^L = X.
//
// For example, CalcLogBase2( 32 ) = 5, since 2^5 = 32.
//
inline const int CalcLogBase2( int x )
{
	int numBitShifts = 0;
	while ( x )
	{
		x >>= 1;
		++numBitShifts;
	}

	return numBitShifts - 1; // e.g. if we shifted 6 times right before zeroing out, we were 00100000 (32), which is 2^5 (or 1<<5).
}


//-----------------------------------------------------------------------------------------------
// Returns true if x is a positive power of two (e.g. 1, 2, 4, 8, 16, 32, 64, 128, 256, 512...)
//
inline const bool IsPowerOfTwo( int x )
{
	return x && !( x & ( x - 1 ) );
}


//-----------------------------------------------------------------------------------------------
inline float SmoothStep( float inputZeroToOne )
{
	return inputZeroToOne * inputZeroToOne * ( 3.f - ( 2.f * inputZeroToOne ) );
}


//-----------------------------------------------------------------------------------------------
inline float SmoothStep5( float inputZeroToOne )
{
	const float& t = inputZeroToOne;
	return t * t * t * ( t * ( t * 6.f - 15.f ) + 10.f );
}


//-----------------------------------------------------------------------------------------------
inline float SmoothStart( float inputZeroToOne )
{
	return ( inputZeroToOne * inputZeroToOne );
}


//-----------------------------------------------------------------------------------------------
inline float SmoothStop( float inputZeroToOne )
{
	float oneMinusInput = 1.f - inputZeroToOne;
	return 1.f - ( oneMinusInput * oneMinusInput );
}


//-----------------------------------------------------------------------------------------------
inline int GetRandomIntInRange( int minValueInclusive, int maxValueInclusive )
{
	// #TODO: use a faster, better random number generator
	// #TODO: use more bits for higher-range numbers
	return minValueInclusive + rand() % ( 1 + maxValueInclusive - minValueInclusive );
}


//-----------------------------------------------------------------------------------------------
inline int GetRandomIntLessThan( int maxValueNotInclusive )
{
	// #TODO: use a faster, better random number generator
	// #TODO: use more bits for higher-range numbers
	return rand() % maxValueNotInclusive;
}


//-----------------------------------------------------------------------------------------------
inline float GetRandomFloatZeroToOne()
{
	// #TODO: use a faster, better random number generator
	const float oneOverRandMax = 1.f / static_cast< float >( RAND_MAX );
	return static_cast< float >( rand() ) * oneOverRandMax;
}


//-----------------------------------------------------------------------------------------------
inline float GetRandomFloatInRange( float minimumInclusive, float maximumInclusive )
{
	// #TODO: use a faster, better random number generator
	const float randomZeroToOne = GetRandomFloatZeroToOne();
	return minimumInclusive + ( randomZeroToOne * ( maximumInclusive - minimumInclusive ) );
}


//-----------------------------------------------------------------------------------------------
inline bool GetRandomChance( float probabilityOfReturningTrue )
{
	if ( probabilityOfReturningTrue >= 1.f )
		return true;

	return GetRandomFloatZeroToOne() < probabilityOfReturningTrue;
}


//-----------------------------------------------------------------------------------------------
inline float GetRandomBinomial()
{
	return GetRandomFloatZeroToOne() - GetRandomFloatZeroToOne();
}


//-----------------------------------------------------------------------------------------------
int RandomInt( int HI = 0, int LO = 0 );


//-----------------------------------------------------------------------------------------------
Vector3 Slerp( Vector3 start, Vector3 end, float percent );
Vector3 SlerpForseth( Vector3 const &a, Vector3 const &b, float t );
Vector3 SlerpUnit( Vector3 const &a, Vector3 const &b, float t );


//-----------------------------------------------------------------------------------------------
template <typename Tvalue, typename Ttime>
inline Tvalue Lerp( Tvalue const &y0, Tvalue const &y1, Ttime const &t )
{
	Ttime const one = Ttime( 1 );
	Tvalue ret = y0 * ( one - t ) + y1 * t;
	return ret;
}


//------------------------------------------------------------------------
template <typename Tvalue, typename Ttime>
inline Tvalue Lerp( Ttime const &x0, Ttime const &x1,
	Tvalue const &y0, Tvalue const &y1,
	Ttime const &x )
{
	Ttime const one = Ttime( 1 );
	Ttime const t = ( x - x0 ) / ( x1 - x0 );
	return Lerp( y0, y1, t );
}


//------------------------------------------------------------------------
template <typename T>
inline T Lerp( T const &x, T const &y, float const &t )
{
	return x * ( 1.0f - t ) + y * t;
}


//------------------------------------------------------------------------
template <>
inline float Lerp( float const &a, float const &b, float const &t )
{
	return Lerp<float, float>( a, b, t );
}


//------------------------------------------------------------------------
template <typename Tvalue, typename Ttime>
inline Tvalue Cubic( Ttime const &x0, Ttime const &x1,
	Tvalue const &y0, Tvalue const &y1,
	Tvalue const &vel0, Tvalue const &vel1,
	Ttime const &x )
{
	Ttime const domain = x1 - x0;
	Tvalue const range = y1 - y0;
	Ttime const t = ( x - x0 ) / domain;
	Tvalue const a = vel0 * ( domain ) -range;
	Tvalue const b = vel1 * ( domain ) +range;

	Ttime it = Ttime( 1 ) - t;

	Tvalue const ret = it * y0
		+ t * y1
		+ ( t * it ) * ( ( a * it ) + ( b * t ) );

	return ret;
}


//------------------------------------------------------------------------
template <typename Tvalue, typename Ttime>
inline Tvalue Cubic( Tvalue const &y0, Tvalue const &y1,
	Ttime const &t )
{
	Tvalue const range = y1 - y0;
	Tvalue const a = -range;
	Tvalue const b = range;

	Ttime const it = Ttime( 1 ) - t;

	Tvalue const ret = it * y0
		+ t * y1
		+ ( t * it ) * ( ( a * it ) + ( b * t ) );

	return ret;
}