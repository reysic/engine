//-----------------------------------------------------------------------------------------------
// Noise.cpp
// Code by Squirrel Eiserloh


//-----------------------------------------------------------------------------------------------
#include <math.h>

#include "Engine/Math/Noise.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/IntVector2.hpp"


//-----------------------------------------------------------------------------------------------
// Returns an unsigned integer containing 32 reasonably-well-scrambled bits, based on a given
//	(signed) integer input parameter (position/index) and [optional] seed.  Kind of like looking
//	up a value in an infinitely large [non-existent] table of previously generated random numbers.
//
// The base bit-noise constants were chosen semi-methodically to have distinctive and interesting
//	bits, and have so far produced excellent experimental results under moderate testing.
//
unsigned int Get1dNoiseUint( int positionX, unsigned int seed )
{
	const unsigned int BIT_NOISE1 = 0xB5297A4D; // 0b1011'0101'0010'1001'0111'1010'0100'1101;
	const unsigned int BIT_NOISE2 = 0x68E31DA4; // 0b0110'1000'1110'0011'0001'1101'1010'0100;
	const unsigned int BIT_NOISE3 = 0x1B56C4E9; // 0b0001'1011'0101'0110'1100'0100'1110'1001;

	unsigned int mangledBits = ( unsigned int ) positionX;
	mangledBits *= BIT_NOISE1;
	mangledBits += seed;
	mangledBits ^= ( mangledBits >> 8 );
	mangledBits += BIT_NOISE2;
	mangledBits ^= ( mangledBits << 8 );
	mangledBits *= BIT_NOISE3;
	mangledBits ^= ( mangledBits >> 8 );
	return mangledBits;
}


//-----------------------------------------------------------------------------------------------
float Compute1dFractalNoise( float position, float scale, unsigned int numOctaves, float octavePersistence, float octaveScale, bool renormalize, unsigned int seed )
{
	const float OCTAVE_OFFSET = 0.636764989593174f; // Translation/bias to add to each octave

	float totalNoise = 0.f;
	float totalAmplitude = 0.f;
	float currentAmplitude = 1.f;
	float currentPosition = position * ( 1.f / scale );

	for ( unsigned int octaveNum = 0; octaveNum < numOctaves; ++octaveNum )
	{
		// Determine noise values at nearby integer "grid point" positions
		float positionFloor = FastFloor( currentPosition );
		int indexWest = ( int ) positionFloor;
		int indexEast = indexWest + 1;
		float valueWest = Get1dNoiseZeroToOne( indexWest, seed );
		float valueEast = Get1dNoiseZeroToOne( indexEast, seed );

		// Do a smoothed (nonlinear) weighted average of nearby grid point values
		float distanceFromWest = currentPosition - positionFloor;
		float weightEast = SmoothStep( distanceFromWest ); // Gives rounder (nonlinear) results
		float weightWest = 1.f - weightEast;
		float noiseZeroToOne = ( valueWest * weightWest ) + ( valueEast * weightEast );
		float noiseThisOctave = 2.f * ( noiseZeroToOne - 0.5f ); // Map from [0,1] to [-1,1]

		// Accumulate results and prepare for next octave (if any)
		totalNoise += noiseThisOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPosition *= octaveScale;
		currentPosition += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		++seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if ( renormalize && totalAmplitude > 0.f )
	{
		totalNoise /= totalAmplitude;				// Amplitude exceeds 1.0 if octaves are used!
		totalNoise = ( totalNoise * 0.5f ) + 0.5f;	// Map to [0,1]
		totalNoise = SmoothStep( totalNoise );		// Push towards extents (octaves pull us away)
		totalNoise = ( totalNoise * 2.0f ) - 1.f;		// Map back to [-1,1]
	}

	return totalNoise;
}


//-----------------------------------------------------------------------------------------------
float Compute2dFractalNoise( float posX, float posY, float scale, unsigned int numOctaves, float octavePersistence, float octaveScale, bool renormalize, unsigned int seed )
{
	const float OCTAVE_OFFSET = 0.636764989593174f; // Translation/bias to add to each octave

	float totalNoise = 0.f;
	float totalAmplitude = 0.f;
	float currentAmplitude = 1.f;
	float invScale = ( 1.f / scale );
	Vector2 currentPos( posX * invScale, posY * invScale );

	for ( unsigned int octaveNum = 0; octaveNum < numOctaves; ++octaveNum )
	{
		// Determine noise values at nearby integer "grid point" positions
		Vector2 cellMins( FastFloor( currentPos.x ), FastFloor( currentPos.y ) );
		int indexWestX = ( int ) cellMins.x;
		int indexSouthY = ( int ) cellMins.y;
		int indexEastX = indexWestX + 1;
		int indexNorthY = indexSouthY + 1;
		float valueSouthWest = Get2dNoiseZeroToOne( indexWestX, indexSouthY, seed );
		float valueSouthEast = Get2dNoiseZeroToOne( indexEastX, indexSouthY, seed );
		float valueNorthWest = Get2dNoiseZeroToOne( indexWestX, indexNorthY, seed );
		float valueNorthEast = Get2dNoiseZeroToOne( indexEastX, indexNorthY, seed );

		// Do a smoothed (nonlinear) weighted average of nearby grid point values
		Vector2 displacementFromMins = currentPos - cellMins;
		float weightEast = SmoothStep( displacementFromMins.x );
		float weightNorth = SmoothStep( displacementFromMins.y );
		float weightWest = 1.f - weightEast;
		float weightSouth = 1.f - weightNorth;

		float blendSouth = ( weightEast * valueSouthEast ) + ( weightWest * valueSouthWest );
		float blendNorth = ( weightEast * valueNorthEast ) + ( weightWest * valueNorthWest );
		float blendTotal = ( weightSouth * blendSouth ) + ( weightNorth * blendNorth );
		float noiseThisOctave = 2.f * ( blendTotal - 0.5f ); // Map from [0,1] to [-1,1]

		// Accumulate results and prepare for next octave (if any)
		totalNoise += noiseThisOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPos *= octaveScale;
		currentPos.x += OCTAVE_OFFSET; // Add "irrational" offsets to noise position components
		currentPos.y += OCTAVE_OFFSET; //	at each octave to break up their grid alignment
		++seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if ( renormalize && totalAmplitude > 0.f )
	{
		totalNoise /= totalAmplitude;				// Amplitude exceeds 1.0 if octaves are used
		totalNoise = ( totalNoise * 0.5f ) + 0.5f;	// Map to [0,1]
		totalNoise = SmoothStep( totalNoise );		// Push towards extents (octaves pull us away)
		totalNoise = ( totalNoise * 2.0f ) - 1.f;		// Map back to [-1,1]
	}

	return totalNoise;
}


//-----------------------------------------------------------------------------------------------
float Compute3dFractalNoise( float posX, float posY, float posZ, float scale, unsigned int numOctaves, float octavePersistence, float octaveScale, bool renormalize, unsigned int seed )
{
	const float OCTAVE_OFFSET = 0.636764989593174f; // Translation/bias to add to each octave

	float totalNoise = 0.f;
	float totalAmplitude = 0.f;
	float currentAmplitude = 1.f;
	float invScale = ( 1.f / scale );
	Vector3 currentPos( posX * invScale, posY * invScale, posZ * invScale );

	for ( unsigned int octaveNum = 0; octaveNum < numOctaves; ++octaveNum )
	{
		// Determine noise values at nearby integer "grid point" positions
		Vector3 cellMins( FastFloor( currentPos.x ), FastFloor( currentPos.y ), FastFloor( currentPos.z ) );
		int indexWestX = ( int ) cellMins.x;
		int indexSouthY = ( int ) cellMins.y;
		int indexBelowZ = ( int ) cellMins.z;
		int indexEastX = indexWestX + 1;
		int indexNorthY = indexSouthY + 1;
		int indexAboveZ = indexBelowZ + 1;

		// Noise grid cell has 8 corners in 3D
		float aboveSouthWest = Get3dNoiseZeroToOne( indexWestX, indexSouthY, indexAboveZ, seed );
		float aboveSouthEast = Get3dNoiseZeroToOne( indexEastX, indexSouthY, indexAboveZ, seed );
		float aboveNorthWest = Get3dNoiseZeroToOne( indexWestX, indexNorthY, indexAboveZ, seed );
		float aboveNorthEast = Get3dNoiseZeroToOne( indexEastX, indexNorthY, indexAboveZ, seed );
		float belowSouthWest = Get3dNoiseZeroToOne( indexWestX, indexSouthY, indexBelowZ, seed );
		float belowSouthEast = Get3dNoiseZeroToOne( indexEastX, indexSouthY, indexBelowZ, seed );
		float belowNorthWest = Get3dNoiseZeroToOne( indexWestX, indexNorthY, indexBelowZ, seed );
		float belowNorthEast = Get3dNoiseZeroToOne( indexEastX, indexNorthY, indexBelowZ, seed );

		// Do a smoothed (nonlinear) weighted average of nearby grid point values
		Vector3 displacementFromMins = currentPos - cellMins;

		float weightEast = SmoothStep( displacementFromMins.x );
		float weightNorth = SmoothStep( displacementFromMins.y );
		float weightAbove = SmoothStep( displacementFromMins.z );
		float weightWest = 1.f - weightEast;
		float weightSouth = 1.f - weightNorth;
		float weightBelow = 1.f - weightAbove;

		// 8-way blend (8 -> 4 -> 2 -> 1)
		float blendBelowSouth = ( weightEast * belowSouthEast ) + ( weightWest * belowSouthWest );
		float blendBelowNorth = ( weightEast * belowNorthEast ) + ( weightWest * belowNorthWest );
		float blendAboveSouth = ( weightEast * aboveSouthEast ) + ( weightWest * aboveSouthWest );
		float blendAboveNorth = ( weightEast * aboveNorthEast ) + ( weightWest * aboveNorthWest );
		float blendBelow = ( weightSouth * blendBelowSouth ) + ( weightNorth * blendBelowNorth );
		float blendAbove = ( weightSouth * blendAboveSouth ) + ( weightNorth * blendAboveNorth );
		float blendTotal = ( weightBelow * blendBelow ) + ( weightAbove * blendAbove );
		float noiseThisOctave = 2.f * ( blendTotal - 0.5f ); // Map from [0,1] to [-1,1]

		// Accumulate results and prepare for next octave (if any)
		totalNoise += noiseThisOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPos *= octaveScale;
		currentPos.x += OCTAVE_OFFSET; // Add "irrational" offsets to noise position components
		currentPos.y += OCTAVE_OFFSET; //	at each octave to break up their grid alignment
		currentPos.z += OCTAVE_OFFSET;
		++seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if ( renormalize && totalAmplitude > 0.f )
	{
		totalNoise /= totalAmplitude;				// Amplitude exceeds 1.0 if octaves are used
		totalNoise = ( totalNoise * 0.5f ) + 0.5f;	// Map to [0,1]
		totalNoise = SmoothStep( totalNoise );		// Push towards extents (octaves pull us away)
		totalNoise = ( totalNoise * 2.0f ) - 1.f;		// Map back to [-1,1]
	}

	return totalNoise;
}


//-----------------------------------------------------------------------------------------------
float Compute4dFractalNoise( float posX, float posY, float posZ, float posT, float scale, unsigned int numOctaves, float octavePersistence, float octaveScale, bool renormalize, unsigned int seed )
{
	const float OCTAVE_OFFSET = 0.636764989593174f; // Translation/bias to add to each octave

	float totalNoise = 0.f;
	float totalAmplitude = 0.f;
	float currentAmplitude = 1.f;
	float invScale = ( 1.f / scale );
	Vector4 currentPos( posX * invScale, posY * invScale, posZ * invScale, posT * invScale );

	for ( unsigned int octaveNum = 0; octaveNum < numOctaves; ++octaveNum )
	{
		// Determine noise values at nearby integer "grid point" positions
		Vector4 cellMins( FastFloor( currentPos.x ), FastFloor( currentPos.y ), FastFloor( currentPos.z ), FastFloor( currentPos.w ) );
		int indexWestX = ( int ) cellMins.x;
		int indexSouthY = ( int ) cellMins.y;
		int indexBelowZ = ( int ) cellMins.z;
		int indexBeforeT = ( int ) cellMins.w;
		int indexEastX = indexWestX + 1;
		int indexNorthY = indexSouthY + 1;
		int indexAboveZ = indexBelowZ + 1;
		int indexAfterT = indexBeforeT + 1;

		// Noise grid cell has 16 "corners" in 4D
		float beforeBelowSW = Get4dNoiseZeroToOne( indexWestX, indexSouthY, indexBelowZ, indexBeforeT, seed );
		float beforeBelowSE = Get4dNoiseZeroToOne( indexEastX, indexSouthY, indexBelowZ, indexBeforeT, seed );
		float beforeBelowNW = Get4dNoiseZeroToOne( indexWestX, indexNorthY, indexBelowZ, indexBeforeT, seed );
		float beforeBelowNE = Get4dNoiseZeroToOne( indexEastX, indexNorthY, indexBelowZ, indexBeforeT, seed );
		float beforeAboveSW = Get4dNoiseZeroToOne( indexWestX, indexSouthY, indexAboveZ, indexBeforeT, seed );
		float beforeAboveSE = Get4dNoiseZeroToOne( indexEastX, indexSouthY, indexAboveZ, indexBeforeT, seed );
		float beforeAboveNW = Get4dNoiseZeroToOne( indexWestX, indexNorthY, indexAboveZ, indexBeforeT, seed );
		float beforeAboveNE = Get4dNoiseZeroToOne( indexEastX, indexNorthY, indexAboveZ, indexBeforeT, seed );

		float afterBelowSW = Get4dNoiseZeroToOne( indexWestX, indexSouthY, indexBelowZ, indexAfterT, seed );
		float afterBelowSE = Get4dNoiseZeroToOne( indexEastX, indexSouthY, indexBelowZ, indexAfterT, seed );
		float afterBelowNW = Get4dNoiseZeroToOne( indexWestX, indexNorthY, indexBelowZ, indexAfterT, seed );
		float afterBelowNE = Get4dNoiseZeroToOne( indexEastX, indexNorthY, indexBelowZ, indexAfterT, seed );
		float afterAboveSW = Get4dNoiseZeroToOne( indexWestX, indexSouthY, indexAboveZ, indexAfterT, seed );
		float afterAboveSE = Get4dNoiseZeroToOne( indexEastX, indexSouthY, indexAboveZ, indexAfterT, seed );
		float afterAboveNW = Get4dNoiseZeroToOne( indexWestX, indexNorthY, indexAboveZ, indexAfterT, seed );
		float afterAboveNE = Get4dNoiseZeroToOne( indexEastX, indexNorthY, indexAboveZ, indexAfterT, seed );

		// Do a smoothed (nonlinear) weighted average of nearby grid point values
		Vector4 displacementFromMins = currentPos - cellMins;

		float weightEast = SmoothStep( displacementFromMins.x );
		float weightNorth = SmoothStep( displacementFromMins.y );
		float weightAbove = SmoothStep( displacementFromMins.z );
		float weightAfter = SmoothStep( displacementFromMins.w );
		float weightWest = 1.f - weightEast;
		float weightSouth = 1.f - weightNorth;
		float weightBelow = 1.f - weightAbove;
		float weightBefore = 1.f - weightAfter;

		// 16-way blend (16 -> 8 -> 4 -> 2 -> 1)
		float blendBeforeBelowSouth = ( weightEast * beforeBelowSE ) + ( weightWest * beforeBelowSW );
		float blendBeforeBelowNorth = ( weightEast * beforeBelowNE ) + ( weightWest * beforeBelowNW );
		float blendBeforeAboveSouth = ( weightEast * beforeAboveSE ) + ( weightWest * beforeAboveSW );
		float blendBeforeAboveNorth = ( weightEast * beforeAboveNE ) + ( weightWest * beforeAboveNW );
		float blendAfterBelowSouth = ( weightEast * afterBelowSE ) + ( weightWest * afterBelowSW );
		float blendAfterBelowNorth = ( weightEast * afterBelowNE ) + ( weightWest * afterBelowNW );
		float blendAfterAboveSouth = ( weightEast * afterAboveSE ) + ( weightWest * afterAboveSW );
		float blendAfterAboveNorth = ( weightEast * afterAboveNE ) + ( weightWest * afterAboveNW );
		float blendBeforeBelow = ( weightSouth * blendBeforeBelowSouth ) + ( weightNorth * blendBeforeBelowNorth );
		float blendBeforeAbove = ( weightSouth * blendBeforeAboveSouth ) + ( weightNorth * blendBeforeAboveNorth );
		float blendAfterBelow = ( weightSouth * blendAfterBelowSouth ) + ( weightNorth * blendAfterBelowNorth );
		float blendAfterAbove = ( weightSouth * blendAfterAboveSouth ) + ( weightNorth * blendAfterAboveNorth );
		float blendBefore = ( weightBelow * blendBeforeBelow ) + ( weightAbove * blendBeforeAbove );
		float blendAfter = ( weightBelow * blendAfterBelow ) + ( weightAbove * blendAfterAbove );
		float blendTotal = ( weightBefore * blendBefore ) + ( weightAfter * blendAfter );
		float noiseThisOctave = 2.f * ( blendTotal - 0.5f ); // Map from [0,1] to [-1,1]

		// Accumulate results and prepare for next octave (if any)
		totalNoise += noiseThisOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPos *= octaveScale;
		currentPos.x += OCTAVE_OFFSET; // Add "irrational" offsets to noise position components
		currentPos.y += OCTAVE_OFFSET; //	at each octave to break up their grid alignment
		currentPos.z += OCTAVE_OFFSET;
		currentPos.w += OCTAVE_OFFSET;
		++seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if ( renormalize && totalAmplitude > 0.f )
	{
		totalNoise /= totalAmplitude;				// Amplitude exceeds 1.0 if octaves are used
		totalNoise = ( totalNoise * 0.5f ) + 0.5f;	// Map to [0,1]
		totalNoise = SmoothStep( totalNoise );		// Push towards extents (octaves pull us away)
		totalNoise = ( totalNoise * 2.0f ) - 1.f;		// Map back to [-1,1]
	}

	return totalNoise;
}


//-----------------------------------------------------------------------------------------------
// Perlin noise is fractal noise with "gradient vector smoothing" applied.
//
// In 1D, the gradients are trivial: -1.0 or 1.0, so resulting noise is boring at one octave.
//
float Compute1dPerlinNoise( float position, float scale, unsigned int numOctaves, float octavePersistence, float octaveScale, bool renormalize, unsigned int seed )
{
	const float OCTAVE_OFFSET = 0.636764989593174f; // Translation/bias to add to each octave
	const float gradients[ 2 ] = { -1.f, 1.f }; // 1D unit "gradient" vectors; one back, one forward

	float totalNoise = 0.f;
	float totalAmplitude = 0.f;
	float currentAmplitude = 1.f;
	float currentPosition = position * ( 1.f / scale );

	for ( unsigned int octaveNum = 0; octaveNum < numOctaves; ++octaveNum )
	{
		// Determine random "gradient vectors" (just +1 or -1 for 1D Perlin) for surrounding corners
		float positionFloor = ( float ) FastFloor( currentPosition );
		int indexWest = ( int ) positionFloor;
		int indexEast = indexWest + 1;
		float gradientWest = gradients[ Get1dNoiseUint( indexWest, seed ) & 0x00000001 ];
		float gradientEast = gradients[ Get1dNoiseUint( indexEast, seed ) & 0x00000001 ];

		// Dot each point's gradient with displacement from point to position
		float displacementFromWest = currentPosition - positionFloor; // always positive
		float displacementFromEast = displacementFromWest - 1.f; // always negative
		float dotWest = gradientWest * displacementFromWest; // 1D "dot product" is... multiply
		float dotEast = gradientEast * displacementFromEast;

		// Do a smoothed (nonlinear) weighted average of dot results
		float weightEast = SmoothStep( displacementFromWest );
		float weightWest = 1.f - weightEast;
		float blendTotal = ( weightWest * dotWest ) + ( weightEast * dotEast );
		float noiseThisOctave = 2.f * blendTotal; // 1D Perlin is in [-.5,.5]; map to [-1,1]

		// Accumulate results and prepare for next octave (if any)
		totalNoise += noiseThisOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPosition *= octaveScale;
		currentPosition += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		++seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if ( renormalize && totalAmplitude > 0.f )
	{
		totalNoise /= totalAmplitude;				// Amplitude exceeds 1.0 if octaves are used
		totalNoise = ( totalNoise * 0.5f ) + 0.5f;	// Map to [0,1]
		totalNoise = SmoothStep( totalNoise );		// Push towards extents (octaves pull us away)
		totalNoise = ( totalNoise * 2.0f ) - 1.f;		// Map back to [-1,1]
	}

	return totalNoise;
}


//-----------------------------------------------------------------------------------------------
// Perlin noise is fractal noise with "gradient vector smoothing" applied.
//
// In 2D, gradients are unit-length vectors in various directions with even angular distribution.
//
float Compute2dPerlinNoise( float posX, float posY, float scale, unsigned int numOctaves, float octavePersistence, float octaveScale, bool renormalize, unsigned int seed )
{
	const float OCTAVE_OFFSET = 0.636764989593174f; // Translation/bias to add to each octave
	const Vector2 gradients[ 8 ] = // Normalized unit vectors in 8 quarter-cardinal directions
	{
		Vector2( +0.923879533f, +0.382683432f ), // 22.5 degrees
		Vector2( +0.382683432f, +0.923879533f ), // 67.5 degrees
		Vector2( -0.382683432f, +0.923879533f ), // 112.5 degrees
		Vector2( -0.923879533f, +0.382683432f ), // 157.5 degrees
		Vector2( -0.923879533f, -0.382683432f ), // 202.5 degrees
		Vector2( -0.382683432f, -0.923879533f ), // 247.5 degrees
		Vector2( +0.382683432f, -0.923879533f ), // 292.5 degrees
		Vector2( +0.923879533f, -0.382683432f )	 // 337.5 degrees
	};

	float totalNoise = 0.f;
	float totalAmplitude = 0.f;
	float currentAmplitude = 1.f;
	float invScale = ( 1.f / scale );
	Vector2 currentPos( posX * invScale, posY * invScale );

	for ( unsigned int octaveNum = 0; octaveNum < numOctaves; ++octaveNum )
	{
		// Determine random unit "gradient vectors" for surrounding corners
		Vector2 cellMins( FastFloor( currentPos.x ), FastFloor( currentPos.y ) );
		Vector2 cellMaxs( cellMins.x + 1.f, cellMins.y + 1.f );
		int indexWestX = ( int ) cellMins.x;
		int indexSouthY = ( int ) cellMins.y;
		int indexEastX = indexWestX + 1;
		int indexNorthY = indexSouthY + 1;

		unsigned int noiseSW = Get2dNoiseUint( indexWestX, indexSouthY, seed );
		unsigned int noiseSE = Get2dNoiseUint( indexEastX, indexSouthY, seed );
		unsigned int noiseNW = Get2dNoiseUint( indexWestX, indexNorthY, seed );
		unsigned int noiseNE = Get2dNoiseUint( indexEastX, indexNorthY, seed );

		Vector2 gradientSW = gradients[ noiseSW & 0x00000007 ];
		Vector2 gradientSE = gradients[ noiseSE & 0x00000007 ];
		Vector2 gradientNW = gradients[ noiseNW & 0x00000007 ];
		Vector2 gradientNE = gradients[ noiseNE & 0x00000007 ];

		// Dot each corner's gradient with displacement from corner to position
		Vector2 displacementFromSW( currentPos.x - cellMins.x, currentPos.y - cellMins.y );
		Vector2 displacementFromSE( currentPos.x - cellMaxs.x, currentPos.y - cellMins.y );
		Vector2 displacementFromNW( currentPos.x - cellMins.x, currentPos.y - cellMaxs.y );
		Vector2 displacementFromNE( currentPos.x - cellMaxs.x, currentPos.y - cellMaxs.y );

		float dotSouthWest = DotProductVector2( gradientSW, displacementFromSW );
		float dotSouthEast = DotProductVector2( gradientSE, displacementFromSE );
		float dotNorthWest = DotProductVector2( gradientNW, displacementFromNW );
		float dotNorthEast = DotProductVector2( gradientNE, displacementFromNE );

		// Do a smoothed (nonlinear) weighted average of dot results
		float weightEast = SmoothStep5( displacementFromSW.x );
		float weightNorth = SmoothStep5( displacementFromSW.y );
		float weightWest = 1.f - weightEast;
		float weightSouth = 1.f - weightNorth;

		float blendSouth = ( weightEast * dotSouthEast ) + ( weightWest * dotSouthWest );
		float blendNorth = ( weightEast * dotNorthEast ) + ( weightWest * dotNorthWest );
		float blendTotal = ( weightSouth * blendSouth ) + ( weightNorth * blendNorth );
		float noiseThisOctave = 1.5f * blendTotal; // 2D Perlin is in ~[-.66,.66]; map to ~[-1,1]

		// Accumulate results and prepare for next octave (if any)
		totalNoise += noiseThisOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPos *= octaveScale;
		currentPos.x += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		currentPos.y += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		++seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if ( renormalize && totalAmplitude > 0.f )
	{
		totalNoise /= totalAmplitude;				// Amplitude exceeds 1.0 if octaves are used
		totalNoise = ( totalNoise * 0.5f ) + 0.5f;	// Map to [0,1]
		totalNoise = SmoothStep( totalNoise );		// Push towards extents (octaves pull us away)
		totalNoise = ( totalNoise * 2.0f ) - 1.f;		// Map back to [-1,1]
	}

	return totalNoise;
}


//-----------------------------------------------------------------------------------------------
// Perlin noise is fractal noise with "gradient vector smoothing" applied.
//
// In 3D, gradients are unit-length vectors in random (3D) directions.
//
float Compute3dPerlinNoise( float posX, float posY, float posZ, float scale, unsigned int numOctaves, float octavePersistence, float octaveScale, bool renormalize, unsigned int seed )
{
	const float OCTAVE_OFFSET = 0.636764989593174f; // Translation/bias to add to each octave

	const Vector3 gradients[ 8 ] = // Traditional "12 edges" requires modulus and isn't any better.
	{
		Vector3( +fSQRT_3_OVER_3, +fSQRT_3_OVER_3, +fSQRT_3_OVER_3 ), // Normalized unit 3D vectors
		Vector3( -fSQRT_3_OVER_3, +fSQRT_3_OVER_3, +fSQRT_3_OVER_3 ), //  pointing toward cube
		Vector3( +fSQRT_3_OVER_3, -fSQRT_3_OVER_3, +fSQRT_3_OVER_3 ), //  corners, so components
		Vector3( -fSQRT_3_OVER_3, -fSQRT_3_OVER_3, +fSQRT_3_OVER_3 ), //  are all sqrt(3)/3, i.e.
		Vector3( +fSQRT_3_OVER_3, +fSQRT_3_OVER_3, -fSQRT_3_OVER_3 ), // 0.5773502691896257645091f.
		Vector3( -fSQRT_3_OVER_3, +fSQRT_3_OVER_3, -fSQRT_3_OVER_3 ), // These are slightly better
		Vector3( +fSQRT_3_OVER_3, -fSQRT_3_OVER_3, -fSQRT_3_OVER_3 ), // than axes (1,0,0) and much
		Vector3( -fSQRT_3_OVER_3, -fSQRT_3_OVER_3, -fSQRT_3_OVER_3 )  // faster than edges (1,1,0).
	};

	float totalNoise = 0.f;
	float totalAmplitude = 0.f;
	float currentAmplitude = 1.f;
	float invScale = ( 1.f / scale );
	Vector3 currentPos( posX * invScale, posY * invScale, posZ * invScale );

	for ( unsigned int octaveNum = 0; octaveNum < numOctaves; ++octaveNum )
	{
		// Determine random unit "gradient vectors" for surrounding corners
		Vector3 cellMins( FastFloor( currentPos.x ), FastFloor( currentPos.y ), FastFloor( currentPos.z ) );
		Vector3 cellMaxs( cellMins.x + 1.f, cellMins.y + 1.f, cellMins.z + 1.f );
		int indexWestX = ( int ) cellMins.x;
		int indexSouthY = ( int ) cellMins.y;
		int indexBelowZ = ( int ) cellMins.z;
		int indexEastX = indexWestX + 1;
		int indexNorthY = indexSouthY + 1;
		int indexAboveZ = indexBelowZ + 1;

		unsigned int noiseBelowSW = Get3dNoiseUint( indexWestX, indexSouthY, indexBelowZ, seed );
		unsigned int noiseBelowSE = Get3dNoiseUint( indexEastX, indexSouthY, indexBelowZ, seed );
		unsigned int noiseBelowNW = Get3dNoiseUint( indexWestX, indexNorthY, indexBelowZ, seed );
		unsigned int noiseBelowNE = Get3dNoiseUint( indexEastX, indexNorthY, indexBelowZ, seed );
		unsigned int noiseAboveSW = Get3dNoiseUint( indexWestX, indexSouthY, indexAboveZ, seed );
		unsigned int noiseAboveSE = Get3dNoiseUint( indexEastX, indexSouthY, indexAboveZ, seed );
		unsigned int noiseAboveNW = Get3dNoiseUint( indexWestX, indexNorthY, indexAboveZ, seed );
		unsigned int noiseAboveNE = Get3dNoiseUint( indexEastX, indexNorthY, indexAboveZ, seed );

		Vector3 gradientBelowSW = gradients[ noiseBelowSW & 0x00000007 ];
		Vector3 gradientBelowSE = gradients[ noiseBelowSE & 0x00000007 ];
		Vector3 gradientBelowNW = gradients[ noiseBelowNW & 0x00000007 ];
		Vector3 gradientBelowNE = gradients[ noiseBelowNE & 0x00000007 ];
		Vector3 gradientAboveSW = gradients[ noiseAboveSW & 0x00000007 ];
		Vector3 gradientAboveSE = gradients[ noiseAboveSE & 0x00000007 ];
		Vector3 gradientAboveNW = gradients[ noiseAboveNW & 0x00000007 ];
		Vector3 gradientAboveNE = gradients[ noiseAboveNE & 0x00000007 ];

		// Dot each corner's gradient with displacement from corner to position
		Vector3 displacementFromBelowSW( currentPos.x - cellMins.x, currentPos.y - cellMins.y, currentPos.z - cellMins.z );
		Vector3 displacementFromBelowSE( currentPos.x - cellMaxs.x, currentPos.y - cellMins.y, currentPos.z - cellMins.z );
		Vector3 displacementFromBelowNW( currentPos.x - cellMins.x, currentPos.y - cellMaxs.y, currentPos.z - cellMins.z );
		Vector3 displacementFromBelowNE( currentPos.x - cellMaxs.x, currentPos.y - cellMaxs.y, currentPos.z - cellMins.z );
		Vector3 displacementFromAboveSW( currentPos.x - cellMins.x, currentPos.y - cellMins.y, currentPos.z - cellMaxs.z );
		Vector3 displacementFromAboveSE( currentPos.x - cellMaxs.x, currentPos.y - cellMins.y, currentPos.z - cellMaxs.z );
		Vector3 displacementFromAboveNW( currentPos.x - cellMins.x, currentPos.y - cellMaxs.y, currentPos.z - cellMaxs.z );
		Vector3 displacementFromAboveNE( currentPos.x - cellMaxs.x, currentPos.y - cellMaxs.y, currentPos.z - cellMaxs.z );

		float dotBelowSW = DotProductVector3( gradientBelowSW, displacementFromBelowSW );
		float dotBelowSE = DotProductVector3( gradientBelowSE, displacementFromBelowSE );
		float dotBelowNW = DotProductVector3( gradientBelowNW, displacementFromBelowNW );
		float dotBelowNE = DotProductVector3( gradientBelowNE, displacementFromBelowNE );
		float dotAboveSW = DotProductVector3( gradientAboveSW, displacementFromAboveSW );
		float dotAboveSE = DotProductVector3( gradientAboveSE, displacementFromAboveSE );
		float dotAboveNW = DotProductVector3( gradientAboveNW, displacementFromAboveNW );
		float dotAboveNE = DotProductVector3( gradientAboveNE, displacementFromAboveNE );

		// Do a smoothed (nonlinear) weighted average of dot results
		float weightEast = SmoothStep5( displacementFromBelowSW.x );
		float weightNorth = SmoothStep5( displacementFromBelowSW.y );
		float weightAbove = SmoothStep5( displacementFromBelowSW.z );
		float weightWest = 1.f - weightEast;
		float weightSouth = 1.f - weightNorth;
		float weightBelow = 1.f - weightAbove;

		// 8-way blend (8 -> 4 -> 2 -> 1)
		float blendBelowSouth = ( weightEast * dotBelowSE ) + ( weightWest * dotBelowSW );
		float blendBelowNorth = ( weightEast * dotBelowNE ) + ( weightWest * dotBelowNW );
		float blendAboveSouth = ( weightEast * dotAboveSE ) + ( weightWest * dotAboveSW );
		float blendAboveNorth = ( weightEast * dotAboveNE ) + ( weightWest * dotAboveNW );
		float blendBelow = ( weightSouth * blendBelowSouth ) + ( weightNorth * blendBelowNorth );
		float blendAbove = ( weightSouth * blendAboveSouth ) + ( weightNorth * blendAboveNorth );
		float blendTotal = ( weightBelow * blendBelow ) + ( weightAbove * blendAbove );
		float noiseThisOctave = 1.66666666f * blendTotal; // 3D Perlin is ~[-.6,.6]; map to ~[-1,1]

		// Accumulate results and prepare for next octave (if any)
		totalNoise += noiseThisOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPos *= octaveScale;
		currentPos.x += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		currentPos.y += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		currentPos.z += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		++seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if ( renormalize && totalAmplitude > 0.f )
	{
		totalNoise /= totalAmplitude;				// Amplitude exceeds 1.0 if octaves are used
		totalNoise = ( totalNoise * 0.5f ) + 0.5f;	// Map to [0,1]
		totalNoise = SmoothStep( totalNoise );		// Push towards extents (octaves pull us away)
		totalNoise = ( totalNoise * 2.0f ) - 1.f;		// Map back to [-1,1]
	}

	return totalNoise;
}


//-----------------------------------------------------------------------------------------------
// Perlin noise is fractal noise with "gradient vector smoothing" applied.
//
// In 4D, gradients are unit-length hyper-vectors in random (4D) directions.
//
float Compute4dPerlinNoise( float posX, float posY, float posZ, float posT, float scale, unsigned int numOctaves, float octavePersistence, float octaveScale, bool renormalize, unsigned int seed )
{
	const float OCTAVE_OFFSET = 0.636764989593174f; // Translation/bias to add to each octave

	const Vector4 gradients[ 16 ] = // Hard to tell if this is any better in 4D than just having 8
	{
		Vector4( +0.5f, +0.5f, +0.5f, +0.5f ), // Normalized unit 4D vectors pointing toward each
		Vector4( -0.5f, +0.5f, +0.5f, +0.5f ), //  of the 16 hypercube corners, so components are
		Vector4( +0.5f, -0.5f, +0.5f, +0.5f ), //  all sqrt(4)/4, i.e. one-half.
		Vector4( -0.5f, -0.5f, +0.5f, +0.5f ), // 
		Vector4( +0.5f, +0.5f, -0.5f, +0.5f ), // It's hard to tell whether these are any better
		Vector4( -0.5f, +0.5f, -0.5f, +0.5f ), //  or worse than vectors facing axes (1,0,0,0) or
		Vector4( +0.5f, -0.5f, -0.5f, +0.5f ), //  3D edges (.7,.7,0,0) or 4D edges (.57,.57,.57,0)
		Vector4( -0.5f, -0.5f, -0.5f, +0.5f ), //  but less-axial gradients looked a little better
		Vector4( +0.5f, +0.5f, +0.5f, -0.5f ), //  with 2D and 3D noise so I'm assuming this is as
		Vector4( -0.5f, +0.5f, +0.5f, -0.5f ), //  good or better as any other gradient-selection
		Vector4( +0.5f, -0.5f, +0.5f, -0.5f ), //  scheme (and is crazy-fast).  *shrug*
		Vector4( -0.5f, -0.5f, +0.5f, -0.5f ), // 
		Vector4( +0.5f, +0.5f, -0.5f, -0.5f ), // Plus, we want a power-of-two number of evenly-
		Vector4( -0.5f, +0.5f, -0.5f, -0.5f ), //  distributed gradients, so we can cheaply select
		Vector4( +0.5f, -0.5f, -0.5f, -0.5f ), //  one from bit-noise (use bit-mask, not modulus).
		Vector4( -0.5f, -0.5f, -0.5f, -0.5f )  // 
	};

	float totalNoise = 0.f;
	float totalAmplitude = 0.f;
	float currentAmplitude = 1.f;
	float invScale = ( 1.f / scale );
	Vector4 currentPos( posX * invScale, posY * invScale, posZ * invScale, posT * invScale );

	for ( unsigned int octaveNum = 0; octaveNum < numOctaves; ++octaveNum )
	{
		// Determine random unit "gradient vectors" for 16 surrounding 4D (hypercube) cell corners
		Vector4 cellMins( FastFloor( currentPos.x ), FastFloor( currentPos.y ), FastFloor( currentPos.z ), FastFloor( currentPos.w ) );
		Vector4 cellMaxs( cellMins.x + 1.f, cellMins.y + 1.f, cellMins.z + 1.f, cellMins.w + 1.f );
		int indexWestX = ( int ) cellMins.x;
		int indexSouthY = ( int ) cellMins.y;
		int indexBelowZ = ( int ) cellMins.z;
		int indexBeforeT = ( int ) cellMins.w;
		int indexEastX = indexWestX + 1;
		int indexNorthY = indexSouthY + 1;
		int indexAboveZ = indexBelowZ + 1;
		int indexAfterT = indexBeforeT + 1;

		// "BeforeBSW" stands for "BeforeBelowSouthWest" below (i.e. 4D hypercube mins), etc.
		unsigned int noiseBeforeBSW = Get4dNoiseUint( indexWestX, indexSouthY, indexBelowZ, indexBeforeT, seed );
		unsigned int noiseBeforeBSE = Get4dNoiseUint( indexEastX, indexSouthY, indexBelowZ, indexBeforeT, seed );
		unsigned int noiseBeforeBNW = Get4dNoiseUint( indexWestX, indexNorthY, indexBelowZ, indexBeforeT, seed );
		unsigned int noiseBeforeBNE = Get4dNoiseUint( indexEastX, indexNorthY, indexBelowZ, indexBeforeT, seed );
		unsigned int noiseBeforeASW = Get4dNoiseUint( indexWestX, indexSouthY, indexAboveZ, indexBeforeT, seed );
		unsigned int noiseBeforeASE = Get4dNoiseUint( indexEastX, indexSouthY, indexAboveZ, indexBeforeT, seed );
		unsigned int noiseBeforeANW = Get4dNoiseUint( indexWestX, indexNorthY, indexAboveZ, indexBeforeT, seed );
		unsigned int noiseBeforeANE = Get4dNoiseUint( indexEastX, indexNorthY, indexAboveZ, indexBeforeT, seed );
		unsigned int noiseAfterBSW = Get4dNoiseUint( indexWestX, indexSouthY, indexBelowZ, indexAfterT, seed );
		unsigned int noiseAfterBSE = Get4dNoiseUint( indexEastX, indexSouthY, indexBelowZ, indexAfterT, seed );
		unsigned int noiseAfterBNW = Get4dNoiseUint( indexWestX, indexNorthY, indexBelowZ, indexAfterT, seed );
		unsigned int noiseAfterBNE = Get4dNoiseUint( indexEastX, indexNorthY, indexBelowZ, indexAfterT, seed );
		unsigned int noiseAfterASW = Get4dNoiseUint( indexWestX, indexSouthY, indexAboveZ, indexAfterT, seed );
		unsigned int noiseAfterASE = Get4dNoiseUint( indexEastX, indexSouthY, indexAboveZ, indexAfterT, seed );
		unsigned int noiseAfterANW = Get4dNoiseUint( indexWestX, indexNorthY, indexAboveZ, indexAfterT, seed );
		unsigned int noiseAfterANE = Get4dNoiseUint( indexEastX, indexNorthY, indexAboveZ, indexAfterT, seed );

		Vector4 gradientBeforeBSW = gradients[ noiseBeforeBSW & 0x0000000F ];
		Vector4 gradientBeforeBSE = gradients[ noiseBeforeBSE & 0x0000000F ];
		Vector4 gradientBeforeBNW = gradients[ noiseBeforeBNW & 0x0000000F ];
		Vector4 gradientBeforeBNE = gradients[ noiseBeforeBNE & 0x0000000F ];
		Vector4 gradientBeforeASW = gradients[ noiseBeforeASW & 0x0000000F ];
		Vector4 gradientBeforeASE = gradients[ noiseBeforeASE & 0x0000000F ];
		Vector4 gradientBeforeANW = gradients[ noiseBeforeANW & 0x0000000F ];
		Vector4 gradientBeforeANE = gradients[ noiseBeforeANE & 0x0000000F ];
		Vector4 gradientAfterBSW = gradients[ noiseAfterBSW & 0x0000000F ];
		Vector4 gradientAfterBSE = gradients[ noiseAfterBSE & 0x0000000F ];
		Vector4 gradientAfterBNW = gradients[ noiseAfterBNW & 0x0000000F ];
		Vector4 gradientAfterBNE = gradients[ noiseAfterBNE & 0x0000000F ];
		Vector4 gradientAfterASW = gradients[ noiseAfterASW & 0x0000000F ];
		Vector4 gradientAfterASE = gradients[ noiseAfterASE & 0x0000000F ];
		Vector4 gradientAfterANW = gradients[ noiseAfterANW & 0x0000000F ];
		Vector4 gradientAfterANE = gradients[ noiseAfterANE & 0x0000000F ];

		// Dot each corner's gradient with displacement from corner to position
		Vector4 displacementFromBeforeBSW( currentPos.x - cellMins.x, currentPos.y - cellMins.y, currentPos.z - cellMins.z, currentPos.w - cellMins.w );
		Vector4 displacementFromBeforeBSE( currentPos.x - cellMaxs.x, currentPos.y - cellMins.y, currentPos.z - cellMins.z, currentPos.w - cellMins.w );
		Vector4 displacementFromBeforeBNW( currentPos.x - cellMins.x, currentPos.y - cellMaxs.y, currentPos.z - cellMins.z, currentPos.w - cellMins.w );
		Vector4 displacementFromBeforeBNE( currentPos.x - cellMaxs.x, currentPos.y - cellMaxs.y, currentPos.z - cellMins.z, currentPos.w - cellMins.w );
		Vector4 displacementFromBeforeASW( currentPos.x - cellMins.x, currentPos.y - cellMins.y, currentPos.z - cellMaxs.z, currentPos.w - cellMins.w );
		Vector4 displacementFromBeforeASE( currentPos.x - cellMaxs.x, currentPos.y - cellMins.y, currentPos.z - cellMaxs.z, currentPos.w - cellMins.w );
		Vector4 displacementFromBeforeANW( currentPos.x - cellMins.x, currentPos.y - cellMaxs.y, currentPos.z - cellMaxs.z, currentPos.w - cellMins.w );
		Vector4 displacementFromBeforeANE( currentPos.x - cellMaxs.x, currentPos.y - cellMaxs.y, currentPos.z - cellMaxs.z, currentPos.w - cellMins.w );
		Vector4 displacementFromAfterBSW( currentPos.x - cellMins.x, currentPos.y - cellMins.y, currentPos.z - cellMins.z, currentPos.w - cellMaxs.w );
		Vector4 displacementFromAfterBSE( currentPos.x - cellMaxs.x, currentPos.y - cellMins.y, currentPos.z - cellMins.z, currentPos.w - cellMaxs.w );
		Vector4 displacementFromAfterBNW( currentPos.x - cellMins.x, currentPos.y - cellMaxs.y, currentPos.z - cellMins.z, currentPos.w - cellMaxs.w );
		Vector4 displacementFromAfterBNE( currentPos.x - cellMaxs.x, currentPos.y - cellMaxs.y, currentPos.z - cellMins.z, currentPos.w - cellMaxs.w );
		Vector4 displacementFromAfterASW( currentPos.x - cellMins.x, currentPos.y - cellMins.y, currentPos.z - cellMaxs.z, currentPos.w - cellMaxs.w );
		Vector4 displacementFromAfterASE( currentPos.x - cellMaxs.x, currentPos.y - cellMins.y, currentPos.z - cellMaxs.z, currentPos.w - cellMaxs.w );
		Vector4 displacementFromAfterANW( currentPos.x - cellMins.x, currentPos.y - cellMaxs.y, currentPos.z - cellMaxs.z, currentPos.w - cellMaxs.w );
		Vector4 displacementFromAfterANE( currentPos.x - cellMaxs.x, currentPos.y - cellMaxs.y, currentPos.z - cellMaxs.z, currentPos.w - cellMaxs.w );

		float dotBeforeBSW = DotProductVector4( gradientBeforeBSW, displacementFromBeforeBSW );
		float dotBeforeBSE = DotProductVector4( gradientBeforeBSE, displacementFromBeforeBSE );
		float dotBeforeBNW = DotProductVector4( gradientBeforeBNW, displacementFromBeforeBNW );
		float dotBeforeBNE = DotProductVector4( gradientBeforeBNE, displacementFromBeforeBNE );
		float dotBeforeASW = DotProductVector4( gradientBeforeASW, displacementFromBeforeASW );
		float dotBeforeASE = DotProductVector4( gradientBeforeASE, displacementFromBeforeASE );
		float dotBeforeANW = DotProductVector4( gradientBeforeANW, displacementFromBeforeANW );
		float dotBeforeANE = DotProductVector4( gradientBeforeANE, displacementFromBeforeANE );
		float dotAfterBSW = DotProductVector4( gradientAfterBSW, displacementFromAfterBSW );
		float dotAfterBSE = DotProductVector4( gradientAfterBSE, displacementFromAfterBSE );
		float dotAfterBNW = DotProductVector4( gradientAfterBNW, displacementFromAfterBNW );
		float dotAfterBNE = DotProductVector4( gradientAfterBNE, displacementFromAfterBNE );
		float dotAfterASW = DotProductVector4( gradientAfterASW, displacementFromAfterASW );
		float dotAfterASE = DotProductVector4( gradientAfterASE, displacementFromAfterASE );
		float dotAfterANW = DotProductVector4( gradientAfterANW, displacementFromAfterANW );
		float dotAfterANE = DotProductVector4( gradientAfterANE, displacementFromAfterANE );

		// Do a smoothed (nonlinear) weighted average of dot results
		float weightEast = SmoothStep( displacementFromBeforeBSW.x );
		float weightNorth = SmoothStep( displacementFromBeforeBSW.y );
		float weightAbove = SmoothStep( displacementFromBeforeBSW.z );
		float weightAfter = SmoothStep( displacementFromBeforeBSW.w );
		float weightWest = 1.f - weightEast;
		float weightSouth = 1.f - weightNorth;
		float weightBelow = 1.f - weightAbove;
		float weightBefore = 1.f - weightAfter;

		// 16-way blend (16 -> 8 -> 4 -> 2 -> 1)
		float blendBeforeBelowSouth = ( weightEast * dotBeforeBSE ) + ( weightWest * dotBeforeBSW );
		float blendBeforeBelowNorth = ( weightEast * dotBeforeBNE ) + ( weightWest * dotBeforeBNW );
		float blendBeforeAboveSouth = ( weightEast * dotBeforeASE ) + ( weightWest * dotBeforeASW );
		float blendBeforeAboveNorth = ( weightEast * dotBeforeANE ) + ( weightWest * dotBeforeANW );
		float blendAfterBelowSouth = ( weightEast * dotAfterBSE ) + ( weightWest * dotAfterBSW );
		float blendAfterBelowNorth = ( weightEast * dotAfterBNE ) + ( weightWest * dotAfterBNW );
		float blendAfterAboveSouth = ( weightEast * dotAfterASE ) + ( weightWest * dotAfterASW );
		float blendAfterAboveNorth = ( weightEast * dotAfterANE ) + ( weightWest * dotAfterANW );
		float blendBeforeBelow = ( weightSouth * blendBeforeBelowSouth ) + ( weightNorth * blendBeforeBelowNorth );
		float blendBeforeAbove = ( weightSouth * blendBeforeAboveSouth ) + ( weightNorth * blendBeforeAboveNorth );
		float blendAfterBelow = ( weightSouth * blendAfterBelowSouth ) + ( weightNorth * blendAfterBelowNorth );
		float blendAfterAbove = ( weightSouth * blendAfterAboveSouth ) + ( weightNorth * blendAfterAboveNorth );
		float blendBefore = ( weightBelow * blendBeforeBelow ) + ( weightAbove * blendBeforeAbove );
		float blendAfter = ( weightBelow * blendAfterBelow ) + ( weightAbove * blendAfterAbove );
		float blendTotal = ( weightBefore * blendBefore ) + ( weightAfter * blendAfter );
		float noiseThisOctave = 1.6f * blendTotal; // 4D Perlin is in ~[-.5,.5]; map to ~[-1,1]

		// Accumulate results and prepare for next octave (if any)
		totalNoise += noiseThisOctave * currentAmplitude;
		totalAmplitude += currentAmplitude;
		currentAmplitude *= octavePersistence;
		currentPos *= octaveScale;
		currentPos.x += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		currentPos.y += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		currentPos.z += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		currentPos.w += OCTAVE_OFFSET; // Add "irrational" offset to de-align octave grids
		++seed; // Eliminates octaves "echoing" each other (since each octave is uniquely seeded)
	}

	// Re-normalize total noise to within [-1,1] and fix octaves pulling us far away from limits
	if ( renormalize && totalAmplitude > 0.f )
	{
		totalNoise /= totalAmplitude;				// Amplitude exceeds 1.0 if octaves are used
		totalNoise = ( totalNoise * 0.5f ) + 0.5f;	// Map to [0,1]
		totalNoise = SmoothStep( totalNoise );		// Push towards extents (octaves pull us away)
		totalNoise = ( totalNoise * 2.0f ) - 1.f;		// Map back to [-1,1]
	}

	return totalNoise;
}