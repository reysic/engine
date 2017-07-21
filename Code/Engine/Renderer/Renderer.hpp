#pragma once

#include <string>
#include <vector>
#include <list>
#include <cmath>

#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Matrix4x4.hpp"
#include "Engine/Math/Matrix4x4c.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Fonts/BitmapFont.hpp"
#include "Engine/Renderer/Fonts/BitmapFontProportional.hpp"
#include "Engine/Renderer/Vertices/Vertex.hpp"
#include "Engine/Renderer/OpenGLExtensions.hpp"
#include "Engine/Renderer/RenderCommand.hpp"
#include "Engine/Renderer/Cameras/Camera.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Meshes/Mesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Framebuffer.hpp"


//-----------------------------------------------------------------------------------------------
enum PrimitiveType
{
	PRIMITIVE_POINTS,
	PRIMITIVE_LINES,
	PRIMITIVE_LINE_LOOP,
	PRIMITIVE_TRIANGLES,
	PRIMITIVE_TRIANGLE_FAN,
	PRIMITIVE_QUADS,
	NUM_PRIMITIVE_TYPES
};


//-----------------------------------------------------------------------------------------------
class Renderer;
class ShaderProgram;


//-----------------------------------------------------------------------------------------------
extern Renderer* g_theRenderer;


//-----------------------------------------------------------------------------------------------
class Renderer
{
public:
	Renderer();
	~Renderer();
	void SetGLFunctionPointers();
	void MakeScreenQuad();
	void PushView() const;
	void PopView() const;
	void RotateView( float degrees, const Vector3& rotation );
	void TranslateView( const Vector3& translation ) const;
	void DrawText2D( const Vector2& startBottomLeft, const std::string& asciiText, float cellHeight, const Rgba& tint, const BitmapFont* font );
	void DrawTextProportional2D( const Vector3& position, const std::string& asciiText, float scale, const Rgba& color, BitmapFontProportional* font = nullptr, const Vector3& up = Vector3( 0.0f, 1.0f, 0.0f ), const Vector3& right = Vector3( 1.0f, 0.0f, 0.0f ) );
	void DrawText3D( const Vector3& startBottomLeft, const std::string& asciiText, float cellHeight, const Rgba& tint, const BitmapFont* font );
	void SetOrtho( const Vector2& bottomLeft, const Vector2& topRight );
	void SetPerspective( float fovDegreesY, float aspect, float nearDist, float farDist );
	void ClearBuffer();
	void ClearScreen( const Rgba& clearColor );
	void DrawQuad( const Vector2& bottomLeft, const Vector2& topRight, const Rgba& tint );
	void DrawQuad3D( const Vector3& bottomLeft, const Vector3& topRight, const Rgba& tint );
	void DrawTexturedQuad3D( const Vector3& bottomLeft, const Vector3& bottomRight, const Vector3& topRight, const Vector3& topLeft, const Texture& texture, const Vector2& texCoordMins, const Vector2& texCoordMaxs, const Rgba& tint );
	void DrawTexturedAABB2( const AABB2& bounds, const Texture& texture, const Vector2& texCoordMins, const Vector2& texCoordMaxs, const Rgba& tint );
	void DrawDebugAxisLines();
	void DrawCrosshairs( const Vector2& screenPosition, const Rgba& tint );
	void SetActiveTextureTo0() const;
	void SetLineWidth( float lineWidth ) const;
	void DepthTestingEnabled( bool enabled ) const;
	void BackFaceCullingEnabled( bool enabled ) const;
	void AlphaTestingEnabled( bool enabled ) const;
	void ZWriteEnabled( bool enabled ) const;
	void BindTexture( const Texture& texture );
	void DrawVertexArray_PCT( const Vertex_PCT* verts, int numVerts, PrimitiveType primitiveType );
	void DrawSquareLineLoop3D( const Vector3& pos1, const Vector3& pos2, const Vector3& pos3, const Vector3& pos4, const Rgba& tint );
	void DrawSquareLineLoop3DXray( const Vector3& pos1, const Vector3& pos2, const Vector3& pos3, const Vector3& pos4, const Rgba& tint );
	void DrawPlayerRaycastLine3DXray( const Vector3& pos1, const Vector3& pos2, const Rgba& tint );
	void DrawVBO_PCT( unsigned int vboID, int numVerts, PrimitiveType primitiveType );
	void BindBuffer( unsigned int vboID );
	void BufferData( GLsizeiptr size, const GLvoid* data );
	void DrawPoint3D( const Vector3& position, const Rgba& tint );
	void DrawLine3D( const Vector3& startPosition, const Vector3& endPosition, const Rgba& tint, float lineWidth );
	void DrawAABB3( const Vector3& mins, const Vector3& maxs, const Rgba& edgeDrawColor, const Rgba& faceDrawColor );
	void DebugDrawPoint( const Vector3& position, const Rgba& drawColor = Rgba::WHITE, float drawDurationSeconds = -1.0f, DebugDrawMode = DRAW_MODE_DEPTH_TESTING_ON );
	void DebugDrawLine( const Vector3& startPosition, const Vector3& endPosition, const Rgba& drawColor = Rgba::WHITE, float drawDurationSeconds = -1.0f, DebugDrawMode = DRAW_MODE_DEPTH_TESTING_ON );
	void DebugDrawArrow( const Vector3& startPosition, const Vector3& endPosition, const Rgba& drawColor = Rgba::WHITE, float drawDurationSeconds = -1.0f, DebugDrawMode = DRAW_MODE_DEPTH_TESTING_ON );
	void DebugDrawAABB3( const Vector3& mins, const Vector3& maxs, const Rgba& edgeDrawColor = Rgba::WHITE, const Rgba& faceDrawColor = Rgba::BLUE, float drawDurationSeconds = -1.0f, DebugDrawMode = DRAW_MODE_DEPTH_TESTING_ON );
	void DebugDrawSphere( const Vector3& position, float radius, const Rgba& drawColor = Rgba::WHITE, float drawDurationSeconds = -1.0f, DebugDrawMode = DRAW_MODE_DEPTH_TESTING_ON );
	void DrawDebugRenderCommands( float deltaSeconds );
	void DrawSphere( const Vector3& position, float radius, const Rgba& tint );
	void DrawGrid();
	void SetLineWidth( float lineWidth );
	void SetDrawColor( const Rgba& color );
	void DrawFilledPolygon( int numSides, float centerX, float centerY, float radius );
	void DrawHollowPolygon( int numSides, float centerX, float centerY, float radius );
	void DrawFilledPolygon( int numSides, float centerX, float centerY, float radius, const Rgba& drawColor = Rgba::WHITE );
	void DrawHollowPolygon( int numSides, float centerX, float centerY, float radius, const Rgba& drawColor = Rgba::WHITE );
	void ScaleView( float uniformScale );
	void EnableGLAlphaBlending();
	void EnableGLAdditiveBlending();

	// New for SD3
	void RenderCleanup();
	void RenderBufferDestroy( GLuint buffer );
	void FramebufferBind( Framebuffer* fbo );
	void PostProcess( Framebuffer* fbo );
	void CreateMaterials();

	// Is really only used for debug purposes
	// You will probably never use it after this lesson
	void FramebufferCopyToBack( Framebuffer* fbo );

public:
	// New for SD3
	std::list< RenderCommand* > m_renderCommands;
	Matrix4x4 m_view;
	Matrix4x4 m_model;
	Matrix4x4 m_projection;
	Mesh* m_screenQuad;
	Material* m_fboMaterial0;
	Material* m_fboMaterial1;
	Material* m_fboMaterial2;
	Material* m_fboMaterial3;
	Material* m_fboMaterial4;
	Material* m_fboMaterial5;
	Material* m_fboMaterial6;
	Material* m_fboToDrawWith;
	Framebuffer* m_fbo;
	Framebuffer* m_activeFBO;
};