#pragma once

#include "Engine/Renderer/Sprites/SpriteResource.hpp"


//-----------------------------------------------------------------------------------------------
class Sprite
{
public:
	Sprite( const std::string& name );
	~Sprite();

	void SetLayer( int layer );
	void Enable();
	void Disable();
	void SetMaterial( Material* material );
	const SpriteResource* GetSpriteResource() { return m_spriteResource; }

public:
	const SpriteResource* m_spriteResource;

	int m_orderingLayer;

	Vector2 m_position;
	Vector2 m_scale;
	float m_rotation;

	Rgba m_tint;

	bool m_enabled;
	
	Material* m_material;
};