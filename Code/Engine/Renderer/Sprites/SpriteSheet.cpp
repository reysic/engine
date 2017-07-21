#include "Engine/Renderer/Sprites/SpriteSheet.hpp"


//-----------------------------------------------------------------------------------------------
SpriteSheet::SpriteSheet()
{
}


//-----------------------------------------------------------------------------------------------
SpriteSheet::SpriteSheet( const std::string& imageFilePath, int tilesWide, int tilesHigh )
{
	m_spriteSheetTexture = Texture::CreateOrGetTexture( imageFilePath.c_str() );
	m_spriteLayout = IntVector2( tilesWide, tilesHigh );
	m_texCoordsPerTile.x = 1.0f / ( float ) tilesWide;
	m_texCoordsPerTile.y = 1.0f / ( float ) tilesHigh;
}


//-----------------------------------------------------------------------------------------------
AABB2 SpriteSheet::GetTexCoordsForSpriteCoords( int spriteX, int spriteY ) const
{
	AABB2 texCoords;
	texCoords.mins.x = m_texCoordsPerTile.x * ( float ) spriteX;
	texCoords.mins.y = m_texCoordsPerTile.y * ( float ) spriteY;
	texCoords.maxs = texCoords.mins + m_texCoordsPerTile;
	return texCoords;
}


//-----------------------------------------------------------------------------------------------
AABB2 SpriteSheet::GetTexCoordsForSpriteIndex( int spriteIndex ) const
{
	int spriteX = spriteIndex % m_spriteLayout.x;
	int spriteY = spriteIndex / m_spriteLayout.x;
	return GetTexCoordsForSpriteCoords( spriteX, spriteY );
}


//-----------------------------------------------------------------------------------------------
int SpriteSheet::GetNumSprites() const
{
	return ( m_spriteLayout.x * m_spriteLayout.y );
}


//-----------------------------------------------------------------------------------------------
Texture& SpriteSheet::GetTexture() const
{
	return *m_spriteSheetTexture;
}