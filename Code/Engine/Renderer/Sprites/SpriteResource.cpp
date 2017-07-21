#include "Engine/Renderer/Sprites/SpriteResource.hpp"


//-----------------------------------------------------------------------------------------------
SpriteResource::SpriteResource()
	: m_id( "Invalid sprite resource" )
	, m_texture( nullptr )
	, m_texBounds( AABB2( Vector2( 0.0f, 0.0f ), Vector2( 1.0f, 1.0f ) ) )
	, m_dimensions( Vector2( 1.0f, 1.0f ) )
	, m_pivotPoint( 0.5f, 0.5f )
	, m_defaultMaterial( nullptr )
{
}
