#include "Engine/Renderer/Shaders/Attribute.hpp"
#include "Engine/Renderer/Vertices/Vertex.hpp"


//-----------------------------------------------------------------------------------------------
Attribute::Attribute()
	: m_location( -1 )
	, m_name( "" )
	, m_type( GL_INT )
{
}


//-----------------------------------------------------------------------------------------------
bool Attribute::BindAttribute()
{
	return true;
}


//-----------------------------------------------------------------------------------------------
AttributeVec2::AttributeVec2()
	: m_value( Vector2( 0.f, 0.f ) )
{
}


//-----------------------------------------------------------------------------------------------
bool AttributeVec2::BindAttribute()
{
	if ( m_location >= 0 )
	{
		glEnableVertexAttribArray( m_location );
		glVertexAttribPointer( m_location, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex_PCUTB ), ( GLvoid* ) offsetof( Vertex_PCUTB, m_texCoords ) );
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
AttributeVec3::AttributeVec3()
	: m_value( Vector3::ZERO )
{
}


//-----------------------------------------------------------------------------------------------
bool AttributeVec3::BindAttribute()
{
	if ( m_location >= 0 )
	{
		glEnableVertexAttribArray( m_location );
		glVertexAttribPointer( m_location, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex_PCUTB ), ( GLvoid* ) offsetof( Vertex_PCUTB, m_position ) );
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
AttributeVec4::AttributeVec4()
	: m_value( Vector4::ZERO )
{
}


//-----------------------------------------------------------------------------------------------
bool AttributeVec4::BindAttribute()
{
	if ( m_location >= 0 )
	{
		glEnableVertexAttribArray( m_location );
		glVertexAttribPointer( m_location, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof( Vertex_PCUTB ), ( GLvoid* ) offsetof( Vertex_PCUTB, m_color ) );
	}

	return false;
}