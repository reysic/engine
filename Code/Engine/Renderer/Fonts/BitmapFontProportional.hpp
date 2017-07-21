#pragma once

#include <string>
#include <vector>

#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/AABB2.hpp"


//-----------------------------------------------------------------------------------------------
struct Glyph
{
	int m_charId;
	int m_xPos;
	int m_yPos;
	int m_width;
	int m_height;
	int m_xOffset;
	int m_yOffset;
	int m_xAdvance;
};


//-----------------------------------------------------------------------------------------------
class BitmapFontProportional;

//-----------------------------------------------------------------------------------------------
typedef std::pair< size_t, BitmapFontProportional* > StringToBitmapFontProportionalPair;
typedef std::map< size_t, BitmapFontProportional*, std::less< size_t >, TUntrackedAllocator< StringToBitmapFontProportionalPair > > StringToBitmapFontProportionalMap;
typedef StringToBitmapFontProportionalMap::iterator StringToBitmapFontMapProportionalIter;
class BitmapFontProportional
{
public:
	BitmapFontProportional( const std::string& fontName );
	static BitmapFontProportional* CreateOrGetFont( const std::string& fontName );
	Texture* BitmapFontProportional::GetTexture() const;
	Glyph* GetGlyph( const char c );
	const bool GetKerning( const int first, const int second, int& out_val ) const;
	bool ReadMetaFile( const std::string& metaFileName );
	float CalcTextWidth( const std::string& asciiText, const float scale );

public:
	Texture* m_fontTexture;
	int m_textureHeight;
	int m_textureWidth;
	std::map<int, Glyph*> m_glyphRegistry;
	std::map<std::pair<int, int>, int> m_kerningRegistry;
	static StringToBitmapFontProportionalMap s_fontRegistry;
	/*static std::map< std::string, BitmapFontProportional* > s_fontRegistry;*/
	bool m_hasKerning;
	
	static void DestroyFontRegistry();
};