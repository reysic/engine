#include "Engine/Renderer/Sprites/SpriteDatabase.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Sprites/SpriteGameRenderer.hpp"


//-----------------------------------------------------------------------------------------------
SpriteDatabase* g_spriteDatabase = nullptr;
std::map< std::string, SpriteResource* > SpriteDatabase::s_spriteRegistry;


//-----------------------------------------------------------------------------------------------
SpriteDatabase* SpriteDatabase::Instance()
{
	if ( g_spriteDatabase == nullptr )
	{
		g_spriteDatabase = new SpriteDatabase();
	}
	return g_spriteDatabase;
}


//-----------------------------------------------------------------------------------------------
const SpriteResource* SpriteDatabase::ReturnConstantSpriteResource( std::string name )
{
	std::map< std::string, SpriteResource* >::iterator registryIterator = s_spriteRegistry.find( name );
	if ( registryIterator == s_spriteRegistry.end() )
	{
		return nullptr;
	}
	else
	{
		return registryIterator->second;
	}
}


//-----------------------------------------------------------------------------------------------
SpriteResource* SpriteDatabase::ReturnEditableSpriteResource( std::string name )
{
	std::map< std::string, SpriteResource* >::iterator registryIterator = s_spriteRegistry.find( name );
	if ( registryIterator == s_spriteRegistry.end() )
	{
		return nullptr;
	}
	else
	{
		return registryIterator->second;
	}
}


//-----------------------------------------------------------------------------------------------
void SpriteDatabase::AddSpriteResource( std::string name, const char* texturePath, const AABB2& textureBounds )
{
	SpriteResource* resource = new SpriteResource();
	
	resource->m_id = name;
	resource->m_texture = Texture::CreateOrGetTexture( texturePath );
	resource->m_texBounds = textureBounds; // use non-default when not a single sprite
	int texWidth = resource->m_texture->m_texelSize.x;
	int texHeight = resource->m_texture->m_texelSize.y;
	float virtualToImportRatio = SpriteGameRenderer::Instance()->GetVirtualToImportRatio();
	resource->m_dimensions = Vector2( texWidth * virtualToImportRatio, texHeight * virtualToImportRatio );
	float pivotPointX = resource->m_dimensions.x / 2.0f;
	float pivotPointY = resource->m_dimensions.y / 2.0f;
	resource->m_pivotPoint = Vector2( pivotPointX, pivotPointY );
	resource->m_defaultMaterial = SpriteGameRenderer::Instance()->GetDefaultMaterial();

	s_spriteRegistry.insert( std::pair< std::string, SpriteResource* >( name, resource ) );
}
