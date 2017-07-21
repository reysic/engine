#pragma once

#include <string.h>
#include <map>

#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Sprites/SpriteSheet.hpp"


//-----------------------------------------------------------------------------------------------
class BitmapFont;


//-----------------------------------------------------------------------------------------------
typedef std::pair< size_t, BitmapFont* > StringToBitmapFontPair;
typedef std::map< size_t, BitmapFont*, std::less< size_t >, TUntrackedAllocator< StringToBitmapFontPair > > StringToBitmapFontMap;
typedef StringToBitmapFontMap::iterator StringToBitmapFontMapIter;
class BitmapFont
{
public:
	static BitmapFont* CreateOrGetFont( const std::string& bitmapFontName );
	const Texture& GetTexture() const;
	AABB2 GetTexCoordsForGlyph( int glyphAscii ) const;
	static float GetTextWidth( const std::string text, float textCellHeight );

	static void DestroyFontRegistry();

private:
	BitmapFont( const std::string& bitmapFontName, const SpriteSheet& glyphsSpriteSheet );

private:
	static StringToBitmapFontMap s_fontRegistry;
/*	static std::map< std::string, BitmapFont* > s_fontRegistry;*/

	std::string m_fontName;
	SpriteSheet m_glyphsSpriteSheet;
};