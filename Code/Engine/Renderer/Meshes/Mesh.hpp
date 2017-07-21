#pragma once

#include <vector>

#include "Engine/Renderer/Vertices/Vertex.hpp"
#include "Engine/Renderer/OpenGLExtensions.hpp"


//-----------------------------------------------------------------------------------------------
enum PrimitiveType;
class Material;


//-----------------------------------------------------------------------------------------------
class Mesh
{
public:
	Mesh();
	void MakeLineMesh( const Vector3& point1, const Vector3& point2, const Rgba& color );
	void MakeQuadMesh();
	void MakeCubeMesh();
	void MakeSphereMesh();
	GLuint BufferCreate( void* data, size_t count, size_t elem_size, GLenum usage );
	void BindWithMaterial( Material* mat );
	void DrawWithMaterial( Material* mat, PrimitiveType primitiveType );
	void MakeTangentQuadMesh();
	void MakeNDCTangentQuadMesh();
	void MakeTangentCubeMesh();
	void MakeTangentSphereMesh();
	void BindAttributeToMaterial( Material* mat, std::string name, GLsizei stride, const GLvoid* pointer );
	void BindWithTangentMaterial( Material* mat );

public:
	std::vector< Vertex_PCT > m_verts;
	std::vector< Vertex_PCUTB > m_tangentVerts;
	std::vector< uint16_t > m_indices;
	GLuint m_VAO;
	GLuint m_VBO;
	GLuint m_IBO;
};