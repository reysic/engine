#include "Engine/Renderer/Fonts/BitmapFont.hpp"
#include "Engine/Core/StringUtils.hpp"


//-----------------------------------------------------------------------------------------------
#define STATIC // Do-nothing indicator that method/member is static in class definition


//-----------------------------------------------------------------------------------------------
/*STATIC std::map< std::string, BitmapFont* >	BitmapFont::s_fontRegistry;*/
STATIC StringToBitmapFontMap BitmapFont::s_fontRegistry;


//-----------------------------------------------------------------------------------------------
BitmapFont* BitmapFont::CreateOrGetFont( const std::string& fontFilePath )
{
	// See if it already exists
	//std::string lowerCaseName = GetAsLowerCase( bitmapFontName );
	auto foundIter = s_fontRegistry.find( std::hash< std::string >()( fontFilePath ) );
	if ( foundIter != s_fontRegistry.end() )
		return foundIter->second;

	// Create and register it
	//std::string glyphsTexturePath = Stringf( "Data/Fonts/%s.png", bitmapFontName.c_str() );
	Texture* glyphsTexture = Texture::CreateOrGetTexture( fontFilePath.c_str() );
	if ( !glyphsTexture )
		return nullptr;

	SpriteSheet glyphsSpriteSheet( fontFilePath, 16, 16 );
	BitmapFont* newFont = new BitmapFont( "FontName", glyphsSpriteSheet );
	s_fontRegistry[ std::hash< std::string >()( fontFilePath ) ] = newFont;
	return newFont;

}


//-----------------------------------------------------------------------------------------------
const Texture& BitmapFont::GetTexture() const
{
	return m_glyphsSpriteSheet.GetTexture();
}


//-----------------------------------------------------------------------------------------------
AABB2 BitmapFont::GetTexCoordsForGlyph( int glyphAscii ) const
{
	int spriteX = glyphAscii % 16;
	int spriteY = glyphAscii / 16;
	AABB2 texCoords;
	texCoords.mins.x = ( 1.0f / ( float ) 16 ) * ( float ) spriteX;
	texCoords.mins.y = ( 1.0f / ( float ) 16 ) * ( float ) spriteY;
	texCoords.maxs = texCoords.mins + Vector2( 1.0f / 16.0f, 1.0f / 16.0f );
	return texCoords;
}


//-----------------------------------------------------------------------------------------------
float BitmapFont::GetTextWidth( const std::string text, float textCellHeight )
{
	return ( text.length() * textCellHeight );
}


//-----------------------------------------------------------------------------------------------
STATIC void BitmapFont::DestroyFontRegistry()
{
	for ( auto &p : s_fontRegistry )
	{
		delete p.second;
		p.second = nullptr;
	}
}


//-----------------------------------------------------------------------------------------------
BitmapFont::BitmapFont( const std::string& bitmapFontName, const SpriteSheet& glyphsSpriteSheet )
{
	m_fontName = bitmapFontName;
	m_glyphsSpriteSheet = glyphsSpriteSheet;
}