#pragma once

#include <vector>

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Meshes/Mesh.hpp"


//-----------------------------------------------------------------------------------------------
class Sprite;
class SpriteLayer;


//-----------------------------------------------------------------------------------------------
class SpriteGameRenderer
{
public:
	static SpriteGameRenderer* Instance();

 	void SetScreenResolution( unsigned int pixelWidth, unsigned int pixelHeight );

	// size in game units of our screen
	void SetVirtualSize( float virtualSize );

	// what resolution we're authoring at, standardized scale of sprite in virtual space
	void SetImportSize( float importSize ) { m_importSize = importSize; }

	void SetClearColor( const Rgba& color ) { m_clearColor = color; }

	void UpdateProjectionMatrix();

	void RegisterSprite( Sprite* sprite );
	void UnregisterSprite( Sprite* sprite );
	static void CopySpriteToMesh( Sprite* sprite );
	void RenderFrame() const;
	void RenderLayer( SpriteLayer* layer ) const;
	void RenderSprite( Sprite* sprite ) const;

	Material* GetDefaultMaterial() { return m_defaultMaterial; }
	float GetVirtualToImportRatio() { return ( m_virtualSize / m_importSize ); }
	float GetVirtualWidth() { return m_virtualWidth; }
	float GetVirtualHeight() { return m_virtualHeight; }
	float GetImportSize() { return m_importSize; }
	Rgba GetClearColor() { return m_clearColor; }

public:
	static Mesh* SpriteGameRenderer::s_mesh;

private:
	SpriteGameRenderer() {};
	SpriteLayer* CreateOrGetSpriteLayer( int layer );

private:
	std::vector< SpriteLayer* > m_spriteLayers;
	Material* m_defaultMaterial;
	float m_virtualWidth;
	float m_virtualHeight;
	float m_virtualSize;
	float m_importSize;
	float m_aspectRatio;
	IntVector2 m_windowSizePixels;
	Rgba m_clearColor;
	mat44_fl m_modelMatrix;
	mat44_fl m_viewMatrix;
	mat44_fl m_projectionMatrix;
};