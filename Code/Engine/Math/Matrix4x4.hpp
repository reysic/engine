#pragma once
#pragma warning( disable : 4201 )  // nonstandard extension used: nameless struct/union
#if !defined( __ITW_MATH_MATRIX4x4_FL__ )
#define __ITW_MATH_MATRIX4x4_FL__

#include <string>

#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/EulerAngles.hpp"


//-----------------------------------------------------------------------------------------------
// memory is with columns laid out contigously in memory;  I multiply on the left.

/* On paper, my matrixes look like;
x1, x2, x3, 0,
y1, y2, y3, 0,
z1, z2, z3, 0,
o1, o2, o3, 1

-- In memory, it's transposed

*/
struct mat44_fl
{
	union
	{
		struct { Vector4 col[ 4 ]; };
		float data[ 16 ];
	};

	// nothing - leave it uninitialized
	mat44_fl() {}
	mat44_fl( const mat44_fl& m )
	{
		data[ 0 ] = m.data[ 0 ];
		data[ 1 ] = m.data[ 1 ];
		data[ 2 ] = m.data[ 2 ];
		data[ 3 ] = m.data[ 3 ];

		data[ 4 ] = m.data[ 4 ];
		data[ 5 ] = m.data[ 5 ];
		data[ 6 ] = m.data[ 6 ];
		data[ 7 ] = m.data[ 7 ];

		data[ 8 ] = m.data[ 8 ];
		data[ 9 ] = m.data[ 9 ];
		data[ 10 ] = m.data[ 10 ];
		data[ 11 ] = m.data[ 11 ];

		data[ 12 ] = m.data[ 12 ];
		data[ 13 ] = m.data[ 13 ];
		data[ 14 ] = m.data[ 14 ];
		data[ 15 ] = m.data[ 15 ];
	}
	void SetData( mat44_fl const &a )
	{
		for ( int i = 0; i < 16; i++ )
			data[ i ] = a.data[ i ];
	}

	mat44_fl( float v00, float v01, float v02, float v03,
		float v10, float v11, float v12, float v13,
		float v20, float v21, float v22, float v23,
		float v30, float v31, float v32, float v33 )
	{
		data[ 0 ] = v00;
		data[ 1 ] = v01;
		data[ 2 ] = v02;
		data[ 3 ] = v03;

		data[ 4 ] = v10;
		data[ 5 ] = v11;
		data[ 6 ] = v12;
		data[ 7 ] = v13;

		data[ 8 ] = v20;
		data[ 9 ] = v21;
		data[ 10 ] = v22;
		data[ 11 ] = v23;

		data[ 12 ] = v30;
		data[ 13 ] = v31;
		data[ 14 ] = v32;
		data[ 15 ] = v33;
	}

	mat44_fl( float const *_data )
	{
		memcpy( data, _data, sizeof( data ) );
	}

	static mat44_fl identity;
	void MakeRotationEuler( EulerAngles const &orientation, Vector3 const &position );
	void MakeRotationEuler( float yawDegrees, float pitchDegrees, float rollDegrees, Vector3 const &position );
	void MatrixCOB();
	void InvertOrthonormal();
	inline mat44_fl operator=( mat44_fl const &a )
	{

		data[ 0 ] = a.data[ 0 ];
		data[ 1 ] = a.data[ 1 ];
		data[ 2 ] = a.data[ 2 ];
		data[ 3 ] = a.data[ 3 ];

		data[ 4 ] = a.data[ 4 ];
		data[ 5 ] = a.data[ 5 ];
		data[ 6 ] = a.data[ 6 ];
		data[ 7 ] = a.data[ 7 ];

		data[ 8 ] = a.data[ 8 ];
		data[ 9 ] = a.data[ 9 ];
		data[ 10 ] = a.data[ 10 ];
		data[ 11 ] = a.data[ 11 ];

		data[ 12 ] = a.data[ 12 ];
		data[ 13 ] = a.data[ 13 ];
		data[ 14 ] = a.data[ 14 ];
		data[ 15 ] = a.data[ 15 ];
		return *this;
	}
};


//-----------------------------------------------------------------------------------------------
void MatrixMakeIdentity( mat44_fl *mat );
void MatrixTranspose( mat44_fl *mat );
void MatrixNormalize( mat44_fl *m );


//-----------------------------------------------------------------------------------------------
float MatrixGetDeterminant( mat44_fl const *mat );


//-----------------------------------------------------------------------------------------------
bool MatrixIsOrthogonal( mat44_fl const *m );
void MatrixInvertOrthogonal( mat44_fl *mat );
void MatrixInvert( mat44_fl *mat );


//-----------------------------------------------------------------------------------------------
void MatrixMultiply( mat44_fl *out, mat44_fl const *a, mat44_fl const *b );
Vector3 MatrixTransform( mat44_fl const *m, Vector3 const &v );
Vector4 MatrixTransform( mat44_fl const *m, Vector4 const &v );


//-----------------------------------------------------------------------------------------------
void MatrixGetBasis( mat44_fl const *mat, Vector3 *r, Vector3 *u, Vector3 *f, Vector3 *o );
void MatrixSetBasis( mat44_fl *mat, Vector3 const &r, Vector3 const &u, Vector3 const &f, Vector3 const &o );


//-----------------------------------------------------------------------------------------------
void MatrixGenerateBasis( mat44_fl *outm, Vector3 const &forward );


//-----------------------------------------------------------------------------------------------
Vector3 MatrixGetRight( mat44_fl const *mat );
Vector3 MatrixGetUp( mat44_fl const *mat );
Vector3 MatrixGetForward( mat44_fl const *mat );
void MatrixSetForward( mat44_fl *mat, Vector3 const &forward );


//-----------------------------------------------------------------------------------------------
void MatrixGetRow( mat44_fl const *m, int row, Vector4 *out );
void MatrixSetRow( mat44_fl *m, int row, Vector4 const &v );
void MatrixGetRows( mat44_fl const *mat, Vector4 *x, Vector4 *y, Vector4 *z, Vector4 *o );
void MatrixSetRows( mat44_fl *mat, Vector4 const &x, Vector4 const &y, Vector4 const &z, Vector4 const &o );


//-----------------------------------------------------------------------------------------------
void MatrixTransposeRotation( mat44_fl* mat );
Vector3 MatrixGetOffset( mat44_fl const *m );
void MatrixSetOffset( mat44_fl *m, Vector3 const &o );


//-----------------------------------------------------------------------------------------------
void MatrixMakeScale( mat44_fl *mat, float c );
void MatrixMakeTranslation( mat44_fl *mat, Vector3 const &offset );


//-----------------------------------------------------------------------------------------------
void MatrixMakeRotationAroundX( mat44_fl *mat, float const rad );
void MatrixMakeRotationAroundY( mat44_fl *mat, float const rad );
void MatrixMakeRotationAroundZ( mat44_fl *mat, float const rad );


//-----------------------------------------------------------------------------------------------
void MatrixMakeLookTo( mat44_fl *mat, Vector3 const &pos, Vector3 const &dir, Vector3 const &up );
void MatrixMakeLookAt( mat44_fl *mat, Vector3 const &pos, Vector3 const &lookat, Vector3 const &up );


//-----------------------------------------------------------------------------------------------
void MatrixMakeProjOrthogonal( mat44_fl *mat, float nx, float fx, float ny, float fy, float nz, float fz );
void MatrixMakeProjOrthogonal( mat44_fl *mat, float width, float height, float nz, float fz );
void MatrixMakeProjPerspective( mat44_fl *mat, float ratio, float rads, float near_z, float far_z );


//-----------------------------------------------------------------------------------------------
void MatrixMakePerspective( mat44_fl* mat, float fov_degrees, float aspect, float nz, float fz );
void MatrixTranslate( mat44_fl *mat, Vector3 const &o );


//-----------------------------------------------------------------------------------------------
void EulerRight( Vector3 *v, float const yaw, float const pitch, float const roll );
void EulerUp( Vector3 *v, float const yaw, float const pitch, float const roll );
void EulerForward( Vector3 *v, float const yaw, float const pitch, float const roll );


//-----------------------------------------------------------------------------------------------
inline mat44_fl operator*( mat44_fl const &a, mat44_fl const &b )
{
	mat44_fl ret;
	MatrixMultiply( &ret, &a, &b );
	return ret;
}


//-----------------------------------------------------------------------------------------------
// We only allow multiplication on the left - since it's the only way it makes sense
inline Vector4 operator*( Vector4 const &v, mat44_fl const &a )
{
	return MatrixTransform( &a, v );
}


//-----------------------------------------------------------------------------------------------
inline Vector3 operator*( Vector3 const &v, mat44_fl const &a )
{
	Vector4 temp = Vector4( v.x, v.y, v.z, 0.f );
	temp = MatrixTransform( &a, temp );
	return Vector3( temp.x, temp.y, temp.z );
}


#endif 