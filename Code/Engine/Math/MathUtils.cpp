//-----------------------------------------------------------------------------------------------
// MathUtils.cpp
// Math helper functions


//-----------------------------------------------------------------------------------------------
#include <math.h>
#include <stdlib.h>
#include <limits>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
// Lookup table for sin() values for 256 evenly distributed angles around the unit circle,
//	where Byte 0 = 0 degrees, Byte 64 = 90 degrees, Byte 128 = 180 degrees, etc.
// Code by Squirrel Eiserloh
const float g_sinValuesForByteAngles[ 256 ] = {
	0.000000000f, 0.024541229f, 0.049067674f, 0.073564564f, 0.098017140f, 0.122410675f, 0.146730474f, 0.170961889f,
	0.195090322f, 0.219101240f, 0.242980180f, 0.266712757f, 0.290284677f, 0.313681740f, 0.336889853f, 0.359895037f,
	0.382683432f, 0.405241314f, 0.427555093f, 0.449611330f, 0.471396737f, 0.492898192f, 0.514102744f, 0.534997620f,
	0.555570233f, 0.575808191f, 0.595699304f, 0.615231591f, 0.634393284f, 0.653172843f, 0.671558955f, 0.689540545f,
	0.707106781f, 0.724247083f, 0.740951125f, 0.757208847f, 0.773010453f, 0.788346428f, 0.803207531f, 0.817584813f,
	0.831469612f, 0.844853565f, 0.857728610f, 0.870086991f, 0.881921264f, 0.893224301f, 0.903989293f, 0.914209756f,
	0.923879533f, 0.932992799f, 0.941544065f, 0.949528181f, 0.956940336f, 0.963776066f, 0.970031253f, 0.975702130f,
	0.980785280f, 0.985277642f, 0.989176510f, 0.992479535f, 0.995184727f, 0.997290457f, 0.998795456f, 0.999698819f,
	1.000000000f, 0.999698819f, 0.998795456f, 0.997290457f, 0.995184727f, 0.992479535f, 0.989176510f, 0.985277642f,
	0.980785280f, 0.975702130f, 0.970031253f, 0.963776066f, 0.956940336f, 0.949528181f, 0.941544065f, 0.932992799f,
	0.923879533f, 0.914209756f, 0.903989293f, 0.893224301f, 0.881921264f, 0.870086991f, 0.857728610f, 0.844853565f,
	0.831469612f, 0.817584813f, 0.803207531f, 0.788346428f, 0.773010453f, 0.757208847f, 0.740951125f, 0.724247083f,
	0.707106781f, 0.689540545f, 0.671558955f, 0.653172843f, 0.634393284f, 0.615231591f, 0.595699304f, 0.575808191f,
	0.555570233f, 0.534997620f, 0.514102744f, 0.492898192f, 0.471396737f, 0.449611330f, 0.427555093f, 0.405241314f,
	0.382683432f, 0.359895037f, 0.336889853f, 0.313681740f, 0.290284677f, 0.266712757f, 0.242980180f, 0.219101240f,
	0.195090322f, 0.170961889f, 0.146730474f, 0.122410675f, 0.098017140f, 0.073564564f, 0.049067674f, 0.024541229f,
	0.000000000f, -0.024541229f, -0.049067674f, -0.073564564f, -0.098017140f, -0.122410675f, -0.146730474f, -0.170961889f,
	-0.195090322f, -0.219101240f, -0.242980180f, -0.266712757f, -0.290284677f, -0.313681740f, -0.336889853f, -0.359895037f,
	-0.382683432f, -0.405241314f, -0.427555093f, -0.449611330f, -0.471396737f, -0.492898192f, -0.514102744f, -0.534997620f,
	-0.555570233f, -0.575808191f, -0.595699304f, -0.615231591f, -0.634393284f, -0.653172843f, -0.671558955f, -0.689540545f,
	-0.707106781f, -0.724247083f, -0.740951125f, -0.757208847f, -0.773010453f, -0.788346428f, -0.803207531f, -0.817584813f,
	-0.831469612f, -0.844853565f, -0.857728610f, -0.870086991f, -0.881921264f, -0.893224301f, -0.903989293f, -0.914209756f,
	-0.923879533f, -0.932992799f, -0.941544065f, -0.949528181f, -0.956940336f, -0.963776066f, -0.970031253f, -0.975702130f,
	-0.980785280f, -0.985277642f, -0.989176510f, -0.992479535f, -0.995184727f, -0.997290457f, -0.998795456f, -0.999698819f,
	-1.000000000f, -0.999698819f, -0.998795456f, -0.997290457f, -0.995184727f, -0.992479535f, -0.989176510f, -0.985277642f,
	-0.980785280f, -0.975702130f, -0.970031253f, -0.963776066f, -0.956940336f, -0.949528181f, -0.941544065f, -0.932992799f,
	-0.923879533f, -0.914209756f, -0.903989293f, -0.893224301f, -0.881921264f, -0.870086991f, -0.857728610f, -0.844853565f,
	-0.831469612f, -0.817584813f, -0.803207531f, -0.788346428f, -0.773010453f, -0.757208847f, -0.740951125f, -0.724247083f,
	-0.707106781f, -0.689540545f, -0.671558955f, -0.653172843f, -0.634393284f, -0.615231591f, -0.595699304f, -0.575808191f,
	-0.555570233f, -0.534997620f, -0.514102744f, -0.492898192f, -0.471396737f, -0.449611330f, -0.427555093f, -0.405241314f,
	-0.382683432f, -0.359895037f, -0.336889853f, -0.313681740f, -0.290284677f, -0.266712757f, -0.242980180f, -0.219101240f,
	-0.195090322f, -0.170961889f, -0.146730474f, -0.122410675f, -0.098017140f, -0.073564564f, -0.049067674f, -0.024541229f
};


//-----------------------------------------------------------------------------------------------
bool DoDiscsOverlap( const Vector2& center1, const float radius1, const Vector2& center2, const float radius2 )
{
	float distanceSquared = CalculateDistanceSquaredBetweenPoints( center1, center2 );
	float radii = radius1 + radius2;
	return ( distanceSquared < ( radii * radii ) );
}


//-----------------------------------------------------------------------------------------------
float CalculateDiscOverlapDepth( const Vector2& center1, const float radius1, const Vector2& center2, const float radius2 )
{
	float totalOfRadii = radius1 + radius2;

	float distanceBetweenDiscCenters = CalculateDistanceBetweenPoints( center1, center2 );

	return ( totalOfRadii - distanceBetweenDiscCenters );
}


//-----------------------------------------------------------------------------------------------
float CalculateDistanceBetweenPoints( const Vector2& point1, const Vector2& point2 )
{
	float xDistance = point2.x - point1.x;
	float yDistance = point2.y - point1.y;
	return sqrt( ( xDistance * xDistance ) + ( yDistance * yDistance ) );
}


//-----------------------------------------------------------------------------------------------
float CalculateDistanceSquaredBetweenPoints( const Vector2& point1, const Vector2& point2 )
{
	float xDistance = point2.x - point1.x;
	float yDistance = point2.y - point1.y;
	return (( xDistance * xDistance ) + ( yDistance * yDistance ) );
}


//-----------------------------------------------------------------------------------------------
float GetRandomFloatNegativeOneToOne()
{
	return ( -1.0f + static_cast <float> ( rand() ) / ( static_cast <float> ( RAND_MAX / ( 1.0f - -1.0f ) ) ) );
}


//-----------------------------------------------------------------------------------------------
float GetVectorAngleDegrees( const Vector2& vector )
{
	return ConvertRadiansToDegrees( atan2( vector.y, vector.x ) );
}


//-----------------------------------------------------------------------------------------------
float RangeMap( float inValue, float inStart, float inEnd, float outStart, float outEnd )
{
	float normalizedInRange = ( inValue - inStart ) / ( inEnd - inStart );
	float normalizedOutRange = normalizedInRange * ( outEnd - outStart );
	float adjustedOutRange = normalizedOutRange + outStart;
	adjustedOutRange = ClampFloat( adjustedOutRange, outStart, outEnd );
	return adjustedOutRange;
}


//-----------------------------------------------------------------------------------------------
float RangeMapZeroToOne( float inValue, float inStart, float inEnd )
{
	float normalizedInRange = ( inValue - inStart ) / ( inEnd - inStart );
	float normalizedOutRange = normalizedInRange * ( 1.0f - 0.0f );
	float adjustedOutRange = normalizedOutRange + 0.0f;
	adjustedOutRange = ClampFloatZeroToOne( adjustedOutRange );
	return adjustedOutRange;
}


//-----------------------------------------------------------------------------------------------
float ClampFloat( float f, float min, float max )
{
	if ( f < min )
		return min;
	
	if ( f > max )
		return max;
	
	return f;
}


//-----------------------------------------------------------------------------------------------
float ClampFloatZeroToOne( float f )
{
	if ( f < 0.0f )
		return 0.0f;

	if ( f > 1.0f )
		return 1.0f;

	return f;
}



//-----------------------------------------------------------------------------------------------
float InterpolateBetweenFloats( float startFloat, float endFloat, float interpolationFactor )
{
	return startFloat + ( ( endFloat- startFloat ) * interpolationFactor );
}


//-----------------------------------------------------------------------------------------------
Vector3 InterpolateBetweenVertices( const Vector3& startVertex, const Vector3& endVertex, float interpolationFactor )
{
	return startVertex + ( ( endVertex - startVertex ) * interpolationFactor );
}


//-----------------------------------------------------------------------------------------------
float ComputeShortestSignedAngularDistance( float startDegrees, float targetDegrees )
{
	// Code by Squirrel Eiserloh
	float angularDistance = targetDegrees - startDegrees; // naive value, only actually correct for simple cases

	while ( angularDistance > 180.f ) // must be the "long way around" (or "wound up" several times...)
		angularDistance -= 360.f;

	while ( angularDistance < -180.f ) // the long way around in the negative direction
		angularDistance += 360.f;

	return angularDistance; // Always a value between -180 and +180; any bigger angle would by definition be the "long way around" and therefore incorrect
}


//-----------------------------------------------------------------------------------------------
float DotProductVector2( const Vector2& vector1, const Vector2& vector2 )
{
	return ( ( vector1.x * vector2.x ) + ( vector1.y * vector2.y ) );
}


//-----------------------------------------------------------------------------------------------
float DotProductVector3( const Vector3& vector1, const Vector3& vector2 )
{
	return ( ( vector1.x * vector2.x ) + ( vector1.y * vector2.y ) + ( vector1.z * vector2.z ) );
}


//-----------------------------------------------------------------------------------------------
float DotProductVector4( const Vector4& vector1, const Vector4& vector2 )
{
	return ( ( vector1.x * vector2.x ) + ( vector1.y * vector2.y ) + ( vector1.z * vector2.z ) + ( vector1.w * vector2.w ) );
}


//-----------------------------------------------------------------------------------------------
Vector3 CrossProductVector3( const Vector3& vector1, const Vector3& vector2 )
{
	Vector3 result;
	result.x = ( vector1.y * vector2.z ) - ( vector1.z * vector2.y );
	result.y = ( vector1.z * vector2.x ) - ( vector1.x * vector2.z );
	result.z = ( vector1.x * vector2.y ) - ( vector1.y * vector2.x );
	return result;
}


//-----------------------------------------------------------------------------------------------
// A replacement for floor(), about 3x faster on my machine.
// Reliable within [-2 billion, +2 billion] or so.  I think.
// Code by Squirrel Eiserloh
float FastFloor( float f )
{
	if ( f >= 0.f )
		return ( float ) ( int ) f;
	else
	{
		float f2 = ( float ) ( int ) f;
		if ( f == f2 )
			return f2;
		else
			return f2 - 1.f;
	}
}


//-----------------------------------------------------------------------------------------------
// A replacement for (int) floor(), about 3x faster on my machine
// Reliable within the range of int.  I think.
// Code by Squirrel Eiserloh
int FastFloorToInt( float f )
{
	if ( f >= 0.f )
		return ( int ) f;
	else
	{
		int i = ( int ) f;
		float f2 = ( float ) i;
		if ( f == f2 )
			return i;
		else
			return i - 1;
	}
}


//-----------------------------------------------------------------------------------------------
bool IsCollinearVector3( const Vector3& vector1, const Vector3& vector2 )
{
	Vector3 crossProduct = CrossProductVector3( vector1, vector2 );

	if ( crossProduct.x == 0.0f && crossProduct.y == 0.0f && crossProduct.z == 0.0f )
	{
		return true;
	}
	else
	{
		return false;
	}
}


//-----------------------------------------------------------------------------------------------
int RandomInt( int HI /*= 0*/, int LO /*= 0 */ )
{
	return ( rand() % ( HI - LO ) ) + LO;
}


//-----------------------------------------------------------------------------------------------
Vector3 Slerp( Vector3 start, Vector3 end, float percent )
{
	// Dot product - the cosine of the angle between 2 vectors.
	float dot = DotProductVector3( start, end );
	// Clamp it to be in the range of Acos()
	// This may be unnecessary, but floating point
	// precision can be a fickle mistress.
	dot = ClampFloat( dot, -1.0f, 1.0f );
	// Acos(dot) returns the angle between start and end,
	// And multiplying that by percent returns the angle between
	// start and the final result.
	float theta = acosf( dot )*percent;
	Vector3 RelativeVec = end - start*dot;
	RelativeVec.Normalize();     // Orthonormal basis
									 // The final result.
	return ( ( start*cos( theta ) ) + ( RelativeVec*sin( theta ) ) );
}


//------------------------------------------------------------------------
Vector3 SlerpForseth( Vector3 const &a, Vector3 const &b, float t )
{
	float al = a.Length();
	float bl = b.Length();

	float len = Lerp<float>( al, bl, t );
	Vector3 u = SlerpUnit( a / al, b / bl, t );
	return len * u;
}


//------------------------------------------------------------------------
Vector3 SlerpUnit( Vector3 const &a, Vector3 const &b, float t )
{
	float cosangle = ClampFloat( DotProductVector3( a, b ), -1.0f, 1.0f );
	float angle = acosf( cosangle );

	if ( angle < std::numeric_limits<float>::epsilon() ) {
		return Lerp( a, b, t );
	}
	else {
		float pos_num = sinf( t * angle );
		float neg_num = sinf( ( 1.0f - t ) * angle );
		float den = sinf( angle );

		return ( neg_num / den ) * a + ( pos_num / den ) * b;
	}
}