#pragma once

#include <set>

#include "Engine/Renderer/Sprites/Sprite.hpp"


//-----------------------------------------------------------------------------------------------
class SpriteLayer
{
public:
	SpriteLayer( int id );

	void AddSprite( Sprite* sprite );
	void RemoveSprite( Sprite* sprite );
	int GetLayerID() const { return m_id; }
	bool IsLayerEnabled() const { return m_enabled; }

public:
	std::set< Sprite* > m_sprites;

private:
	bool m_enabled;
	int m_id;
};