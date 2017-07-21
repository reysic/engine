#include "Engine/Renderer/Sprites/SpriteGameRenderer.hpp"
#include "Engine/Renderer/Sprites/Sprite.hpp"
#include "Engine/Renderer/Sprites/SpriteLayer.hpp"
#include "Engine/Math/Matrix4x4.hpp"


#define STATIC // Do-nothing indicator that method/member is static in class definition


//-----------------------------------------------------------------------------------------------
SpriteGameRenderer* g_spriteGameRenderer = nullptr;
STATIC Mesh* SpriteGameRenderer::s_mesh;


//-----------------------------------------------------------------------------------------------
SpriteGameRenderer* SpriteGameRenderer::Instance()
{
	if ( g_spriteGameRenderer == nullptr )
	{
		g_spriteGameRenderer = new SpriteGameRenderer();
		g_spriteGameRenderer->SetScreenResolution( 1600, 900 );
		g_spriteGameRenderer->SetVirtualSize( 10.0f );
		g_spriteGameRenderer->SetImportSize( 240.0f );
		g_spriteGameRenderer->m_clearColor = Rgba::BLACK;
		g_spriteGameRenderer->m_defaultMaterial = new Material( "../../../Engine/Code/Engine/Shaders/basic_sprite.vert",
			"../../../Engine/Code/Engine/Shaders/basic_sprite.frag" );
		glUseProgram( NULL );
		g_spriteGameRenderer->s_mesh = new Mesh();
		g_spriteGameRenderer->s_mesh->MakeQuadMesh();
		g_spriteGameRenderer->m_modelMatrix = mat44_fl::identity;
		g_spriteGameRenderer->m_viewMatrix = mat44_fl::identity;
		g_spriteGameRenderer->m_projectionMatrix = mat44_fl::identity;

		g_spriteGameRenderer->UpdateProjectionMatrix();
	}
	return g_spriteGameRenderer;
}


//-----------------------------------------------------------------------------------------------
void SpriteGameRenderer::SetScreenResolution( unsigned int pixelWidth, unsigned int pixelHeight )
{
	m_windowSizePixels = IntVector2( pixelWidth, pixelHeight );
	m_aspectRatio = ( float ) pixelWidth / ( float ) pixelHeight;
}


//-----------------------------------------------------------------------------------------------
void SpriteGameRenderer::SetVirtualSize( float virtualSize )
{
	m_virtualSize = virtualSize;

	if ( m_aspectRatio < 1.0f )
	{
		// aspect ratio < 1.0f
		m_virtualHeight = virtualSize / m_aspectRatio;
		m_virtualWidth = virtualSize;
	}
	else
	{
		// aspect ratio >= 1.0f
		m_virtualWidth = virtualSize * m_aspectRatio;
		m_virtualHeight = virtualSize;
	}
}


//-----------------------------------------------------------------------------------------------
// Called whenever the resolution or projection matrix changes
void SpriteGameRenderer::UpdateProjectionMatrix()
{
	MatrixMakeProjOrthogonal( &m_projectionMatrix, m_virtualWidth, m_virtualHeight, -1.0f, 1.0f );
}


//-----------------------------------------------------------------------------------------------
void SpriteGameRenderer::RegisterSprite( Sprite* sprite )
{
	if ( sprite == NULL )
	{
		return;
	}

	SpriteLayer* layer = CreateOrGetSpriteLayer( sprite->m_orderingLayer );

	layer->AddSprite( sprite );
}


//-----------------------------------------------------------------------------------------------
void SpriteGameRenderer::UnregisterSprite( Sprite* sprite )
{
	if ( sprite == NULL )
	{
		return;
	}

	SpriteLayer* layer = CreateOrGetSpriteLayer( sprite->m_orderingLayer );

	layer->RemoveSprite( sprite );
}


//-----------------------------------------------------------------------------------------------
STATIC void SpriteGameRenderer::CopySpriteToMesh( Sprite* sprite )
{
	std::vector< Vertex_PCT > spriteVerts;
	Vector2 dimensions = sprite->GetSpriteResource()->m_dimensions;

	Vector2 v0 = Vector2( -sprite->GetSpriteResource()->m_pivotPoint.x, 
		-sprite->GetSpriteResource()->m_pivotPoint.y );
	Vector2 v1 = Vector2( dimensions.x - sprite->GetSpriteResource()->m_pivotPoint.x,
		-sprite->GetSpriteResource()->m_pivotPoint.y );
	Vector2 v2 = Vector2( dimensions.x - sprite->GetSpriteResource()->m_pivotPoint.x,
		dimensions.y - sprite->GetSpriteResource()->m_pivotPoint.y );
	Vector2 v3 = Vector2( -sprite->GetSpriteResource()->m_pivotPoint.x,
		dimensions.y - sprite->GetSpriteResource()->m_pivotPoint.y );

	float scaleX = sprite->m_scale.x;
	float scaleY = sprite->m_scale.y;
	float sinDegrees = SinDegrees( sprite->m_rotation );
	float cosDegrees = CosDegrees( sprite->m_rotation );

	Vector2 v0Prime = Vector2( v0.x * scaleX * cosDegrees + v0.y * scaleY * sinDegrees,
		-v0.x * scaleX * sinDegrees + v0.y * scaleY * cosDegrees ) + sprite->m_position;
	Vector2 v1Prime = Vector2( v1.x * scaleX * cosDegrees + v1.y * scaleY * sinDegrees,
		-v1.x * scaleX * sinDegrees + v1.y * scaleY * cosDegrees ) + sprite->m_position;
	Vector2 v2Prime = Vector2( v2.x * scaleX * cosDegrees + v2.y * scaleY * sinDegrees,
		-v2.x * scaleX * sinDegrees + v2.y * scaleY * cosDegrees ) + sprite->m_position;
	Vector2 v3Prime = Vector2( v3.x * scaleX * cosDegrees + v3.y * scaleY * sinDegrees,
		-v3.x * scaleX * sinDegrees + v3.y * scaleY * cosDegrees ) + sprite->m_position;

	Vector2 bottomLeft = sprite->GetSpriteResource()->m_texBounds.mins;
	Vector2 topRight = sprite->GetSpriteResource()->m_texBounds.maxs;
	Vector2 v0TexCoord = Vector2( bottomLeft.x, bottomLeft.y );
	Vector2 v1TexCoord = Vector2( topRight.x, bottomLeft.y );
	Vector2 v2TexCoord = Vector2( topRight.x, topRight.y );
	Vector2 v3TexCoord = Vector2( bottomLeft.x, topRight.y );

	spriteVerts.push_back( Vertex_PCT( Vector3( v0Prime.x, v0Prime.y, 0.0f ), Rgba::WHITE, v0TexCoord ) );
	spriteVerts.push_back( Vertex_PCT( Vector3( v1Prime.x, v1Prime.y, 0.0f ), Rgba::WHITE, v1TexCoord ) );
	spriteVerts.push_back( Vertex_PCT( Vector3( v2Prime.x, v2Prime.y, 0.0f ), Rgba::WHITE, v2TexCoord ) );
	spriteVerts.push_back( Vertex_PCT( Vector3( v3Prime.x, v3Prime.y, 0.0f ), Rgba::WHITE, v3TexCoord ) );

	// Copy vertices into mesh
	s_mesh->m_verts.clear();
	for ( Vertex_PCT vert : spriteVerts )
	{
		s_mesh->m_verts.push_back( vert );
	}
}


//-----------------------------------------------------------------------------------------------
void SpriteGameRenderer::RenderFrame() const
{
	g_theRenderer->ClearScreen( m_clearColor );

	for ( SpriteLayer* layer : m_spriteLayers )
	{
		RenderLayer( layer );
	}

	// Swap buffers done in Main_Win32.cpp
}


//-----------------------------------------------------------------------------------------------
void SpriteGameRenderer::RenderLayer( SpriteLayer* layer ) const
{
	for ( std::set< Sprite* >::iterator spriteIterator = layer->m_sprites.begin();
			spriteIterator != layer->m_sprites.end(); ++spriteIterator )
	{
		RenderSprite( *spriteIterator );
	}
}


//-----------------------------------------------------------------------------------------------
void SpriteGameRenderer::RenderSprite( Sprite* sprite ) const
{
	CopySpriteToMesh( sprite );
	sprite->m_material->SetUniform( "gTexDiffuse", sprite->m_spriteResource->m_texture );
	s_mesh->DrawWithMaterial( sprite->m_material, PRIMITIVE_LINE_LOOP );
}


//-----------------------------------------------------------------------------------------------
SpriteLayer* SpriteGameRenderer::CreateOrGetSpriteLayer( int layerID )
{
	for ( SpriteLayer* layer : m_spriteLayers )
	{
		if ( layer->GetLayerID() == layerID )
		{
			return layer;
		}
	}

	SpriteLayer* newLayer = new SpriteLayer( layerID );
	m_spriteLayers.push_back( newLayer );

	return newLayer;
}