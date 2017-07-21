#include "Engine/Renderer/Sprites/Sprite.hpp"
#include "Engine/Renderer/Sprites/SpriteGameRenderer.hpp"
#include "Engine/Renderer/Sprites/SpriteDatabase.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


//-----------------------------------------------------------------------------------------------
Sprite::Sprite(const std::string& name)
	: m_position( Vector2::ZERO)
	, m_scale( Vector2::ONE )
	, m_rotation( 0.0f )
	, m_orderingLayer( 0 )
	, m_tint( Rgba::WHITE )
	, m_enabled( false )
{
	m_spriteResource = SpriteDatabase::Instance()->ReturnConstantSpriteResource( name );
	ASSERT_OR_DIE( m_spriteResource != NULL, "m_spriteResource = NULL" );
	m_material = m_spriteResource->m_defaultMaterial;
}


//-----------------------------------------------------------------------------------------------
Sprite::~Sprite()
{
	SpriteGameRenderer::Instance()->UnregisterSprite( this );
}


//-----------------------------------------------------------------------------------------------
void Sprite::SetLayer( int layer )
{
	if ( m_enabled )
	{
		SpriteGameRenderer::Instance()->UnregisterSprite( this );
	}

	m_orderingLayer = layer;

	if ( m_enabled )
	{
		SpriteGameRenderer::Instance()->RegisterSprite( this );
	}
}


//-----------------------------------------------------------------------------------------------
void Sprite::Enable()
{
	if ( !m_enabled )
	{
		SpriteGameRenderer::Instance()->RegisterSprite( this );
		m_enabled = true;
	}
}


//-----------------------------------------------------------------------------------------------
void Sprite::Disable()
{
	if ( m_enabled )
	{
		SpriteGameRenderer::Instance()->UnregisterSprite( this );
		m_enabled = false;
	}
}


//-----------------------------------------------------------------------------------------------
void Sprite::SetMaterial( Material* material )
{
	m_material = material;
}