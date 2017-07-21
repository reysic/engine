#include "Engine/Math/Matrix4x4c.hpp"

#include <string>


//-----------------------------------------------------------------------------------------------
#define STATIC // Do-nothing indicator that method/member is static in class definition


//-----------------------------------------------------------------------------------------------
STATIC const Matrix4x4 Matrix4x4::BASIS( Vector4( 1.0f, 0.0f, 0.0f, 0.0f ),
	Vector4( 0.0f, 0.0f, 1.0f, 0.0f ),
	Vector4( 0.0f, 1.0f, 0.0f, 0.0f ),
	Vector4( 0.0f, 0.0f, 0.0f, 1.0f ) );


//-----------------------------------------------------------------------------------------------
Matrix4x4::Matrix4x4()
{
	matrix[ 0 ] = 1.0f;    matrix[ 1 ] = 0.0f;    matrix[ 2 ] = 0.0f;    matrix[ 3 ] = 0.0f;
	matrix[ 4 ] = 0.0f;    matrix[ 5 ] = 1.0f;    matrix[ 6 ] = 0.0f;    matrix[ 7 ] = 0.0f;
	matrix[ 8 ] = 0.0f;    matrix[ 9 ] = 0.0f;    matrix[ 10 ] = 1.0f;   matrix[ 11 ] = 0.0f;
	matrix[ 12 ] = 0.0f;   matrix[ 13 ] = 0.0f;   matrix[ 14 ] = 0.0f;   matrix[ 15 ] = 1.0f;
}


//-----------------------------------------------------------------------------------------------
Matrix4x4::Matrix4x4( Vector4 r1, Vector4 r2, Vector4 r3, Vector4 r4 )
{
	matrix[ 0 ] = r1.x;    matrix[ 1 ] = r1.y;    matrix[ 2 ] = r1.z;   matrix[ 3 ] = r1.w;
	matrix[ 4 ] = r2.x;    matrix[ 5 ] = r2.y;    matrix[ 6 ] = r2.z;   matrix[ 7 ] = r2.w;
	matrix[ 8 ] = r3.x;    matrix[ 9 ] = r3.y;    matrix[ 10 ] = r3.z;  matrix[ 11 ] = r3.w;
	matrix[ 12 ] = r4.x;   matrix[ 13 ] = r4.y;   matrix[ 14 ] = r4.z;  matrix[ 15 ] = r4.w;
}

//-----------------------------------------------------------------------------------------------
Matrix4x4::Matrix4x4( float arr[ 16 ] )
{
	matrix[ 0 ] = arr[ 0 ];    matrix[ 1 ] = arr[ 1 ];    matrix[ 2 ] = arr[ 2 ];    matrix[ 3 ] = arr[ 3 ];
	matrix[ 4 ] = arr[ 4 ];    matrix[ 5 ] = arr[ 5 ];    matrix[ 6 ] = arr[ 6 ];    matrix[ 7 ] = arr[ 7 ];
	matrix[ 8 ] = arr[ 8 ];    matrix[ 9 ] = arr[ 9 ];    matrix[ 10 ] = arr[ 10 ];  matrix[ 11 ] = arr[ 11 ];
	matrix[ 12 ] = arr[ 12 ];  matrix[ 13 ] = arr[ 13 ];  matrix[ 14 ] = arr[ 14 ];  matrix[ 15 ] = arr[ 15 ];
}


//-----------------------------------------------------------------------------------------------
void Matrix4x4::Transpose()
{
	std::swap( matrix[ 1 ], matrix[ 4 ] );
	std::swap( matrix[ 2 ], matrix[ 8 ] );
	std::swap( matrix[ 3 ], matrix[ 12 ] );
	std::swap( matrix[ 6 ], matrix[ 9 ] );
	std::swap( matrix[ 7 ], matrix[ 13 ] );
	std::swap( matrix[ 11 ], matrix[ 14 ] );
}

//-----------------------------------------------------------------------------------------------
void Matrix4x4::ChangeToProjectionMatrix( float xNearClip, float xFarClip, float yNearClip, float yFarClip, float zNearClip, float zFarClip )
{
	float sx = 1.0f / ( xFarClip - xNearClip );
	float sy = 1.0f / ( yFarClip - yNearClip );
	float sz = 1.0f / ( zFarClip - zNearClip );

	matrix[ 0 ] = 2.0f * sx;   matrix[ 1 ] = 0.0f;       matrix[ 2 ] = 0.0f;        matrix[ 3 ] = -( xFarClip + xNearClip ) * sx;
	matrix[ 4 ] = 0.0f;        matrix[ 5 ] = 2.0f * sy;  matrix[ 6 ] = 0.0f;        matrix[ 7 ] = -( yFarClip + yNearClip ) * sy;
	matrix[ 8 ] = 0.0f;        matrix[ 9 ] = 0.0f;       matrix[ 10 ] = 2.0f * sz;  matrix[ 11 ] = -( zFarClip + zNearClip ) * sz;
	matrix[ 12 ] = 0.0f;       matrix[ 13 ] = 0.0f;      matrix[ 14 ] = 0.0f;       matrix[ 15 ] = 1.0f;

}


//-----------------------------------------------------------------------------------------------
void Matrix4x4::ChangeToProjectionMatrix( float screenWidth, float screenHeight, float nearClip, float farClip )
{
	float sz = 1.0f / ( farClip - nearClip );

	matrix[ 0 ] = 2.0f / screenWidth;  matrix[ 1 ] = 0.0f;                matrix[ 2 ] = 0.0f;        matrix[ 3 ] = 0.0f;
	matrix[ 4 ] = 0.0f;                matrix[ 5 ] = 2.0f / screenHeight; matrix[ 6 ] = 0.0f;        matrix[ 7 ] = 0.0f;
	matrix[ 8 ] = 0.0f;                matrix[ 9 ] = 0.0f;                matrix[ 10 ] = 2.0f * sz;  matrix[ 11 ] = -( farClip + nearClip ) * sz;
	matrix[ 12 ] = 0.0f;               matrix[ 13 ] = 0.0f;               matrix[ 14 ] = 0.0f;       matrix[ 15 ] = 1.0f;
}


//-----------------------------------------------------------------------------------------------
void Matrix4x4::ChangeToPerspectiveProjection( float yFovDegrees, float aspect, float nearClippingPlane, float farClippingPlane )
{
	float rads = ConvertDegreesToRadians( yFovDegrees );
	float size = 1.0f / tan( rads / 2.0f );

	float w = size;
	float h = size;

	if ( aspect > 1.0f )
	{
		w /= aspect;
	}
	else
	{
		h *= aspect;
	}

	float q = 1.0f / ( farClippingPlane - nearClippingPlane );
	float val1 = ( farClippingPlane + nearClippingPlane ) * q;
	float val2 = -2.f * nearClippingPlane * farClippingPlane * q;

	matrix[ 0 ] = w;			matrix[ 1 ] = 0.0f;             matrix[ 2 ] = 0.0f;             matrix[ 3 ] = 0.0f;
	matrix[ 4 ] = 0.0f;         matrix[ 5 ] = h;				matrix[ 6 ] = 0.0f;             matrix[ 7 ] = 0.0f;
	matrix[ 8 ] = 0.0f;         matrix[ 9 ] = 0.0f;             matrix[ 10 ] = val1;			matrix[ 11 ] = 1.0f;
	matrix[ 12 ] = 0.0f;        matrix[ 13 ] = 0.0f;            matrix[ 14 ] = val2;			matrix[ 15 ] = 0.0f;
}


//-----------------------------------------------------------------------------------------------
void Matrix4x4::ChangeToTranslationMatrix( Vector3 position )
{
	matrix[ 12 ] = position.x;
	matrix[ 13 ] = position.y;
	matrix[ 14 ] = position.z;
}


//-----------------------------------------------------------------------------------------------
void Matrix4x4::ChangeToLookAtMatrix( Vector3 posToLookAt, Vector3 currPos )
{
	Vector3 dir = posToLookAt - currPos;
	dir.Normalize();

	Vector3 right = CrossProductVector3( dir, Vector3::UP );
	Vector3 up = CrossProductVector3( right, dir );
	Vector3 forward = dir;

	matrix[ 0 ] = right.x;        matrix[ 1 ] = right.y;     matrix[ 2 ] = right.z;      matrix[ 3 ] = 0.0f;
	matrix[ 4 ] = forward.x;      matrix[ 5 ] = forward.y;   matrix[ 6 ] = forward.z;    matrix[ 7 ] = 0.0f;
	matrix[ 8 ] = up.x;           matrix[ 9 ] = up.y;        matrix[ 10 ] = up.z;        matrix[ 11 ] = 0.0f;

}


//-----------------------------------------------------------------------------------------------
void Matrix4x4::TransposeRotation()
{
	std::swap( matrix[ 1 ], matrix[ 4 ] );
	std::swap( matrix[ 2 ], matrix[ 8 ] );
	std::swap( matrix[ 6 ], matrix[ 9 ] );
}


//-----------------------------------------------------------------------------------------------
void Matrix4x4::MatrixMakeRotationEuler( float yaw, float pitch, float roll, Vector3 position )
{
	position;

	float sx = SinDegrees( pitch );
	float cx = CosDegrees( pitch );

	float sy = SinDegrees( roll );
	float cy = CosDegrees( roll );

	float sz = SinDegrees( yaw );
	float cz = CosDegrees( yaw );


	matrix[ 0 ] = cy * cz;
	matrix[ 1 ] = cz * sx * sy - cx * sz;
	matrix[ 2 ] = cx * cz * sy + sx * sz;
	matrix[ 3 ] = 0.0f;

	matrix[ 4 ] = cy * sz;
	matrix[ 5 ] = cx * cz + sx * sy * sz;
	matrix[ 6 ] = -cz * sx + cx * sy * sz;
	matrix[ 7 ] = 0.0f;

	matrix[ 8 ] = -sy;
	matrix[ 9 ] = cy * sx;
	matrix[ 10 ] = cx * cy;
	matrix[ 11 ] = 0.0f;

	matrix[ 12 ] = 0.0f;
	matrix[ 13 ] = 0.0f;
	matrix[ 14 ] = 0.0f;
	matrix[ 15 ] = 1.0f;

	Transpose();
}


//-----------------------------------------------------------------------------------------------
void Matrix4x4::SetBasis( Vector3 right, Vector3 up, Vector3 forward )
{
	matrix[ 0 ] = right.x;     matrix[ 1 ] = right.y;     matrix[ 2 ] = right.z;      matrix[ 3 ] = 0.f;
	matrix[ 4 ] = up.x;        matrix[ 5 ] = up.y;        matrix[ 6 ] = up.z;         matrix[ 7 ] = 0.f;
	matrix[ 8 ] = forward.x;   matrix[ 9 ] = forward.y;   matrix[ 10 ] = forward.z;   matrix[ 11 ] = 0.f;
}


//-----------------------------------------------------------------------------------------------
void Matrix4x4::ChangePosition( Vector3 pos )
{
	matrix[ 12 ] = pos.x;
	matrix[ 13 ] = pos.y;
	matrix[ 14 ] = pos.z;
}


//-----------------------------------------------------------------------------------------------
void Matrix4x4::NegateTranslation( Vector3 position )
{
	Vector3 nPos = position * -1.0f;

	Vector3 col1 = Vector3( matrix[ 0 ], matrix[ 4 ], matrix[ 8 ] );
	Vector3 col2 = Vector3( matrix[ 1 ], matrix[ 5 ], matrix[ 9 ] );
	Vector3 col3 = Vector3( matrix[ 2 ], matrix[ 6 ], matrix[ 10 ] );
	Vector3 col4 = Vector3( matrix[ 3 ], matrix[ 7 ], matrix[ 11 ] );

	Vector3 translatedPos = Vector3( DotProductVector3( nPos, col1 ), DotProductVector3( nPos, col2 ), DotProductVector3( nPos, col3 ) );

	matrix[ 12 ] = translatedPos.x;
	matrix[ 13 ] = translatedPos.y;
	matrix[ 14 ] = translatedPos.z;
}