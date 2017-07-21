#pragma once

#include <map>

#include "Engine/Renderer/Sprites/SpriteResource.hpp"


//-----------------------------------------------------------------------------------------------
class SpriteDatabase
{
public:
	static SpriteDatabase* Instance();
	const SpriteResource* ReturnConstantSpriteResource( std::string name );
	SpriteResource* ReturnEditableSpriteResource( std::string name );
	void AddSpriteResource( std::string name, const char* texturePath, const AABB2& textureBounds );

private:
	SpriteDatabase() {};

public:
	static std::map< std::string, SpriteResource* > SpriteDatabase::s_spriteRegistry;
};