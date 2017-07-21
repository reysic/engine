#include "Engine/Math/Matrix4x4.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
mat44_fl mat44_fl::identity( 1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f );


//-----------------------------------------------------------------------------------------------
Vector3 MatrixTransform( mat44_fl const *m, Vector3 const &v )
{
	Vector4 v4 = Vector4( v, 1.0f );
	MatrixTransform( m, v4 );

	Vector3 ret = Vector3( v4.x, v4.y, v4.z );
	return ret;
}


//-----------------------------------------------------------------------------------------------
Vector4 MatrixTransform( mat44_fl const *m, Vector4 const &v )
{
	m->col[ 0 ];
	Vector4 ret = Vector4(
		DotProductVector4( m->col[ 0 ], v ),
		DotProductVector4( m->col[ 1 ], v ),
		DotProductVector4( m->col[ 2 ], v ),
		DotProductVector4( m->col[ 3 ], v ) );

	return ret;
}


//-----------------------------------------------------------------------------------------------
void MatrixNormalize( mat44_fl *m )
{
	Vector3 f, r, u, o;

	MatrixGetBasis( m, &r, &u, &f, &o );

	f.Normalize();
	u.Normalize();

	r = CrossProductVector3( u, f );
	u = CrossProductVector3( f, r );

	MatrixSetBasis( m, r, u, f, o );
}


//-----------------------------------------------------------------------------------------------
// Is the rotational part orthogonal? (Ignores the translation)
bool MatrixIsOrthogonal( mat44_fl const *m )
{
	Vector4 col0 = m->col[ 0 ];
	Vector4 col1 = m->col[ 1 ];
	Vector4 col2 = m->col[ 2 ];

	col0.w = 0.0f;
	col1.w = 0.0f;
	col2.w = 0.0f;

	return ( ( DotProductVector4( col0, col1 ) == 0.0f )
		&& ( DotProductVector4( col0, col2 ) == 0.0f )
		&& ( DotProductVector4( col1, col2 ) == 0.0f ) );
}


//-----------------------------------------------------------------------------------------------
// Lifted from GLU
float MatrixGetDeterminant( mat44_fl const *mat )
{
	float inv[ 4 ];

	float det;
	float const *m = mat->data;

	inv[ 0 ] = m[ 5 ] * m[ 10 ] * m[ 15 ] -
		m[ 5 ] * m[ 11 ] * m[ 14 ] -
		m[ 9 ] * m[ 6 ] * m[ 15 ] +
		m[ 9 ] * m[ 7 ] * m[ 14 ] +
		m[ 13 ] * m[ 6 ] * m[ 11 ] -
		m[ 13 ] * m[ 7 ] * m[ 10 ];

	inv[ 1 ] = -m[ 4 ] * m[ 10 ] * m[ 15 ] +
		m[ 4 ] * m[ 11 ] * m[ 14 ] +
		m[ 8 ] * m[ 6 ] * m[ 15 ] -
		m[ 8 ] * m[ 7 ] * m[ 14 ] -
		m[ 12 ] * m[ 6 ] * m[ 11 ] +
		m[ 12 ] * m[ 7 ] * m[ 10 ];

	inv[ 2 ] = m[ 4 ] * m[ 9 ] * m[ 15 ] -
		m[ 4 ] * m[ 11 ] * m[ 13 ] -
		m[ 8 ] * m[ 5 ] * m[ 15 ] +
		m[ 8 ] * m[ 7 ] * m[ 13 ] +
		m[ 12 ] * m[ 5 ] * m[ 11 ] -
		m[ 12 ] * m[ 7 ] * m[ 9 ];

	inv[ 3 ] = -m[ 4 ] * m[ 9 ] * m[ 14 ] +
		m[ 4 ] * m[ 10 ] * m[ 13 ] +
		m[ 8 ] * m[ 5 ] * m[ 14 ] -
		m[ 8 ] * m[ 6 ] * m[ 13 ] -
		m[ 12 ] * m[ 5 ] * m[ 10 ] +
		m[ 12 ] * m[ 6 ] * m[ 9 ];


	det = m[ 0 ] * inv[ 0 ] + m[ 1 ] * inv[ 1 ] + m[ 2 ] * inv[ 2 ] + m[ 3 ] * inv[ 3 ];
	return det;
}


//------------------------------------------------------------------------
void MatrixGetColumn( mat44_fl const *m, int col, Vector4 *out )
{
	*out = m->col[ col ];
}


//------------------------------------------------------------------------
void MatrixSetColumn( mat44_fl *m, int col, Vector4 const &v )
{
	m->col[ col ] = v;
}


//------------------------------------------------------------------------
void MatrixSetColumns( mat44_fl *m, Vector4 const &c0, Vector4 const &c1, Vector4 const &c2, Vector4 const &c3 )
{
	m->col[ 0 ] = c0;
	m->col[ 1 ] = c1;
	m->col[ 2 ] = c2;
	m->col[ 3 ] = c3;
}


//------------------------------------------------------------------------
void MatrixGetRow( mat44_fl const *m, int row, Vector4 *out )
{
	*out = Vector4( m->data[ row + 0 ],
		m->data[ row + 4 ],
		m->data[ row + 8 ],
		m->data[ row + 12 ] );
}


//------------------------------------------------------------------------
void MatrixSetRow( mat44_fl *m, int row, Vector4 const &v )
{
	m->data[ row + 0 ] = v.x;
	m->data[ row + 4 ] = v.y;
	m->data[ row + 8 ] = v.z;
	m->data[ row + 12 ] = v.w;
}


//------------------------------------------------------------------------
Vector3 MatrixGetOffset( mat44_fl const *m )
{
	return Vector3( m->data[ 12 ],
		m->data[ 13 ],
		m->data[ 14 ] );
}


//------------------------------------------------------------------------
void MatrixSetOffset( mat44_fl *m, Vector3 const &o )
{
	m->data[ 12 ] = o.x;
	m->data[ 13 ] = o.y;
	m->data[ 14 ] = o.z;
}


//-----------------------------------------------------------------------------------------------
// Only works on orthogonal matrices
void MatrixInvertOrthogonal( mat44_fl *mat )
{
	// ASSERT( MatrixIsOrthogonal(mat) );

	Vector3 translation = MatrixGetOffset( mat );
	MatrixTranspose( mat );
	MatrixSetColumn( mat, 3, Vector4( 0.0f, 0.0f, 0.0f, 1.0f ) );

	translation = -translation * ( *mat );
	MatrixSetOffset( mat, translation );
}


//-----------------------------------------------------------------------------------------------
// Lifted from GLU
void MatrixInvert( mat44_fl *mat )
{
	float inv[ 16 ];
	float det;
	float *const m = mat->data;
	int i;

	inv[ 0 ] = m[ 5 ] * m[ 10 ] * m[ 15 ] -
		m[ 5 ] * m[ 11 ] * m[ 14 ] -
		m[ 9 ] * m[ 6 ] * m[ 15 ] +
		m[ 9 ] * m[ 7 ] * m[ 14 ] +
		m[ 13 ] * m[ 6 ] * m[ 11 ] -
		m[ 13 ] * m[ 7 ] * m[ 10 ];

	inv[ 4 ] = -m[ 4 ] * m[ 10 ] * m[ 15 ] +
		m[ 4 ] * m[ 11 ] * m[ 14 ] +
		m[ 8 ] * m[ 6 ] * m[ 15 ] -
		m[ 8 ] * m[ 7 ] * m[ 14 ] -
		m[ 12 ] * m[ 6 ] * m[ 11 ] +
		m[ 12 ] * m[ 7 ] * m[ 10 ];

	inv[ 8 ] = m[ 4 ] * m[ 9 ] * m[ 15 ] -
		m[ 4 ] * m[ 11 ] * m[ 13 ] -
		m[ 8 ] * m[ 5 ] * m[ 15 ] +
		m[ 8 ] * m[ 7 ] * m[ 13 ] +
		m[ 12 ] * m[ 5 ] * m[ 11 ] -
		m[ 12 ] * m[ 7 ] * m[ 9 ];

	inv[ 12 ] = -m[ 4 ] * m[ 9 ] * m[ 14 ] +
		m[ 4 ] * m[ 10 ] * m[ 13 ] +
		m[ 8 ] * m[ 5 ] * m[ 14 ] -
		m[ 8 ] * m[ 6 ] * m[ 13 ] -
		m[ 12 ] * m[ 5 ] * m[ 10 ] +
		m[ 12 ] * m[ 6 ] * m[ 9 ];

	inv[ 1 ] = -m[ 1 ] * m[ 10 ] * m[ 15 ] +
		m[ 1 ] * m[ 11 ] * m[ 14 ] +
		m[ 9 ] * m[ 2 ] * m[ 15 ] -
		m[ 9 ] * m[ 3 ] * m[ 14 ] -
		m[ 13 ] * m[ 2 ] * m[ 11 ] +
		m[ 13 ] * m[ 3 ] * m[ 10 ];

	inv[ 5 ] = m[ 0 ] * m[ 10 ] * m[ 15 ] -
		m[ 0 ] * m[ 11 ] * m[ 14 ] -
		m[ 8 ] * m[ 2 ] * m[ 15 ] +
		m[ 8 ] * m[ 3 ] * m[ 14 ] +
		m[ 12 ] * m[ 2 ] * m[ 11 ] -
		m[ 12 ] * m[ 3 ] * m[ 10 ];

	inv[ 9 ] = -m[ 0 ] * m[ 9 ] * m[ 15 ] +
		m[ 0 ] * m[ 11 ] * m[ 13 ] +
		m[ 8 ] * m[ 1 ] * m[ 15 ] -
		m[ 8 ] * m[ 3 ] * m[ 13 ] -
		m[ 12 ] * m[ 1 ] * m[ 11 ] +
		m[ 12 ] * m[ 3 ] * m[ 9 ];

	inv[ 13 ] = m[ 0 ] * m[ 9 ] * m[ 14 ] -
		m[ 0 ] * m[ 10 ] * m[ 13 ] -
		m[ 8 ] * m[ 1 ] * m[ 14 ] +
		m[ 8 ] * m[ 2 ] * m[ 13 ] +
		m[ 12 ] * m[ 1 ] * m[ 10 ] -
		m[ 12 ] * m[ 2 ] * m[ 9 ];

	inv[ 2 ] = m[ 1 ] * m[ 6 ] * m[ 15 ] -
		m[ 1 ] * m[ 7 ] * m[ 14 ] -
		m[ 5 ] * m[ 2 ] * m[ 15 ] +
		m[ 5 ] * m[ 3 ] * m[ 14 ] +
		m[ 13 ] * m[ 2 ] * m[ 7 ] -
		m[ 13 ] * m[ 3 ] * m[ 6 ];

	inv[ 6 ] = -m[ 0 ] * m[ 6 ] * m[ 15 ] +
		m[ 0 ] * m[ 7 ] * m[ 14 ] +
		m[ 4 ] * m[ 2 ] * m[ 15 ] -
		m[ 4 ] * m[ 3 ] * m[ 14 ] -
		m[ 12 ] * m[ 2 ] * m[ 7 ] +
		m[ 12 ] * m[ 3 ] * m[ 6 ];

	inv[ 10 ] = m[ 0 ] * m[ 5 ] * m[ 15 ] -
		m[ 0 ] * m[ 7 ] * m[ 13 ] -
		m[ 4 ] * m[ 1 ] * m[ 15 ] +
		m[ 4 ] * m[ 3 ] * m[ 13 ] +
		m[ 12 ] * m[ 1 ] * m[ 7 ] -
		m[ 12 ] * m[ 3 ] * m[ 5 ];

	inv[ 14 ] = -m[ 0 ] * m[ 5 ] * m[ 14 ] +
		m[ 0 ] * m[ 6 ] * m[ 13 ] +
		m[ 4 ] * m[ 1 ] * m[ 14 ] -
		m[ 4 ] * m[ 2 ] * m[ 13 ] -
		m[ 12 ] * m[ 1 ] * m[ 6 ] +
		m[ 12 ] * m[ 2 ] * m[ 5 ];

	inv[ 3 ] = -m[ 1 ] * m[ 6 ] * m[ 11 ] +
		m[ 1 ] * m[ 7 ] * m[ 10 ] +
		m[ 5 ] * m[ 2 ] * m[ 11 ] -
		m[ 5 ] * m[ 3 ] * m[ 10 ] -
		m[ 9 ] * m[ 2 ] * m[ 7 ] +
		m[ 9 ] * m[ 3 ] * m[ 6 ];

	inv[ 7 ] = m[ 0 ] * m[ 6 ] * m[ 11 ] -
		m[ 0 ] * m[ 7 ] * m[ 10 ] -
		m[ 4 ] * m[ 2 ] * m[ 11 ] +
		m[ 4 ] * m[ 3 ] * m[ 10 ] +
		m[ 8 ] * m[ 2 ] * m[ 7 ] -
		m[ 8 ] * m[ 3 ] * m[ 6 ];

	inv[ 11 ] = -m[ 0 ] * m[ 5 ] * m[ 11 ] +
		m[ 0 ] * m[ 7 ] * m[ 9 ] +
		m[ 4 ] * m[ 1 ] * m[ 11 ] -
		m[ 4 ] * m[ 3 ] * m[ 9 ] -
		m[ 8 ] * m[ 1 ] * m[ 7 ] +
		m[ 8 ] * m[ 3 ] * m[ 5 ];

	inv[ 15 ] = m[ 0 ] * m[ 5 ] * m[ 10 ] -
		m[ 0 ] * m[ 6 ] * m[ 9 ] -
		m[ 4 ] * m[ 1 ] * m[ 10 ] +
		m[ 4 ] * m[ 2 ] * m[ 9 ] +
		m[ 8 ] * m[ 1 ] * m[ 6 ] -
		m[ 8 ] * m[ 2 ] * m[ 5 ];

	det = m[ 0 ] * inv[ 0 ] + m[ 1 ] * inv[ 4 ] + m[ 2 ] * inv[ 8 ] + m[ 3 ] * inv[ 12 ];

	//ASSERT_RETURN(!FlEqual(det, 0.0f));  // Not invertible

	det = 1.0f / det;

	for ( i = 0; i < 16; i++ ) {
		m[ i ] = det * inv[ i ];
	}
}


//-----------------------------------------------------------------------------------------------
void MatrixTranspose( mat44_fl *mat )
{
	//temp = y
	float item0 = mat->data[ 1 ];
	float item1 = mat->data[ 2 ];
	float item2 = mat->data[ 3 ];
	float item3 = mat->data[ 6 ];
	float item4 = mat->data[ 7 ];
	float item5 = mat->data[ 11 ];

	//y = z
	mat->data[ 1 ] = mat->data[ 4 ];
	mat->data[ 2 ] = mat->data[ 8 ];
	mat->data[ 3 ] = mat->data[ 12 ];
	mat->data[ 6 ] = mat->data[ 9 ];
	mat->data[ 7 ] = mat->data[ 13 ];
	mat->data[ 11 ] = mat->data[ 14 ];

	//z = temp
	mat->data[ 4 ] = item0;
	mat->data[ 8 ] = item1;
	mat->data[ 12 ] = item2;
	mat->data[ 9 ] = item3;
	mat->data[ 13 ] = item4;
	mat->data[ 14 ] = item5;
}


//------------------------------------------------------------------------
void MatrixMultiply( mat44_fl *out, mat44_fl const *lhs, mat44_fl const *rhs )
{
	float result[ 16 ];
	result[ 0 ] = lhs->data[ 0 ] * rhs->data[ 0 ] + lhs->data[ 1 ] * rhs->data[ 4 ] + lhs->data[ 2 ] * rhs->data[ 8 ] + lhs->data[ 3 ] * rhs->data[ 12 ];
	result[ 1 ] = lhs->data[ 0 ] * rhs->data[ 1 ] + lhs->data[ 1 ] * rhs->data[ 5 ] + lhs->data[ 2 ] * rhs->data[ 9 ] + lhs->data[ 3 ] * rhs->data[ 13 ];
	result[ 2 ] = lhs->data[ 0 ] * rhs->data[ 2 ] + lhs->data[ 1 ] * rhs->data[ 6 ] + lhs->data[ 2 ] * rhs->data[ 10 ] + lhs->data[ 3 ] * rhs->data[ 14 ];
	result[ 3 ] = lhs->data[ 0 ] * rhs->data[ 3 ] + lhs->data[ 1 ] * rhs->data[ 7 ] + lhs->data[ 2 ] * rhs->data[ 11 ] + lhs->data[ 3 ] * rhs->data[ 15 ];

	result[ 4 ] = lhs->data[ 4 ] * rhs->data[ 0 ] + lhs->data[ 5 ] * rhs->data[ 4 ] + lhs->data[ 6 ] * rhs->data[ 8 ] + lhs->data[ 7 ] * rhs->data[ 12 ];
	result[ 5 ] = lhs->data[ 4 ] * rhs->data[ 1 ] + lhs->data[ 5 ] * rhs->data[ 5 ] + lhs->data[ 6 ] * rhs->data[ 9 ] + lhs->data[ 7 ] * rhs->data[ 13 ];
	result[ 6 ] = lhs->data[ 4 ] * rhs->data[ 2 ] + lhs->data[ 5 ] * rhs->data[ 6 ] + lhs->data[ 6 ] * rhs->data[ 10 ] + lhs->data[ 7 ] * rhs->data[ 14 ];
	result[ 7 ] = lhs->data[ 4 ] * rhs->data[ 3 ] + lhs->data[ 5 ] * rhs->data[ 7 ] + lhs->data[ 6 ] * rhs->data[ 11 ] + lhs->data[ 7 ] * rhs->data[ 15 ];

	result[ 8 ] = lhs->data[ 8 ] * rhs->data[ 0 ] + lhs->data[ 9 ] * rhs->data[ 4 ] + lhs->data[ 10 ] * rhs->data[ 8 ] + lhs->data[ 11 ] * rhs->data[ 12 ];
	result[ 9 ] = lhs->data[ 8 ] * rhs->data[ 1 ] + lhs->data[ 9 ] * rhs->data[ 5 ] + lhs->data[ 10 ] * rhs->data[ 9 ] + lhs->data[ 11 ] * rhs->data[ 13 ];
	result[ 10 ] = lhs->data[ 8 ] * rhs->data[ 2 ] + lhs->data[ 9 ] * rhs->data[ 6 ] + lhs->data[ 10 ] * rhs->data[ 10 ] + lhs->data[ 11 ] * rhs->data[ 14 ];
	result[ 11 ] = lhs->data[ 8 ] * rhs->data[ 3 ] + lhs->data[ 9 ] * rhs->data[ 7 ] + lhs->data[ 10 ] * rhs->data[ 11 ] + lhs->data[ 11 ] * rhs->data[ 15 ];

	result[ 12 ] = lhs->data[ 12 ] * rhs->data[ 0 ] + lhs->data[ 13 ] * rhs->data[ 4 ] + lhs->data[ 14 ] * rhs->data[ 8 ] + lhs->data[ 15 ] * rhs->data[ 12 ];
	result[ 13 ] = lhs->data[ 12 ] * rhs->data[ 1 ] + lhs->data[ 13 ] * rhs->data[ 5 ] + lhs->data[ 14 ] * rhs->data[ 9 ] + lhs->data[ 15 ] * rhs->data[ 13 ];
	result[ 14 ] = lhs->data[ 12 ] * rhs->data[ 2 ] + lhs->data[ 13 ] * rhs->data[ 6 ] + lhs->data[ 14 ] * rhs->data[ 10 ] + lhs->data[ 15 ] * rhs->data[ 14 ];
	result[ 15 ] = lhs->data[ 12 ] * rhs->data[ 3 ] + lhs->data[ 13 ] * rhs->data[ 7 ] + lhs->data[ 14 ] * rhs->data[ 11 ] + lhs->data[ 15 ] * rhs->data[ 15 ];

	memcpy( out->data, result, sizeof( result ) );
}


//------------------------------------------------------------------------
void MatrixMakeIdentity( mat44_fl *mat )
{
	float const values[] = {
		1.0f,	0.0f, 0.0f, 0.0f,
		0.0f, 1.0f,	0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,	0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	memcpy( mat->data, values, sizeof( values ) );
}


//------------------------------------------------------------------------
void MatrixMakeScale( mat44_fl *mat, float c )
{
	float const values[] = {
		c,		0.0f, 0.0f, 0.0f,
		0.0f, c,		0.0f, 0.0f,
		0.0f, 0.0f, c,		0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	// column major sets are a tad quicker, and this one is the same transposed.
	memcpy( mat->data, values, sizeof( values ) );
}


//------------------------------------------------------------------------
void MatrixMakeTranslation( mat44_fl *mat, Vector3 const &offset )
{
	MatrixMakeIdentity( mat );
	MatrixSetOffset( mat, offset );
}


//------------------------------------------------------------------------
void MatrixMakeRotationAroundX( mat44_fl *mat, float const rad )
{
	float cosr = cos( rad );
	float sinr = sin( rad );

	float const values[] = {
		1.0f,		0.0f,		0.0f,		0.0f,
		0.0f,		cosr,		sinr,	   0.0f,
		0.0f,		-sinr,	cosr,		0.0f,
		0.0f,		0.0f,		0.0f,		1.0f
	};

	memcpy( mat->data, values, sizeof( values ) );
}


//------------------------------------------------------------------------
void MatrixMakeRotationAroundY( mat44_fl *mat, float const rad )
{
	float cosr = cos( rad );
	float sinr = sin( rad );

	float const values[] = {
		cosr,		0.0f,		sinr,	   0.0f,
		0.0f,		1.0f,		0.0f,		0.0f,
		-sinr,	0.0f,		cosr,		0.0f,
		0.0f,		0.0f,		0.0f,		1.0f
	};

	memcpy( mat->data, values, sizeof( values ) );
}


//------------------------------------------------------------------------
void MatrixMakeRotationAroundZ( mat44_fl *mat, float const rad )
{
	float cosr = cos( rad );
	float sinr = sin( rad );

	float const values[] = {
		cosr,		sinr,   0.0f,		0.0f,
		-sinr,	cosr,		0.0f,		0.0f,
		0.0f,		0.0f,		1.0f,		0.0f,
		0.0f,		0.0f,		0.0f,		1.0f
	};

	memcpy( mat->data, values, sizeof( values ) );
}


//------------------------------------------------------------------------
void EulerRight( Vector3 *v, float const yaw, float const pitch, float const roll )
{
	float sx = sin( pitch );
	float cx = cos( pitch );

	float sy = sin( yaw );
	float cy = cos( yaw );

	float sz = sin( roll );
	float cz = cos( roll );

	*v = Vector3( cy*cz + sx*sy*sz, -cx*sx, -cz*sy + cy*sx*sz );
}


//------------------------------------------------------------------------
void EulerUp( Vector3 *v, float const yaw, float const pitch, float const roll )
{
	float sx = sin( pitch );
	float cx = cos( pitch );

	float sy = sin( yaw );
	float cy = cos( yaw );

	float sz = sin( roll );
	float cz = cos( roll );

	*v = Vector3( cy*sz - cz*sx*sy, cx*cz, -sy*sz - cy*cz*sx );
}


//------------------------------------------------------------------------
void EulerForward( Vector3 *v, float const yaw, float const pitch, float const roll )
{
	roll;
	float sx = sin( pitch );
	float cx = cos( pitch );

	float sy = sin( yaw );
	float cy = cos( yaw );

	*v = Vector3( cx*sy, sx, cx*cy );
}


//------------------------------------------------------------------------
void MatrixGenerateBasis( mat44_fl *outm, Vector3 const &forward )
{
	Vector3 f = forward;
	f.Normalize();
	Vector3 r;
	Vector3 u;

	Vector3 up = Vector3( 0.f, 0.f, 1.f );
	Vector3 right = Vector3( 1.f, 0.f, 0.f );

	if ( IsCollinearVector3( forward, up ) ) {
		u = CrossProductVector3( f, right );
		r = CrossProductVector3( u, f );
	}
	else {
		r = CrossProductVector3( up, f );
		u = CrossProductVector3( f, r );
	}

	MatrixSetBasis( outm, r, u, f, Vector3::ZERO );
}


//------------------------------------------------------------------------
void MatrixMakeLookTo( mat44_fl *mat, Vector3 const &pos, Vector3 const &dir, Vector3 const &up )
{
	Vector3 u = up;
	Vector3 f = dir;

	u.Normalize();
	f.Normalize();
	Vector3 r = CrossProductVector3( u, f );
	u = CrossProductVector3( f, r );
	u.Normalize();

	MatrixSetBasis( mat, r, u, f, pos );
}


//------------------------------------------------------------------------
void MatrixMakeLookAt( mat44_fl *mat, Vector3 const &from, Vector3 const &to, Vector3 const &up )
{
	MatrixMakeLookTo( mat, from, to - from, up );
}


//------------------------------------------------------------------------

void MatrixMakeProjOrthogonal( mat44_fl *mat, float nx, float fx, float ny, float fy, float nz, float fz )
{
	float sx = 1.0f / ( fx - nx );
	float sy = 1.0f / ( fy - ny );
	float sz = 1.0f / ( fz - nz );

	// maps (n, f) to (-1, 1)
	// == (x - n) / (f - n) * (2) - 1;
	// == 2(x - n) / (f - n) - (f - n)/(f-n)
	// == (2x / (f - n)) + (-2n - f + n)/(f - n)
	// == (2x / (f - n)) - (f + n) / (f - n);

	// So x = n, (- f - n + 2n) / (f - n) = -(f - n)/(f - n) == -1, checks out
	// And x = f, (-f - n + 2f) / (f - n) = (f - n) / (f - n) = 1, checks out

	float const values[] = {
		2.0f * sx,	0.0f,			0.0f,			-( fx + nx ) * sx,
		0.0f,			2.0f * sy,	0.0f,			-( fy + ny ) * sy,
		0.0f,			0.0f,			2.0f * sz,  -( fz + nz ) * sz,
		0.0f,			0.0f,			0.0f,			1.0f,
	};

	memcpy( mat->data, values, sizeof( values ) );
}


//------------------------------------------------------------------------
void MatrixMakeProjOrthogonal( mat44_fl *mat, float width, float height, float nz, float fz )
{
	float hw = width / 2.0f;
	float hh = height / 2.0f;
	MatrixMakeProjOrthogonal( mat, -hw, hw, -hh, hh, nz, fz );
}


//------------------------------------------------------------------------
void MatrixMakeProjPerspective( mat44_fl *mat, float ratio, float rads, float nz, float fz )
{
	//THIS IS WRONG
	float size = atan( rads / 2.0f );

	float w = size;
	float h = size;
	if ( ratio > 1.0f ) {
		w *= ratio;
	}
	else {
		h /= ratio;
	}

	float q = 1.0f / ( fz - nz );

	float const values[] = {
		1.0f / w,	0.0f,			0.0f,			   0.0f,
		0.0f,			1.0f / h,	0.0f,			   0.0f,
		0.0f,			0.0f,			( fz + nz ) * q,	-2.0f * nz * fz * q,
		0.0f,			0.0f,			1.0f,			   0.0f,
	};

	memcpy( mat->data, values, sizeof( values ) );
}


//------------------------------------------------------------------------
void MatrixMakePerspective( mat44_fl* mat, float fov_degrees, float aspect, float nz, float fz )
{
	float rads = ConvertDegreesToRadians( fov_degrees );
	//float size = atan(rads / 2.f);
	float size = 1.0f / tan( rads / 2.f );

	float w = size;
	float h = size;
	if ( aspect > 1.f )
	{
		w /= aspect;
	}
	else {
		h *= aspect;
	}

	float q = 1.f / ( fz - nz );

	//ROW MAJOR
	float const values[] = {
		w, 0.f, 0.0f, 0.0f,
		0.0f, h, 0.0f, 0.0f,
		0.0f, 0.0f, ( fz + nz ) * q, 1.f,
		0.0f, 0.0f, -2.f * nz * fz * q, 0.0f,
	};


	memcpy( mat->data, values, sizeof( values ) );
}


//------------------------------------------------------------------------
void MatrixTranslate( mat44_fl *mat, Vector3 const &o )
{
	Vector4 v;
	MatrixGetRow( mat, 3, &v );
	v.x += o.x;
	v.y += o.y;
	v.z += o.z;
	MatrixSetRow( mat, 3, v );
}


//------------------------------------------------------------------------
void MatrixGetBasis( mat44_fl const *mat, Vector3 *r, Vector3 *u, Vector3 *f, Vector3 *o )
{
	Vector4 x, y, z, w;
	MatrixGetRows( mat, &x, &y, &z, &w );
	*r = Vector3( x.x, x.y, x.z );
	*u = Vector3( y.x, y.y, y.z );
	*f = Vector3( z.x, z.y, z.z );
	*o = Vector3( w.x, w.y, w.z );
}


//------------------------------------------------------------------------
void MatrixSetBasis( mat44_fl *mat, Vector3 const &r, Vector3 const &u, Vector3 const &f, Vector3 const &o )
{
	MatrixSetRows( mat,
		Vector4( r, 0.0f ),
		Vector4( u, 0.0f ),
		Vector4( f, 0.0f ),
		Vector4( o, 1.0f ) );
}


//------------------------------------------------------------------------
Vector3 MatrixGetRight( mat44_fl const *mat )
{
	Vector4 row;
	MatrixGetRow( mat, 0, &row );
	return row.GetXYZ();
}


//------------------------------------------------------------------------
Vector3 MatrixGetUp( mat44_fl const *mat )
{
	Vector4 row;
	MatrixGetRow( mat, 1, &row );
	return row.GetXYZ();
}


//------------------------------------------------------------------------
Vector3 MatrixGetForward( mat44_fl const *mat )
{
	Vector4 row;
	MatrixGetRow( mat, 2, &row );
	return row.GetXYZ();
}


//------------------------------------------------------------------------
void MatrixSetForward( mat44_fl *mat, Vector3 const &forward )
{
	Vector4 row;
	MatrixGetRow( mat, 2, &row );
	row.SetXYZ( forward.x, forward.y, forward.z, row.w );
}


//------------------------------------------------------------------------
void MatrixGetRows( mat44_fl const *mat, Vector4 *x, Vector4 *y, Vector4 *z, Vector4 *o )
{
	MatrixGetRow( mat, 0, x );
	MatrixGetRow( mat, 1, y );
	MatrixGetRow( mat, 2, z );
	MatrixGetRow( mat, 3, o );
}


//------------------------------------------------------------------------
void MatrixSetRows( mat44_fl *mat, Vector4 const &x, Vector4 const &y, Vector4 const &z, Vector4 const &o )
{
	MatrixSetRow( mat, 0, x );
	MatrixSetRow( mat, 1, y );
	MatrixSetRow( mat, 2, z );
	MatrixSetRow( mat, 3, o );
}


//-------------------------------------------------------------------------------------------------
void mat44_fl::MakeRotationEuler( EulerAngles const &orientation, Vector3 const &position )
{
	float yawDegrees = orientation.m_yawDegreesAboutZ;//orientation.m_yawDegreesAboutY;
	float pitchDegrees = orientation.m_pitchDegreesAboutX;//orientation.m_pitchDegreesAboutX;
	float rollDegrees = orientation.m_rollDegreesAboutY;//orientation.m_rollDegreesAboutZ;

	MakeRotationEuler( yawDegrees, pitchDegrees, rollDegrees, position );
}


//-------------------------------------------------------------------------------------------------
void mat44_fl::MakeRotationEuler( float yawDegrees, float pitchDegrees, float rollDegrees, Vector3 const &position )
{
	float sx = sin( ConvertDegreesToRadians( pitchDegrees ) );
	float cx = cos( ConvertDegreesToRadians( pitchDegrees ) );

	float sy = sin( ConvertDegreesToRadians( yawDegrees ) );
	float cy = cos( ConvertDegreesToRadians( yawDegrees ) );

	float sz = sin( ConvertDegreesToRadians( rollDegrees ) );
	float cz = cos( ConvertDegreesToRadians( rollDegrees ) );

	float const values[] = {
		cy*cz + sx*sy*sz,	-cx*sz,	-cz*sy + cy*sx*sz , 0.f,
		cy*sz - cz*sx*sy,	cx*cz,	-sy*sz - cy*cz*sx,	0.f,
		cx*sy,				sx ,	cx*cy,				0.f,
		position.x,				position.z,	position.y,				1.f
	};

	memcpy( data, values, sizeof( values ) );
}


//-----------------------------------------------------------------------------------------------
void mat44_fl::MatrixCOB()
{
	//temp = y
	float yCol0 = data[ 1 ];
	float yCol1 = data[ 5 ];
	float yCol2 = data[ 9 ];
	float yCol3 = data[ 13 ];

	//y = z
	data[ 1 ] = data[ 2 ];
	data[ 5 ] = data[ 6 ];
	data[ 9 ] = data[ 10 ];
	data[ 13 ] = data[ 14 ];

	//z = temp
	data[ 2 ] = yCol0;
	data[ 6 ] = yCol1;
	data[ 10 ] = yCol2;
	data[ 14 ] = yCol3;
}


//-------------------------------------------------------------------------------------------------
void mat44_fl::InvertOrthonormal()
{
	//(R*T)^-1 = T^-1 * R^-1

	//Rotation

	mat44_fl matRotation = *this;

	//Clearing the sides
	matRotation.data[ 3 ] = 0.f;
	matRotation.data[ 7 ] = 0.f;
	matRotation.data[ 11 ] = 0.f;

	matRotation.data[ 12 ] = 0.f;
	matRotation.data[ 13 ] = 0.f;
	matRotation.data[ 14 ] = 0.f;
	matRotation.data[ 15 ] = 1.f;

	//Invert Rotation

	MatrixTransposeRotation( &matRotation );

	//Get Translation and Invert
	Vector3 translation = MatrixGetOffset( this );
	translation = -translation;
	mat44_fl matTrans = mat44_fl::identity;
	MatrixSetOffset( &matTrans, translation );

	//(R * T)^-1 = T^-1 * R^1
	MatrixMultiply( this, &matTrans, &matRotation );

	/*
	mat44_fl rotation(&this->data[0]);
	rotation.data[12] = 0.f;
	rotation.data[13] = 0.f;
	rotation.data[14] = 0.f;

	//Rotation^-1
	std::swap(rotation.data[1], rotation.data[4]);
	std::swap(rotation.data[2], rotation.data[8]);
	std::swap(rotation.data[6], rotation.data[9]);

	//Translation^-1
	mat44_fl translation(identity);
	MatrixTranslate(this, Vector3(-data[12], -data[13], -data[14]));
	//translation.SetTranslation(-m_data[12], -m_data[13], -m_data[14]);

	mat44_fl const inverse = translation * rotation;

	memcpy(&data[0], &inverse.data[0], sizeof(float) * 16);
	*/
}


//-----------------------------------------------------------------------------------------------
void MatrixTransposeRotation( mat44_fl* mat )
{
	//temp = y
	float item0 = mat->data[ 1 ];
	float item1 = mat->data[ 2 ];
	float item2 = mat->data[ 6 ];

	//y = z
	mat->data[ 1 ] = mat->data[ 4 ];
	mat->data[ 2 ] = mat->data[ 8 ];
	mat->data[ 6 ] = mat->data[ 9 ];

	//z = temp
	mat->data[ 4 ] = item0;
	mat->data[ 8 ] = item1;
	mat->data[ 9 ] = item2;
}