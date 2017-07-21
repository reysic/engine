#include "Engine/Renderer/Sprites/SpriteLayer.hpp"


//-----------------------------------------------------------------------------------------------
SpriteLayer::SpriteLayer( int id )
	: m_id( id )
	, m_enabled( true )
{
}


//-----------------------------------------------------------------------------------------------
void SpriteLayer::AddSprite( Sprite* sprite )
{
	m_sprites.insert( sprite );
}


//-----------------------------------------------------------------------------------------------
void SpriteLayer::RemoveSprite( Sprite* sprite )
{
	m_sprites.erase( sprite );
}