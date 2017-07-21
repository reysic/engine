#pragma once

#include <string.h>
#include <map>

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Tools/Memory/UntrackedAllocator.hpp"


//-----------------------------------------------------------------------------------------------
class Texture;


//-----------------------------------------------------------------------------------------------
enum eTextureFormat
{
	eTextureFormat_RGBA8,	// RGBA, 8 Bits Per Channel
	eTextureFormat_D24S8,	// Depth 24, Stencil 8
};


//-----------------------------------------------------------------------------------------------
typedef std::pair< char*, Texture* > StringToTexturePair;
typedef std::map< char*, Texture*, std::less< char* >, TUntrackedAllocator< StringToTexturePair > > StringToTextureMap;
typedef StringToTextureMap::iterator StringToTextureMapIter;
class Texture
{
public:
	Texture( const char* imageFilePath );
	Texture( uint32_t width, uint32_t height, eTextureFormat format );
	static Texture* Texture::GetTextureByName( const char* imageFilePath ); // #Eiserloh: ditto below
	static Texture* Texture::CreateOrGetTexture( const char* imageFilePath );  // #Eiserloh: This function should not require that you have a texture in order to create a texture.  Therefore, it should be a static function - able to be called directly, without an instance of Texture

	int m_openglTextureID;
	IntVector2 m_texelSize;

	static StringToTextureMap Texture::s_textureRegistry;

/*	static std::map< std::string, Texture* >	Texture::s_textureRegistry;*/

	static void DestroyTextureRegistry();
};