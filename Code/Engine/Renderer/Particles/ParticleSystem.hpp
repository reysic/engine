#pragma once

#include <string.h>
#include <map>

#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Sprites/SpriteSheet.hpp"


//-----------------------------------------------------------------------------------------------
class ParticleSystem;


//-----------------------------------------------------------------------------------------------
typedef std::pair< size_t, ParticleSystem* > StringToParticleSystemPair;
typedef std::map< size_t, ParticleSystem*, std::less< size_t >, TUntrackedAllocator< StringToParticleSystemPair > > StringToParticleSystemMap;
typedef StringToParticleSystemMap::iterator StringToParticleSystemMapIter;
class ParticleSystem
{
public:
	static ParticleSystem* CreateOrGetParticleSystem( const std::string& bitmapFontName );
	const Texture& GetTexture() const;
	AABB2 GetTexCoordsForGlyph( int glyphAscii ) const;

	static void DestroyParticleSystemRegistry();

private:
	ParticleSystem( const std::string& bitmapFontName, const SpriteSheet& glyphsSpriteSheet );

private:
	static StringToParticleSystemMap s_particleSystemRegistry;
	/*	static std::map< std::string, ParticleSystem* > s_fontRegistry;*/

	std::string m_fontName;
	SpriteSheet m_glyphsSpriteSheet;
};