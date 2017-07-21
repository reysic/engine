#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Meshes/Mesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
Mesh::Mesh()
{
}


//-----------------------------------------------------------------------------------------------
void Mesh::MakeLineMesh( const Vector3& point1, const Vector3& point2, const Rgba& color )
{
	m_verts.push_back( Vertex_PCT( Vector3( point1.x, point1.y, point1.z ), color, Vector2( 0.0f, 0.0f ) ) );
	m_verts.push_back( Vertex_PCT( Vector3( point2.x, point2.y, point2.z ), color, Vector2( 1.0f, 1.0f ) ) );

	m_indices = {
		0, 1,
	};

	m_VBO = BufferCreate( &m_verts[ 0 ], m_verts.size(), sizeof( m_verts[ 0 ] ), GL_STATIC_DRAW );

	m_IBO = BufferCreate( &m_indices[ 0 ], m_indices.size(), sizeof( m_indices[ 0 ] ), GL_STATIC_DRAW );

	glGenVertexArrays( 1, &m_VAO );
}


//-----------------------------------------------------------------------------------------------
void Mesh::MakeQuadMesh()
{
	m_verts.push_back( Vertex_PCT( Vector3( -0.5f, 0.5f, 0.0f ), Rgba::WHITE, Vector2( 0.f, 0.f ) ) ); //tl
	m_verts.push_back( Vertex_PCT( Vector3( -0.5f, -0.5f, 0.0f ), Rgba::WHITE, Vector2( 0.f, 1.f ) ) ); //bl
	m_verts.push_back( Vertex_PCT( Vector3( 0.5f, -0.5f, 0.0f ), Rgba::WHITE, Vector2( 1.f, 1.f ) ) ); //br
	m_verts.push_back( Vertex_PCT( Vector3( 0.5f, 0.5f, 0.0f ), Rgba::WHITE, Vector2( 1.f, 0.f ) ) ); //tr

	m_indices = {
		0, 1, 2,
		0, 2, 3,
	};

	m_VBO = BufferCreate( &m_verts[ 0 ], m_verts.size(), sizeof( m_verts[ 0 ] ), GL_STATIC_DRAW );

	m_IBO = BufferCreate( &m_indices[ 0 ], m_indices.size(), sizeof( m_indices[ 0 ] ), GL_STATIC_DRAW );

	glGenVertexArrays( 1, &m_VAO );
}



//-----------------------------------------------------------------------------------------------
void Mesh::MakeCubeMesh()
{
	// Add all vertexes to the mesh for VAO
	// Front face
	m_verts.push_back( Vertex_PCT( Vector3( -.5f, -.5f, .5f ), Rgba::WHITE, Vector2( 0.f, 0.f ) ) ); //tl
	m_verts.push_back( Vertex_PCT( Vector3( -.5f, -.5f, -.5f ), Rgba::WHITE, Vector2( 0.f, 1.f ) ) ); //bl
	m_verts.push_back( Vertex_PCT( Vector3( .5f, -.5f, -.5f ), Rgba::WHITE, Vector2( 1.f, 1.f ) ) ); //br
	m_verts.push_back( Vertex_PCT( Vector3( .5f, -.5f, .5f ), Rgba::WHITE, Vector2( 1.f, 0.f ) ) ); //tr

																									//  Back face
	m_verts.push_back( Vertex_PCT( Vector3( .5f, .5f, .5f ), Rgba::WHITE, Vector2( 0.f, 0.f ) ) ); //tl
	m_verts.push_back( Vertex_PCT( Vector3( .5f, .5f, -.5f ), Rgba::WHITE, Vector2( 0.f, 1.f ) ) ); //bl
	m_verts.push_back( Vertex_PCT( Vector3( -.5f, .5f, -.5f ), Rgba::WHITE, Vector2( 1.f, 1.f ) ) ); //br
	m_verts.push_back( Vertex_PCT( Vector3( -.5f, .5f, .5f ), Rgba::WHITE, Vector2( 1.f, 0.f ) ) ); //tr

																									// Top face
	m_verts.push_back( Vertex_PCT( Vector3( -.5f, .5f, .5f ), Rgba::WHITE, Vector2( 0.f, 0.f ) ) ); //tl
	m_verts.push_back( Vertex_PCT( Vector3( -.5f, -.5f, .5f ), Rgba::WHITE, Vector2( 0.f, 1.f ) ) ); //bl
	m_verts.push_back( Vertex_PCT( Vector3( .5f, -.5f, .5f ), Rgba::WHITE, Vector2( 1.f, 1.f ) ) ); //br
	m_verts.push_back( Vertex_PCT( Vector3( .5f, .5f, .5f ), Rgba::WHITE, Vector2( 1.f, 0.f ) ) ); //tr

																								   // Bottom Face
	m_verts.push_back( Vertex_PCT( Vector3( .5f, .5f, -.5f ), Rgba::WHITE, Vector2( 0.f, 0.f ) ) ); //tr
	m_verts.push_back( Vertex_PCT( Vector3( .5f, -.5f, -.5f ), Rgba::WHITE, Vector2( 0.f, 1.f ) ) ); //br
	m_verts.push_back( Vertex_PCT( Vector3( -.5f, -.5f, -.5f ), Rgba::WHITE, Vector2( 1.f, 1.f ) ) ); //bl
	m_verts.push_back( Vertex_PCT( Vector3( -.5f, .5f, -.5f ), Rgba::WHITE, Vector2( 1.f, 0.f ) ) ); //tl

																									 // Right face
	m_verts.push_back( Vertex_PCT( Vector3( .5f, .5f, .5f ), Rgba::WHITE, Vector2( 1.f, 0.f ) ) ); //tr
	m_verts.push_back( Vertex_PCT( Vector3( .5f, -.5f, .5f ), Rgba::WHITE, Vector2( 0.f, 0.f ) ) ); //br
	m_verts.push_back( Vertex_PCT( Vector3( .5f, -.5f, -.5f ), Rgba::WHITE, Vector2( 0.f, 1.f ) ) ); //bl
	m_verts.push_back( Vertex_PCT( Vector3( .5f, .5f, -.5f ), Rgba::WHITE, Vector2( 1.f, 1.f ) ) ); //tl

																									// Left face
	m_verts.push_back( Vertex_PCT( Vector3( -.5f, .5f, -.5f ), Rgba::WHITE, Vector2( 0.f, 1.f ) ) ); //tl
	m_verts.push_back( Vertex_PCT( Vector3( -.5f, -.5f, -.5f ), Rgba::WHITE, Vector2( 1.f, 1.f ) ) ); //bl
	m_verts.push_back( Vertex_PCT( Vector3( -.5f, -.5f, .5f ), Rgba::WHITE, Vector2( 1.f, 0.f ) ) ); //br
	m_verts.push_back( Vertex_PCT( Vector3( -.5f, .5f, .5f ), Rgba::WHITE, Vector2( 0.f, 0.f ) ) ); //tr

																									// Add all indices to the mesh for IBO
	m_indices = {
		0, 1, 2,
		0, 2, 3,

		4, 5, 6,
		4, 6, 7,

		8, 9, 10,
		8, 10, 11,

		12, 13, 14,
		12, 14, 15,

		16, 17, 18,
		16, 18, 19,

		20, 21, 22,
		20, 22, 23
	};

	m_VBO = BufferCreate( &m_verts[ 0 ], m_verts.size(), sizeof( m_verts[ 0 ] ), GL_STATIC_DRAW );

	m_IBO = BufferCreate( &m_indices[ 0 ], m_indices.size(), sizeof( m_indices[ 0 ] ), GL_STATIC_DRAW );

	glGenVertexArrays( 1, &m_VAO );
}


//-----------------------------------------------------------------------------------------------
void Mesh::MakeSphereMesh()
{
	// Add all vertexes to the mesh for VAO, add all indices to the mesh for IBO
	float radius = 1.0f;
	int numRings = 100;
	int numSectors = 100;
	int currentRow = 0;
	int nextRow = 0;
	int nextSector;

	float const R = 1.0f / ( float ) ( numRings - 1 );
	float const S = 1.0f / ( float ) ( numSectors - 1 );

	for ( int r = 0; r < numRings; ++r )
	{
		for ( int s = 0; s < numSectors; ++s )
		{
			float const z = sin( -pi / 2 + pi * r * R );
			float const y = cos( 2 * pi * s * S ) * sin( pi * r * R );
			float const x = sin( 2 * pi * s * S ) * sin( pi * r * R );

			m_verts.push_back( Vertex_PCT( Vector3( x * radius, y * radius, -z * radius ), Rgba::WHITE, Vector2( 1 - s * S, r * R ) ) );

			currentRow = r * numSectors;
			nextRow = ( r + 1 ) * numSectors;
			nextSector = ( s + 1 ) % numSectors;

			if ( r < numRings - 1 )
			{
				m_indices.push_back( ( uint16_t ) ( nextRow + nextSector ) );
				m_indices.push_back( ( uint16_t ) ( nextRow + s ) );
				m_indices.push_back( ( uint16_t ) ( currentRow + s ) );

				m_indices.push_back( ( uint16_t ) ( currentRow + nextSector ) );
				m_indices.push_back( ( uint16_t ) ( nextRow + nextSector ) );
				m_indices.push_back( ( uint16_t ) ( currentRow + s ) );
			}
		}
	}

	m_VBO = BufferCreate( &m_verts[ 0 ], m_verts.size(), sizeof( m_verts[ 0 ] ), GL_STATIC_DRAW );

	m_IBO = BufferCreate( &m_indices[ 0 ], m_indices.size(), sizeof( m_indices[ 0 ] ), GL_STATIC_DRAW );

	glGenVertexArrays( 1, &m_VAO );
}


//-----------------------------------------------------------------------------------------------
GLuint Mesh::BufferCreate( void *data, size_t count, size_t elem_size, GLenum usage )
{
	GLuint  buffer;
	glGenBuffers( 1, &buffer );

	glBindBuffer( GL_ARRAY_BUFFER, buffer );
	glBufferData( GL_ARRAY_BUFFER, count * elem_size, data, usage );
	glBindBuffer( GL_ARRAY_BUFFER, NULL );

	return buffer;
}


//-----------------------------------------------------------------------------------------------
void Mesh::BindWithMaterial( Material* mat )
{
	glGenVertexArrays( 1, &m_VAO );
	if ( m_VAO != NULL )
	{
		glBindVertexArray( m_VAO );
		glBindBuffer( GL_ARRAY_BUFFER, m_VBO );

		mat->SetAttribute( "inPosition" );
		mat->SetAttribute( "inColor" );
		mat->SetAttribute( "inUV0" );

		glBindBuffer( GL_ARRAY_BUFFER, NULL );

		if ( NULL != m_IBO )
		{
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IBO );
		}

		glBindVertexArray( NULL );
	}
}


//-----------------------------------------------------------------------------------------------
void Mesh::DrawWithMaterial( Material* mat, PrimitiveType primitiveType )
{
	glBindVertexArray( m_VAO );
	glUseProgram( ( GLuint ) mat->m_shaderProgram );

	switch ( primitiveType )
	{
	case 0: // PRIMITIVE_POINTS
		glDrawElements( GL_POINTS, m_indices.size(), GL_UNSIGNED_SHORT, ( GLvoid* ) 0 );
		break;
	case 1: // PRIMITIVE_LINES
		glDrawElements( GL_LINES, m_indices.size(), GL_UNSIGNED_SHORT, ( GLvoid* ) 0 );
		break;
	case 2: // PRIMITIVE_LINE_LOOP
		glDrawElements( GL_LINE_LOOP, m_indices.size(), GL_UNSIGNED_SHORT, ( GLvoid* ) 0 );
		break;
	case 3: // PRIMITIVE_TRIANGLES
		glDrawElements( GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_SHORT, ( GLvoid* ) 0 );
		break;
	case 4: // PRIMITIVE_QUADS
		glDrawElements( GL_QUADS, m_indices.size(), GL_UNSIGNED_SHORT, ( GLvoid* ) 0 );
		break;
	}

	glUseProgram( NULL );
	glBindVertexArray( NULL );
}


//-----------------------------------------------------------------------------------------------
void Mesh::MakeTangentQuadMesh()
{
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( -.5f, 0.0f, .5f ), Rgba::WHITE, Vector2( 0.f, 0.f ), Vector3( 1.f, 0.f, 0.f ), Vector3( 0.f, 0.f, 1.f ) ) ); //tl
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( -.5f, 0.0f, -.5f ), Rgba::WHITE, Vector2( 0.f, 1.f ), Vector3( 1.f, 0.f, 0.f ), Vector3( 0.f, 0.f, 1.f ) ) ); //bl
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( .5f, 0.0f, -.5f ), Rgba::WHITE, Vector2( 1.f, 1.f ), Vector3( 1.f, 0.f, 0.f ), Vector3( 0.f, 0.f, 1.f ) ) ); //br
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( .5f, 0.0f, .5f ), Rgba::WHITE, Vector2( 1.f, 0.f ), Vector3( 1.f, 0.f, 0.f ), Vector3( 0.f, 0.f, 1.f ) ) ); //tr

	m_indices = {
		0, 1, 2,
		0, 2, 3
	};

	m_VBO = BufferCreate( &m_tangentVerts[ 0 ], m_tangentVerts.size(), sizeof( m_tangentVerts[ 0 ] ), GL_STATIC_DRAW );

	m_IBO = BufferCreate( &m_indices[ 0 ], m_indices.size(), sizeof( m_indices[ 0 ] ), GL_STATIC_DRAW );

	glGenVertexArrays( 1, &m_VAO );
}


//-----------------------------------------------------------------------------------------------
void Mesh::MakeNDCTangentQuadMesh()
{
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( -1.0f, 1.0f, 0.0f ), Rgba::WHITE, Vector2( 0.f, 0.f ), Vector3( 1.f, 0.f, 0.f ), Vector3( 0.f, 0.f, 1.f ) ) ); //tl
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( -1.0f, -1.0f, 0.0f ), Rgba::WHITE, Vector2( 0.f, 1.f ), Vector3( 1.f, 0.f, 0.f ), Vector3( 0.f, 0.f, 1.f ) ) ); //bl
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( 1.0f, -1.0f, 0.0f ), Rgba::WHITE, Vector2( 1.f, 1.f ), Vector3( 1.f, 0.f, 0.f ), Vector3( 0.f, 0.f, 1.f ) ) ); //br
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( 1.0f, 1.0f, 0.0f ), Rgba::WHITE, Vector2( 1.f, 0.f ), Vector3( 1.f, 0.f, 0.f ), Vector3( 0.f, 0.f, 1.f ) ) ); //tr

	m_indices = {
		0, 1, 2,
		0, 2, 3
	};

	m_VBO = BufferCreate( &m_tangentVerts[ 0 ], m_tangentVerts.size(), sizeof( m_tangentVerts[ 0 ] ), GL_STATIC_DRAW );

	m_IBO = BufferCreate( &m_indices[ 0 ], m_indices.size(), sizeof( m_indices[ 0 ] ), GL_STATIC_DRAW );

	glGenVertexArrays( 1, &m_VAO );
}


//-----------------------------------------------------------------------------------------------
void Mesh::MakeTangentCubeMesh()
{
	// Add all vertexes to the mesh for VAO
	// Front face
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( -.5f, -.5f, .5f ), Rgba::WHITE, Vector2( 0.f, 0.f ), Vector3( 1.f, 0.f, 0.f ), Vector3( 0.f, 0.f, 1.f ) ) ); //tl
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( -.5f, -.5f, -.5f ), Rgba::WHITE, Vector2( 0.f, 1.f ), Vector3( 1.f, 0.f, 0.f ), Vector3( 0.f, 0.f, 1.f ) ) ); //bl
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( .5f, -.5f, -.5f ), Rgba::WHITE, Vector2( 1.f, 1.f ), Vector3( 1.f, 0.f, 0.f ), Vector3( 0.f, 0.f, 1.f ) ) ); //br
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( .5f, -.5f, .5f ), Rgba::WHITE, Vector2( 1.f, 0.f ), Vector3( 1.f, 0.f, 0.f ), Vector3( 0.f, 0.f, 1.f ) ) ); //tr

																																								 //  Back face
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( .5f, .5f, .5f ), Rgba::WHITE, Vector2( 0.f, 0.f ), Vector3( -1.f, 0.f, 0.f ), Vector3( 0.f, 0.f, 1.f ) ) ); //tl
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( .5f, .5f, -.5f ), Rgba::WHITE, Vector2( 0.f, 1.f ), Vector3( -1.f, 0.f, 0.f ), Vector3( 0.f, 0.f, 1.f ) ) ); //bl
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( -.5f, .5f, -.5f ), Rgba::WHITE, Vector2( 1.f, 1.f ), Vector3( -1.f, 0.f, 0.f ), Vector3( 0.f, 0.f, 1.f ) ) ); //br
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( -.5f, .5f, .5f ), Rgba::WHITE, Vector2( 1.f, 0.f ), Vector3( -1.f, 0.f, 0.f ), Vector3( 0.f, 0.f, 1.f ) ) ); //tr

																																								  // Top face
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( -.5f, .5f, .5f ), Rgba::WHITE, Vector2( 0.f, 0.f ), Vector3( 1.f, 0.f, 0.f ), Vector3( 0.f, 1.f, 0.f ) ) ); //tl
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( -.5f, -.5f, .5f ), Rgba::WHITE, Vector2( 0.f, 1.f ), Vector3( 1.f, 0.f, 0.f ), Vector3( 0.f, 1.f, 0.f ) ) ); //bl
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( .5f, -.5f, .5f ), Rgba::WHITE, Vector2( 1.f, 1.f ), Vector3( 1.f, 0.f, 0.f ), Vector3( 0.f, 1.f, 0.f ) ) ); //br
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( .5f, .5f, .5f ), Rgba::WHITE, Vector2( 1.f, 0.f ), Vector3( 1.f, 0.f, 0.f ), Vector3( 0.f, 1.f, 0.f ) ) ); //tr

																																								// Bottom Face
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( .5f, .5f, -.5f ), Rgba::WHITE, Vector2( 0.f, 0.f ), Vector3( -1.f, 0.f, 0.f ), Vector3( 0.f, 1.f, 0.f ) ) ); //tr
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( .5f, -.5f, -.5f ), Rgba::WHITE, Vector2( 0.f, 1.f ), Vector3( -1.f, 0.f, 0.f ), Vector3( 0.f, 1.f, 0.f ) ) ); //br
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( -.5f, -.5f, -.5f ), Rgba::WHITE, Vector2( 1.f, 1.f ), Vector3( -1.f, 0.f, 0.f ), Vector3( 0.f, 1.f, 0.f ) ) ); //bl
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( -.5f, .5f, -.5f ), Rgba::WHITE, Vector2( 1.f, 0.f ), Vector3( -1.f, 0.f, 0.f ), Vector3( 0.f, 1.f, 0.f ) ) ); //tl

																																								   // Right face
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( .5f, .5f, .5f ), Rgba::WHITE, Vector2( 1.f, 0.f ), Vector3( 0.f, 1.f, 0.f ), Vector3( 0.f, 0.f, 1.f ) ) ); //tr
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( .5f, -.5f, .5f ), Rgba::WHITE, Vector2( 0.f, 0.f ), Vector3( 0.f, 1.f, 0.f ), Vector3( 0.f, 0.f, 1.f ) ) ); //br
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( .5f, -.5f, -.5f ), Rgba::WHITE, Vector2( 0.f, 1.f ), Vector3( 0.f, 1.f, 0.f ), Vector3( 0.f, 0.f, 1.f ) ) ); //bl
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( .5f, .5f, -.5f ), Rgba::WHITE, Vector2( 1.f, 1.f ), Vector3( 0.f, 1.f, 0.f ), Vector3( 0.f, 0.f, 1.f ) ) ); //tl

																																								 // Left face
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( -.5f, .5f, -.5f ), Rgba::WHITE, Vector2( 0.f, 1.f ), Vector3( 0.f, -1.f, 0.f ), Vector3( 0.f, 0.f, 1.f ) ) ); //tl
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( -.5f, -.5f, -.5f ), Rgba::WHITE, Vector2( 1.f, 1.f ), Vector3( 0.f, -1.f, 0.f ), Vector3( 0.f, 0.f, 1.f ) ) ); //bl
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( -.5f, -.5f, .5f ), Rgba::WHITE, Vector2( 1.f, 0.f ), Vector3( 0.f, -1.f, 0.f ), Vector3( 0.f, 0.f, 1.f ) ) ); //br
	m_tangentVerts.push_back( Vertex_PCUTB( Vector3( -.5f, .5f, .5f ), Rgba::WHITE, Vector2( 0.f, 0.f ), Vector3( 0.f, -1.f, 0.f ), Vector3( 0.f, 0.f, 1.f ) ) ); //tr

																																								  // Add all indices to the mesh for IBO
	m_indices = {
		0, 1, 2,
		0, 2, 3,

		4, 5, 6,
		4, 6, 7,

		8, 9, 10,
		8, 10, 11,

		12, 13, 14,
		12, 14, 15,

		16, 17, 18,
		16, 18, 19,

		20, 21, 22,
		20, 22, 23
	};

	m_VBO = BufferCreate( &m_tangentVerts[ 0 ], m_tangentVerts.size(), sizeof( m_tangentVerts[ 0 ] ), GL_STATIC_DRAW );

	m_IBO = BufferCreate( &m_indices[ 0 ], m_indices.size(), sizeof( m_indices[ 0 ] ), GL_STATIC_DRAW );

	glGenVertexArrays( 1, &m_VAO );
}


//-----------------------------------------------------------------------------------------------
void Mesh::MakeTangentSphereMesh()
{
	float radius = 1.0f;
	int numRings = 100;
	int numSectors = 100;
	int currentRow = 0;
	int nextRow = 0;
	int nextSector;

	float const R = 1.0f / ( float ) ( numRings - 1 );
	float const S = 1.0f / ( float ) ( numSectors - 1 );


	for ( int r = 0; r < numRings; ++r )
	{
		for ( int s = 0; s < numSectors; ++s )
		{
			float const z = sin( -pi / 2 + pi * r * R );
			float const y = cos( 2 * pi * s * S ) * sin( pi * r * R );
			float const x = sin( 2 * pi * s * S ) * sin( pi * r * R );

			//r is constant
			float const tanZ = 0.f;
			float const tanY = sin( 2 * pi * s * S );// * -sin(pi * r * R);//-4 * pi * sin(pi * s * S) * cos(pi * s * S) * sin(pi * r * R);//
			float const tanX = cos( 2 * pi * s * S );// * sin(pi * r * R);//0.5f * (pi * cos(pi * r * R - 2 * pi * s * S) + pi * cos(pi * r * R + 2 * pi * s * S));//cos(2 * pi * s * S) * sin(pi * r * R);

													 //s is constant
			float const biTanZ = pi * sin( pi * r * R );
			float const biTanY = cos( 2 * pi * s * S ) * cos( pi * r * R );//0.5f * (pi * cos(pi * r * R - 2 * pi * s * S) + pi * cos(pi * r * R + 2 * pi * s * S));//
			float const biTanX = sin( 2 * pi * s * S ) * cos( pi * r * R );//0.5f * (pi * sin(pi  * r * R + 2 * pi * s * S) - pi * sin(pi  * r * R - 2 * pi * s * S));//

			Vector3 pos = Vector3( x * radius, y * radius, -z * radius );
			Vector3 posNorm = pos.Normalize();



			Vector3 tan = Vector3( -tanX, tanY, tanZ );
			Vector3 biTan = CrossProductVector3( posNorm, tan );
			//Vector3 biTan = Vector3(biTanX, biTanY, biTanZ);

			m_tangentVerts.push_back( Vertex_PCUTB( Vector3( x * radius, y * radius, -z * radius ), Rgba::WHITE, Vector2( 1 - s * S, r * R ),
				tan, biTan ) );

			currentRow = r * numSectors;
			nextRow = ( r + 1 ) * numSectors;
			nextSector = ( s + 1 ) % numSectors;

			if ( r < numRings - 1 )
			{
				m_indices.push_back( ( uint16_t ) ( nextRow + nextSector ) );
				m_indices.push_back( ( uint16_t ) ( nextRow + s ) );
				m_indices.push_back( ( uint16_t ) ( currentRow + s ) );

				m_indices.push_back( ( uint16_t ) ( currentRow + nextSector ) );
				m_indices.push_back( ( uint16_t ) ( nextRow + nextSector ) );
				m_indices.push_back( ( uint16_t ) ( currentRow + s ) );
			}
		}
	}

	m_VBO = BufferCreate( &m_tangentVerts[ 0 ], m_tangentVerts.size(), sizeof( m_tangentVerts[ 0 ] ), GL_STATIC_DRAW );

	m_IBO = BufferCreate( &m_indices[ 0 ], m_indices.size(), sizeof( m_indices[ 0 ] ), GL_STATIC_DRAW );

	glGenVertexArrays( 1, &m_VAO );
}


//-----------------------------------------------------------------------------------------------
void Mesh::BindAttributeToMaterial( Material* mat, std::string name, GLsizei stride, const GLvoid* pointer )
{
	if ( m_VAO != NULL )
	{
		glBindVertexArray( m_VAO );
		glBindBuffer( GL_ARRAY_BUFFER, m_VBO );


		mat->SetAttribute( name, stride, pointer );


		glBindBuffer( GL_ARRAY_BUFFER, NULL );

		if ( NULL != m_IBO ) {
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IBO );
		}

		glBindVertexArray( NULL );
	}
}


//-----------------------------------------------------------------------------------------------
void Mesh::BindWithTangentMaterial( Material* mat )
{
	if ( m_VAO != NULL )
	{
		glBindVertexArray( m_VAO );
		glBindBuffer( GL_ARRAY_BUFFER, m_VBO );
		glUseProgram( mat->m_shaderProgram->m_programID );


		for ( auto nameIter = mat->m_attributes.begin(); nameIter != mat->m_attributes.end(); ++nameIter )
		{
			std::string attribName = ( *nameIter )->m_name;


			GLenum type = ( *nameIter )->m_type;
			switch ( type )
			{
			case GL_FLOAT_VEC2:
			{
				AttributeVec2* newAtrib = dynamic_cast< AttributeVec2* >( ( *nameIter ) );
				glUseProgram( mat->m_shaderProgram->m_programID );
				newAtrib->BindAttribute();
				break;
			}
			case GL_FLOAT_VEC3:
			{
				AttributeVec3* newAtrib = dynamic_cast< AttributeVec3* >( ( *nameIter ) );
				glUseProgram( mat->m_shaderProgram->m_programID );
				if ( attribName == "inPosition" )
				{
					if ( newAtrib->m_location >= 0 )
					{
						glEnableVertexAttribArray( newAtrib->m_location );
						glVertexAttribPointer( newAtrib->m_location, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex_PCUTB ), ( GLvoid* ) offsetof( Vertex_PCUTB, m_position ) );
					}
				}
				else if ( attribName == "inTangent" )
				{
					if ( newAtrib->m_location >= 0 )
					{
						glEnableVertexAttribArray( newAtrib->m_location );
						glVertexAttribPointer( newAtrib->m_location, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex_PCUTB ), ( GLvoid* ) offsetof( Vertex_PCUTB, m_tangent ) );
					}
				}
				else if ( attribName == "inBitangent" )
				{
					if ( newAtrib->m_location >= 0 )
					{
						glEnableVertexAttribArray( newAtrib->m_location );
						glVertexAttribPointer( newAtrib->m_location, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex_PCUTB ), ( GLvoid* ) offsetof( Vertex_PCUTB, m_bitangent ) );
					}
				}
				//newAtrib->BindAttribute();
				break;
			}
			case GL_FLOAT_VEC4:
			{
				AttributeVec4* newAtrib = dynamic_cast< AttributeVec4* >( ( *nameIter ) );
				glUseProgram( mat->m_shaderProgram->m_programID );
				newAtrib->BindAttribute();
				break;
			}
			default:
				break;
			}

		}
		// 		mat->SetAttribute("inPosition");
		// 		mat->SetAttribute("inColor");
		// 		mat->SetAttribute("inUV0");
		// 		mat->SetAttribute("inTangent");
		// 		mat->SetAttribute("inBitangent");

		glBindBuffer( GL_ARRAY_BUFFER, NULL );

		if ( NULL != m_IBO ) {
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_IBO );
		}

		glBindVertexArray( NULL );
	}
}