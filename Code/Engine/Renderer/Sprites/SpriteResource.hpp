#pragma once

#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Material.hpp"


//-----------------------------------------------------------------------------------------------
class SpriteResource
{
public:
	SpriteResource();
	SpriteResource( const SpriteResource& spriteResource ) = delete;

public:
	std::string m_id;

	Texture* m_texture;
	AABB2 m_texBounds;
	
	Vector2 m_dimensions;
	
	Vector2 m_pivotPoint;

	Material* m_defaultMaterial;
};