#include "Engine/Renderer/Particles/ParticleSystem.hpp"
#include "Engine/Core/StringUtils.hpp"


//-----------------------------------------------------------------------------------------------
#define STATIC // Do-nothing indicator that method/member is static in class definition


//-----------------------------------------------------------------------------------------------
/*STATIC std::map< std::string, ParticleSystem* >	ParticleSystem::s_particleSystemRegistry;*/
STATIC StringToParticleSystemMap ParticleSystem::s_particleSystemRegistry;


//-----------------------------------------------------------------------------------------------
ParticleSystem* ParticleSystem::CreateOrGetParticleSystem( const std::string& fontFilePath )
{
	// See if it already exists
	//std::string lowerCaseName = GetAsLowerCase( bitmapFontName );
	auto foundIter = s_particleSystemRegistry.find( std::hash< std::string >()( fontFilePath ) );
	if ( foundIter != s_particleSystemRegistry.end() )
		return foundIter->second;

	// Create and register it
	//std::string glyphsTexturePath = Stringf( "Data/Fonts/%s.png", bitmapFontName.c_str() );
	Texture* glyphsTexture = Texture::CreateOrGetTexture( fontFilePath.c_str() );
	if ( !glyphsTexture )
		return nullptr;

	SpriteSheet glyphsSpriteSheet( fontFilePath, 16, 16 );
	ParticleSystem* newFont = new ParticleSystem( "FontName", glyphsSpriteSheet );
	s_particleSystemRegistry[ std::hash< std::string >()( fontFilePath ) ] = newFont;
	return newFont;

}


//-----------------------------------------------------------------------------------------------
const Texture& ParticleSystem::GetTexture() const
{
	return m_glyphsSpriteSheet.GetTexture();
}


//-----------------------------------------------------------------------------------------------
AABB2 ParticleSystem::GetTexCoordsForGlyph( int glyphAscii ) const
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
STATIC void ParticleSystem::DestroyParticleSystemRegistry()
{
	for ( auto &p : s_particleSystemRegistry )
	{
		delete p.second;
		p.second = nullptr;
	}
}


//-----------------------------------------------------------------------------------------------
ParticleSystem::ParticleSystem( const std::string& bitmapFontName, const SpriteSheet& glyphsSpriteSheet )
{
	m_fontName = bitmapFontName;
	m_glyphsSpriteSheet = glyphsSpriteSheet;
}