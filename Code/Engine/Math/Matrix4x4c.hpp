#pragma once

#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
class Matrix4x4
{
public:
	Matrix4x4();
	Matrix4x4( Vector4 r1, Vector4 r2, Vector4 r3, Vector4 r4 );
	Matrix4x4( float arr[ 16 ] );
	void Transpose();
	void ChangeToProjectionMatrix( float xNearClip, float xFarClip, float yNearClip, float yFarClip, float zNearClip, float zFarClip );
	void ChangeToProjectionMatrix( float screenWidth, float screenHeight, float nearClip, float farClip );
	void ChangeToPerspectiveProjection( float yFovDegrees, float aspect, float nearClippingPLane, float farClippingPlane );
	void ChangeToTranslationMatrix( Vector3 position );
	void ChangeToLookAtMatrix( Vector3 posToLookAt, Vector3 currPos );
	void TransposeRotation();
	void MatrixMakeRotationEuler( float yaw, float pitch, float roll, Vector3 position );
	void SetBasis( Vector3 right, Vector3 up, Vector3 forward );
	void ChangePosition( Vector3 pos );
	Vector4 GetFirstRow() const;
	Vector4 GetSecondRow() const;
	Vector4 GetThirdRow() const;
	Vector4 GetFourthRow() const;
	Vector3 GetPosition() const;
	void NegateTranslation( Vector3 position );
	Matrix4x4& operator=( const Matrix4x4& other );
	bool operator==( const Matrix4x4& other );
	bool operator!=( const Matrix4x4& other );
	friend Matrix4x4 operator*( Matrix4x4 left, const Matrix4x4& right );

public:
	float matrix[ 16 ];

public:
	static const Matrix4x4 BASIS;
};


//-----------------------------------------------------------------------------------------------
inline Vector4 Matrix4x4::GetFirstRow() const
{
	return Vector4( matrix[ 0 ], matrix[ 1 ], matrix[ 2 ], matrix[ 3 ] );
}


//-----------------------------------------------------------------------------------------------
inline Vector4 Matrix4x4::GetSecondRow() const
{
	return Vector4( matrix[ 4 ], matrix[ 5 ], matrix[ 6 ], matrix[ 7 ] );
};


//-----------------------------------------------------------------------------------------------
inline Vector4 Matrix4x4::GetThirdRow() const
{
	return Vector4( matrix[ 8 ], matrix[ 9 ], matrix[ 10 ], matrix[ 11 ] );
}


//-----------------------------------------------------------------------------------------------
inline Vector4 Matrix4x4::GetFourthRow() const
{
	return Vector4( matrix[ 12 ], matrix[ 13 ], matrix[ 14 ], matrix[ 15 ] );
}


//-----------------------------------------------------------------------------------------------
inline Vector3 Matrix4x4::GetPosition() const
{
	return Vector3( matrix[ 12 ], matrix[ 13 ], matrix[ 14 ] );
}


//-----------------------------------------------------------------------------------------------
inline Matrix4x4& Matrix4x4::operator=( const Matrix4x4& other )
{
	matrix[ 0 ] = other.matrix[ 0 ];   matrix[ 1 ] = other.matrix[ 1 ];   matrix[ 2 ] = other.matrix[ 2 ];   matrix[ 3 ] = other.matrix[ 3 ];
	matrix[ 4 ] = other.matrix[ 4 ];   matrix[ 5 ] = other.matrix[ 5 ];   matrix[ 6 ] = other.matrix[ 6 ];   matrix[ 7 ] = other.matrix[ 7 ];
	matrix[ 8 ] = other.matrix[ 8 ];   matrix[ 9 ] = other.matrix[ 9 ];   matrix[ 10 ] = other.matrix[ 10 ]; matrix[ 11 ] = other.matrix[ 11 ];
	matrix[ 12 ] = other.matrix[ 12 ]; matrix[ 13 ] = other.matrix[ 13 ]; matrix[ 14 ] = other.matrix[ 14 ]; matrix[ 15 ] = other.matrix[ 15 ];

	return *this;
}


//-----------------------------------------------------------------------------------------------
inline bool Matrix4x4::operator==( const Matrix4x4& other )
{
	return matrix == other.matrix;
}


//-----------------------------------------------------------------------------------------------
inline bool Matrix4x4::operator!=( const Matrix4x4& other )
{
	return matrix != other.matrix;
}


//-----------------------------------------------------------------------------------------------
inline Matrix4x4 operator*( Matrix4x4 left, const Matrix4x4& right )
{
	Matrix4x4 temp;

	temp.matrix[ 0 ] = left.matrix[ 0 ] * right.matrix[ 0 ] + left.matrix[ 1 ] * right.matrix[ 4 ] + left.matrix[ 2 ] * right.matrix[ 8 ] + left.matrix[ 3 ] * right.matrix[ 12 ];
	temp.matrix[ 1 ] = left.matrix[ 0 ] * right.matrix[ 1 ] + left.matrix[ 1 ] * right.matrix[ 5 ] + left.matrix[ 2 ] * right.matrix[ 9 ] + left.matrix[ 3 ] * right.matrix[ 13 ];
	temp.matrix[ 2 ] = left.matrix[ 0 ] * right.matrix[ 2 ] + left.matrix[ 1 ] * right.matrix[ 6 ] + left.matrix[ 2 ] * right.matrix[ 10 ] + left.matrix[ 3 ] * right.matrix[ 14 ];
	temp.matrix[ 3 ] = left.matrix[ 0 ] * right.matrix[ 3 ] + left.matrix[ 1 ] * right.matrix[ 7 ] + left.matrix[ 2 ] * right.matrix[ 11 ] + left.matrix[ 3 ] * right.matrix[ 15 ];

	temp.matrix[ 4 ] = left.matrix[ 4 ] * right.matrix[ 0 ] + left.matrix[ 5 ] * right.matrix[ 4 ] + left.matrix[ 6 ] * right.matrix[ 8 ] + left.matrix[ 7 ] * right.matrix[ 12 ];
	temp.matrix[ 5 ] = left.matrix[ 4 ] * right.matrix[ 1 ] + left.matrix[ 5 ] * right.matrix[ 5 ] + left.matrix[ 6 ] * right.matrix[ 9 ] + left.matrix[ 7 ] * right.matrix[ 13 ];
	temp.matrix[ 6 ] = left.matrix[ 4 ] * right.matrix[ 2 ] + left.matrix[ 5 ] * right.matrix[ 6 ] + left.matrix[ 6 ] * right.matrix[ 10 ] + left.matrix[ 7 ] * right.matrix[ 14 ];
	temp.matrix[ 7 ] = left.matrix[ 4 ] * right.matrix[ 3 ] + left.matrix[ 5 ] * right.matrix[ 7 ] + left.matrix[ 6 ] * right.matrix[ 11 ] + left.matrix[ 7 ] * right.matrix[ 15 ];

	temp.matrix[ 8 ] = left.matrix[ 8 ] * right.matrix[ 0 ] + left.matrix[ 9 ] * right.matrix[ 4 ] + left.matrix[ 10 ] * right.matrix[ 8 ] + left.matrix[ 11 ] * right.matrix[ 12 ];
	temp.matrix[ 9 ] = left.matrix[ 8 ] * right.matrix[ 1 ] + left.matrix[ 9 ] * right.matrix[ 5 ] + left.matrix[ 10 ] * right.matrix[ 9 ] + left.matrix[ 11 ] * right.matrix[ 13 ];
	temp.matrix[ 10 ] = left.matrix[ 8 ] * right.matrix[ 2 ] + left.matrix[ 9 ] * right.matrix[ 6 ] + left.matrix[ 10 ] * right.matrix[ 10 ] + left.matrix[ 11 ] * right.matrix[ 14 ];
	temp.matrix[ 11 ] = left.matrix[ 8 ] * right.matrix[ 3 ] + left.matrix[ 9 ] * right.matrix[ 7 ] + left.matrix[ 10 ] * right.matrix[ 11 ] + left.matrix[ 11 ] * right.matrix[ 15 ];

	temp.matrix[ 12 ] = left.matrix[ 12 ] * right.matrix[ 0 ] + left.matrix[ 13 ] * right.matrix[ 4 ] + left.matrix[ 14 ] * right.matrix[ 8 ] + left.matrix[ 15 ] * right.matrix[ 12 ];
	temp.matrix[ 13 ] = left.matrix[ 12 ] * right.matrix[ 1 ] + left.matrix[ 13 ] * right.matrix[ 5 ] + left.matrix[ 14 ] * right.matrix[ 9 ] + left.matrix[ 15 ] * right.matrix[ 13 ];
	temp.matrix[ 14 ] = left.matrix[ 12 ] * right.matrix[ 2 ] + left.matrix[ 13 ] * right.matrix[ 6 ] + left.matrix[ 14 ] * right.matrix[ 10 ] + left.matrix[ 15 ] * right.matrix[ 14 ];
	temp.matrix[ 15 ] = left.matrix[ 12 ] * right.matrix[ 3 ] + left.matrix[ 13 ] * right.matrix[ 7 ] + left.matrix[ 14 ] * right.matrix[ 11 ] + left.matrix[ 15 ] * right.matrix[ 15 ];

	return temp;
}