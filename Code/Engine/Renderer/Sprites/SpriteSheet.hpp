#pragma once

#include <string.h>

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Texture.hpp"


//-----------------------------------------------------------------------------------------------
class SpriteSheet
{
public:
	SpriteSheet();
	SpriteSheet( const std::string& imageFilePath, int tilesWide, int tilesHigh );
	AABB2 GetTexCoordsForSpriteCoords( int spriteX, int spriteY ) const; // mostly for atlases
	AABB2 GetTexCoordsForSpriteIndex( int spriteIndex ) const; // mostly for sprite animations
	int GetNumSprites() const;
	Texture& GetTexture() const;
	//... and other member functions as needed

private:
	Texture* 	m_spriteSheetTexture;	// Image with grid-based layout of sub-images
	IntVector2	m_spriteLayout;		// # of sprites across, and down, on the sheet
	Vector2		m_texCoordsPerTile;
	//... and other data member variables as needed
};