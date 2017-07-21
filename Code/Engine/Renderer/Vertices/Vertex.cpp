#include "Engine/Renderer/Vertices/Vertex.hpp"


//-----------------------------------------------------------------------------------------------
Vertex_PC::Vertex_PC( const Vector3& position /*= Vector3::ZERO*/, const Rgba& color /*= Rgba::WHITE */ )
	: m_position( position )
	, m_color( color )
{

}


//-----------------------------------------------------------------------------------------------
Vertex_PCT::Vertex_PCT()
	: m_position( Vector3( 0.0f, 0.0f, 0.0f ) )
	, m_color( Rgba::WHITE )
	, m_texCoords( Vector2( 0, 0 ) )
{
}


//-----------------------------------------------------------------------------------------------
Vertex_PCT::Vertex_PCT( const Vector3& position, const Rgba& color, const Vector2& texCoords )
	: m_position( position )
	, m_color( color )
	, m_texCoords( texCoords )
{
}


//-----------------------------------------------------------------------------------------------
Vertex_PCUTB::Vertex_PCUTB()
	: m_position( Vector3( 0.0f, 0.0f, 0.0f ) )
	, m_color( Rgba::WHITE )
	, m_texCoords( Vector2( 0, 0 ) )
	, m_tangent( Vector3( 0.0f, 0.0f, 0.0f ) )
	, m_bitangent( Vector3( 0.0f, 0.0f, 0.0f ) )
{
}


//-----------------------------------------------------------------------------------------------
Vertex_PCUTB::Vertex_PCUTB( const Vector3& position, const Rgba& color, const Vector2& texCoords, const Vector3& tangent, const Vector3& bitangent )
	: m_position( position )
	, m_color( color )
	, m_texCoords( texCoords )
	, m_tangent( tangent )
	, m_bitangent( bitangent )
{
}


//-----------------------------------------------------------------------------------------------
Vertex_Master::Vertex_Master( const Vector3& position /*= Vector3::ZERO*/, 
	const Rgba& color /*= Rgba::WHITE*/, const Vector2& texCoords /*= Vector2::ZERO*/, 
	const Vector2& texCoords2 /*= Vector2::ZERO*/, 
	const Vector3& tangent /*= Vector3( 1.0f, 0.0f, 0.0f )*/, 
	const Vector3& bitangent /*= Vector3( 0.0f, 1.0f, 0.0f )*/, 
	const Vector3& normal /*= Vector3( 0.0f, 1.0f, 0.0f ) */,
	const Vector4& boneWeights /*= Vector4( 1.0f, 0.0f, 0.0f, 0.0f ) */,
	const UIntVector4& boneIndices /*= UIntVector4( 0, 0, 0, 0 ) */ )
	: m_position( position )
	, m_color( color )
	, m_texCoords( texCoords )
	, m_texCoords2( texCoords2 )
	, m_tangent( tangent )
	, m_bitangent( bitangent )
	, m_normal( normal )
	, m_boneWeights( boneWeights )
	, m_boneIndices( boneIndices )
{
}


//-----------------------------------------------------------------------------------------------
bool operator==( const Vertex_Master& lhs, const Vertex_Master& rhs )
{
	if ( lhs.m_position != rhs.m_position )
	{
		return false;
	}

	if ( lhs.m_color != rhs.m_color )
	{
		return false;
	}

	if ( lhs.m_texCoords != rhs.m_texCoords )
	{
		return false;
	}

	if ( lhs.m_texCoords2 != rhs.m_texCoords2 )
	{
		return false;
	}

	if ( lhs.m_tangent != rhs.m_tangent )
	{
		return false;
	}

	if ( lhs.m_bitangent != rhs.m_bitangent )
	{
		return false;
	}

	if ( lhs.m_normal != rhs.m_normal )
	{
		return false;
	}

	return true;
}


//-----------------------------------------------------------------------------------------------
Vertex_SKINNED_MESH::Vertex_SKINNED_MESH( Vector3 const &position /*= Vector3::ZERO*/, 
	Rgba const &color /*= Rgba::WHITE*/, Vector2 const &texCoords /*= Vector2::ZERO*/, 
	Vector3 const &tangent /*= Vector3( 1.f, 0.f, 0.f )*/, 
	Vector3 const &bitangent /*= Vector3( 0.f, 0.f, 1.f )*/, 
	Vector4 boneWeights /*= Vector4( 1.f, 0.f, 0.f, 0.f )*/, 
	UIntVector4 boneIndices /*= UIntVector4( 0, 0, 0, 0 ) */ )
	: m_position( position )
	, m_texCoords( texCoords )
	, m_color( color )
	, m_tangent( tangent )
	, m_bitangent( bitangent )
	, m_boneWeights( boneWeights )
	, m_boneIndices( boneIndices )
{
}
