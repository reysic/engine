#pragma once

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/UIntVector4.hpp"
#include "Engine/Core/Rgba.hpp"


//-----------------------------------------------------------------------------------------------
enum VertexType
{
	VertexType_PC, // #TODO: Define this vertex type
	VertexType_PCT,
	VertexType_PCUTB,
	VertexType_SKINNED_MESH
};


//-----------------------------------------------------------------------------------------------
struct Vertex_PC
{
	Vertex_PC( const Vector3& position = Vector3::ZERO, const Rgba& color = Rgba::WHITE );

	Vector3 m_position;
	Rgba m_color;
};


//-----------------------------------------------------------------------------------------------
struct Vertex_PCT
{
	Vertex_PCT();
	Vertex_PCT( const Vector3& position, const Rgba& color, const Vector2& texCoords );

	Vector3 m_position;
	Rgba m_color;
	Vector2 m_texCoords;
};


//-----------------------------------------------------------------------------------------------
struct Vertex_PCUTB
{
	Vertex_PCUTB();
	Vertex_PCUTB( const Vector3& position, const Rgba& color, const Vector2& texCoords, 
		const Vector3& tangent, const Vector3& bitangent );

	Vector3 m_position;
	Rgba m_color;
	Vector2 m_texCoords;
	Vector3 m_tangent;
	Vector3 m_bitangent;
};


//-----------------------------------------------------------------------------------------------
struct Vertex_Master
{
	Vertex_Master( const Vector3& position = Vector3::ZERO, const Rgba& color = Rgba::WHITE, 
		const Vector2& texCoords = Vector2::ZERO, const Vector2& texCoords2 = Vector2::ZERO, 
		const Vector3& tangent = Vector3( 1.0f, 0.0f, 0.0f ), 
		const Vector3& bitangent = Vector3( 0.0f, 1.0f, 0.0f ), 
		const Vector3& normal = Vector3( 0.0f, 1.0f, 0.0f ),
		const Vector4& boneWeights = Vector4( 1.0f, 0.0f, 0.0f, 0.0f),
		const UIntVector4& m_boneIndices = UIntVector4( 0, 0, 0, 0 ) );

	friend bool operator==( const Vertex_Master& lhs, const Vertex_Master& rhs );
	Vector3 m_position;
	Rgba m_color;
	Vector2 m_texCoords;
	Vector2 m_texCoords2;
	Vector3 m_tangent;
	Vector3 m_bitangent;
	Vector3 m_normal;

	// New for skinning
	Vector4 m_boneWeights;		// Adds up to 1
	UIntVector4 m_boneIndices;	// unsigned int Vector4
	// defaults
	// m_boneWeights = 1, 0, 0, 0
	// m_boneIndicies = 0, 0, 0, 0
};


//-----------------------------------------------------------------------------------------------
struct Vertex_SKINNED_MESH
{
public:
	Vector3 m_position;
	Rgba m_color;
	Vector2 m_texCoords;
	Vector3 m_tangent;
	Vector3 m_bitangent;

	UIntVector4 m_boneIndices; // unsigned int vec4
	Vector4 m_boneWeights;  // add up to one.   (x + y + z + w == 1.0f)
public:
	Vertex_SKINNED_MESH( Vector3 const &position = Vector3::ZERO, Rgba const &color = Rgba::WHITE, 
		Vector2 const &texCoords = Vector2::ZERO,
		Vector3 const &tangent = Vector3( 1.f, 0.f, 0.f ), 
		Vector3 const &bitangent = Vector3( 0.f, 0.f, 1.f ),
		Vector4 boneWeights = Vector4( 1.f, 0.f, 0.f, 0.f ), 
		UIntVector4 boneIndices = UIntVector4( 0, 0, 0, 0 ) );

};