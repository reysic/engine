#include <direct.h>
#include <sstream>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Shaders/ShaderProgram.hpp"
#include "Engine/Renderer/Lights/Light.hpp"

#pragma comment( lib, "opengl32" ) // Link in the OpenGL32.lib static library
#pragma comment( lib, "GLu32" ) // Link in the GLu32.lib static library


//-----------------------------------------------------------------------------------------------
Renderer* g_theRenderer = nullptr;


//-----------------------------------------------------------------------------------------------
Renderer::Renderer()
	: m_activeFBO( NULL )
{
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable( GL_LINE_SMOOTH );

	SetGLFunctionPointers();

	LoggerPrintfWithCallstack( CallstackFetch( 2 ), "Renderer initialized.\n" );
}


//-----------------------------------------------------------------------------------------------
Renderer::~Renderer()
{
	Texture::DestroyTextureRegistry();

	BitmapFont::DestroyFontRegistry();

	BitmapFontProportional::DestroyFontRegistry();

	//LoggerPrintfWithCallstack( CallstackFetch( 2 ), "Renderer de-initialized.\n" );
}


//-----------------------------------------------------------------------------------------------
void Renderer::SetGLFunctionPointers()
{
	glGenBuffers = ( PFNGLGENBUFFERSPROC ) wglGetProcAddress( "glGenBuffers" );
	glBindBuffer = ( PFNGLBINDBUFFERPROC ) wglGetProcAddress( "glBindBuffer" );
	glBufferData = ( PFNGLBUFFERDATAPROC ) wglGetProcAddress( "glBufferData" );
	glGenerateMipmap = ( PFNGLGENERATEMIPMAPPROC ) wglGetProcAddress( "glGenerateMipmap" );
	glDeleteBuffers = ( PFNGLDELETEBUFFERSPROC ) wglGetProcAddress( "glDeleteBuffers" );
	glCreateShader = ( PFNGLCREATESHADERPROC ) wglGetProcAddress( "glCreateShader" );
	glShaderSource = ( PFNGLSHADERSOURCEPROC ) wglGetProcAddress( "glShaderSource" );
	glCompileShader = ( PFNGLCOMPILESHADERPROC ) wglGetProcAddress( "glCompileShader" );
	glGetShaderiv = ( PFNGLGETSHADERIVPROC ) wglGetProcAddress( "glGetShaderiv" );
	glDeleteShader = ( PFNGLDELETESHADERPROC ) wglGetProcAddress( "glDeleteShader" );
	glGetShaderInfoLog = ( PFNGLGETSHADERINFOLOGPROC ) wglGetProcAddress( "glGetShaderInfoLog" );
	glCreateProgram = ( PFNGLCREATEPROGRAMPROC ) wglGetProcAddress( "glCreateProgram" );
	glAttachShader = ( PFNGLATTACHSHADERPROC ) wglGetProcAddress( "glAttachShader" );
	glLinkProgram = ( PFNGLLINKPROGRAMPROC ) wglGetProcAddress( "glLinkProgram" );
	glGetProgramiv = ( PFNGLGETPROGRAMIVPROC ) wglGetProcAddress( "glGetProgramiv" );
	glDeleteProgram = ( PFNGLDELETEPROGRAMPROC ) wglGetProcAddress( "glDeleteProgram" );
	glDetachShader = ( PFNGLDETACHSHADERPROC ) wglGetProcAddress( "glDetachShader" );
	glGetProgramInfoLog = ( PFNGLGETPROGRAMINFOLOGPROC ) wglGetProcAddress( "glGetProgramInfoLog" );
	glGenVertexArrays = ( PFNGLGENVERTEXARRAYSPROC ) wglGetProcAddress( "glGenVertexArrays" );
	glDeleteVertexArrays = ( PFNGLDELETEVERTEXARRAYSPROC ) wglGetProcAddress( "glDeleteVertexArrays" );
	glBindVertexArray = ( PFNGLBINDVERTEXARRAYPROC ) wglGetProcAddress( "glBindVertexArray" );
	glGetActiveAttrib = ( PFNGLGETACTIVEATTRIBPROC ) wglGetProcAddress( "glGetActiveAttrib" );
	glGetAttribLocation = ( PFNGLGETATTRIBLOCATIONPROC ) wglGetProcAddress( "glGetAttribLocation" );
	glEnableVertexAttribArray = ( PFNGLENABLEVERTEXATTRIBARRAYPROC ) wglGetProcAddress( "glEnableVertexAttribArray" );
	glVertexAttribPointer = ( PFNGLVERTEXATTRIBPOINTERPROC ) wglGetProcAddress( "glVertexAttribPointer" );
	glUseProgram = ( PFNGLUSEPROGRAMPROC ) wglGetProcAddress( "glUseProgram" );
	glGetActiveUniform = ( PFNGLGETACTIVEUNIFORMPROC ) wglGetProcAddress( "glGetActiveUniform" );
	glGetUniformLocation = ( PFNGLGETUNIFORMLOCATIONPROC ) wglGetProcAddress( "glGetUniformLocation" );
	glUniform1fv = ( PFNGLUNIFORM1FVPROC ) wglGetProcAddress( "glUniform1fv" );
	glUniform2fv = ( PFNGLUNIFORM2FVPROC ) wglGetProcAddress( "glUniform2fv" );
	glUniform3fv = ( PFNGLUNIFORM3FVPROC ) wglGetProcAddress( "glUniform3fv" );
	glUniform4fv = ( PFNGLUNIFORM4FVPROC ) wglGetProcAddress( "glUniform4fv" );
	glUniform1iv = ( PFNGLUNIFORM1IVPROC ) wglGetProcAddress( "glUniform1iv" );
	glUniform2iv = ( PFNGLUNIFORM2IVPROC ) wglGetProcAddress( "glUniform2iv" );
	glUniformMatrix4fv = ( PFNGLUNIFORMMATRIX4FVPROC ) wglGetProcAddress( "glUniformMatrix4fv" );
	glActiveTexture = ( PFNGLACTIVETEXTUREPROC ) wglGetProcAddress( "glActiveTexture" );
	glBindSampler = ( PFNGLBINDSAMPLERPROC ) wglGetProcAddress( "glBindSampler" );
	glGenSamplers = ( PFNGLGENSAMPLERSPROC ) wglGetProcAddress( "glGenSamplers" );
	glSamplerParameteri = ( PFNGLSAMPLERPARAMETERIPROC ) wglGetProcAddress( "glSamplerParameteri" );
	glGenFramebuffers = ( PFNGLGENFRAMEBUFFERSPROC ) wglGetProcAddress( "glGenFramebuffers" );
	glBindFramebuffer = ( PFNGLBINDFRAMEBUFFERPROC ) wglGetProcAddress( "glBindFramebuffer" );
	glFramebufferTexture = ( PFNGLFRAMEBUFFERTEXTUREPROC ) wglGetProcAddress( "glFramebufferTexture" );
	glCheckFramebufferStatus = ( PFNGLCHECKFRAMEBUFFERSTATUSPROC ) wglGetProcAddress( "glCheckFramebufferStatus" );
	glDeleteFramebuffers = ( PFNGLDELETEFRAMEBUFFERSPROC ) wglGetProcAddress( "glDeleteFrameBuffers" );
	glBlitFramebuffer = ( PFNGLBLITFRAMEBUFFERPROC ) wglGetProcAddress( "glBlitFramebuffer" );
	glDrawBuffers = ( PFNGLDRAWBUFFERSPROC ) wglGetProcAddress( "glDrawBuffers" );
}


//-----------------------------------------------------------------------------------------------
void Renderer::MakeScreenQuad()
{
	m_screenQuad = new Mesh();
	m_screenQuad->MakeNDCTangentQuadMesh();

	m_screenQuad->BindAttributeToMaterial( m_fboMaterial1, "inPosition", sizeof( Vertex_PCUTB ), ( GLvoid* ) offsetof( Vertex_PCUTB, m_position ) );
	m_screenQuad->BindAttributeToMaterial( m_fboMaterial1, "inUV0", sizeof( Vertex_PCUTB ), ( GLvoid* ) offsetof( Vertex_PCUTB, m_texCoords ) );
	m_screenQuad->BindAttributeToMaterial( m_fboMaterial1, "inColor", sizeof( Vertex_PCUTB ), ( GLvoid* ) offsetof( Vertex_PCUTB, m_color ) );
	m_screenQuad->BindAttributeToMaterial( m_fboMaterial1, "inTangent", sizeof( Vertex_PCUTB ), ( GLvoid* ) offsetof( Vertex_PCUTB, m_tangent ) );
	m_screenQuad->BindAttributeToMaterial( m_fboMaterial1, "inBitangent", sizeof( Vertex_PCUTB ), ( GLvoid* ) offsetof( Vertex_PCUTB, m_bitangent ) );

	BindBuffer( 0 );
}


//-----------------------------------------------------------------------------------------------
void Renderer::PushView() const
{
	glPushMatrix();
}


//-----------------------------------------------------------------------------------------------
void Renderer::PopView() const
{
	glPopMatrix();
}


//-----------------------------------------------------------------------------------------------
void Renderer::RotateView( float degrees, const Vector3& rotation )
{
	glRotatef( degrees, rotation.x, rotation.y, rotation.z );
}


//-----------------------------------------------------------------------------------------------
void Renderer::TranslateView( const Vector3& translation ) const
{
	glTranslatef( translation.x, translation.y, translation.z );
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawText2D( const Vector2& startBottomLeft, const std::string& asciiText, float cellHeight, const Rgba& tint, const BitmapFont* font )
{
	AABB2 bounds( startBottomLeft, Vector2( startBottomLeft.x + cellHeight, startBottomLeft.y + cellHeight ) );

	for ( unsigned int i = 0; i < asciiText.length(); i++ )
	{
		AABB2 glyphTexCoords = font->GetTexCoordsForGlyph( asciiText[ i ] );
		DrawTexturedAABB2( bounds, font->GetTexture(), glyphTexCoords.mins, glyphTexCoords.maxs, tint );
		bounds.mins.x += cellHeight;
		bounds.maxs.x += cellHeight;
	}
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawTextProportional2D( const Vector3& position, const std::string& asciiText, float scale, const Rgba& color, BitmapFontProportional* font, const Vector3& up, const Vector3& right )
{
	if ( asciiText == "" )
		return;

	Vector3 cursor = position;

	Glyph* prev_glyph = nullptr;

	for ( std::string::const_iterator it = asciiText.begin(); it != asciiText.end(); ++it )
	{
		Glyph* thisGlyph = font->m_glyphRegistry.at( *it );

		if ( thisGlyph == nullptr )
			continue;
		int kerningVal = 0;
		if ( prev_glyph != nullptr )
		{
			font->GetKerning( prev_glyph->m_charId, thisGlyph->m_charId, kerningVal );
		}

		cursor += ( right * scale * ( float ) kerningVal );

		Vector3 topLeft = cursor + ( right * scale * ( float ) thisGlyph->m_xOffset ) - ( up * scale * ( float ) thisGlyph->m_yOffset );

		Vector3 topRight = topLeft + ( right * scale * ( float ) thisGlyph->m_width );

		Vector3 bottomLeft = topLeft - ( up * scale * ( float ) thisGlyph->m_height );

		Vector3 bottomRight = bottomLeft + ( right * scale * ( float ) thisGlyph->m_width );

		Vector2 uv_topLeft = Vector2( ( float ) thisGlyph->m_xPos / ( float ) font->m_textureWidth, ( float ) thisGlyph->m_yPos / ( float ) font->m_textureHeight );

		Vector2 uv_topRight = uv_topLeft;
		uv_topRight.x += ( float ) thisGlyph->m_width / ( float ) font->m_textureWidth;

		Vector2 uv_bottomLeft = uv_topLeft;
		uv_bottomLeft.y += ( float ) thisGlyph->m_height / ( float ) font->m_textureHeight;

		Vector2 uv_bottomRight = uv_bottomLeft;
		uv_bottomRight.x += ( float ) thisGlyph->m_width / ( float ) font->m_textureWidth;

		Vertex_PCT verts[ 4 ];

		verts[ 0 ].m_position = bottomLeft;
		verts[ 0 ].m_texCoords = uv_bottomLeft;
		verts[ 0 ].m_color = color;

		verts[ 1 ].m_position = bottomRight;
		verts[ 1 ].m_texCoords = uv_bottomRight;
		verts[ 1 ].m_color = color;

		verts[ 2 ].m_position = topRight;
		verts[ 2 ].m_texCoords = uv_topRight;
		verts[ 2 ].m_color = color;

		verts[ 3 ].m_position = topLeft;
		verts[ 3 ].m_texCoords = uv_topLeft;
		verts[ 3 ].m_color = color;

		BindTexture( *font->GetTexture() );

		DrawVertexArray_PCT( verts, 4, PRIMITIVE_QUADS );

		cursor += right * scale * ( float ) thisGlyph->m_xAdvance;
		prev_glyph = thisGlyph;
	}
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawText3D( const Vector3& startBottomLeft, const std::string& asciiText, float cellHeight, const Rgba& tint, const BitmapFont* font )
{
	AABB3 bounds( startBottomLeft, Vector3( startBottomLeft.x + cellHeight, startBottomLeft.y, startBottomLeft.z + cellHeight ) );

	for ( unsigned int i = 0; i < asciiText.length(); i++ )
	{
		AABB2 glyphTexCoords = font->GetTexCoordsForGlyph( asciiText[ i ] );
		DrawTexturedQuad3D( bounds.mins,
			bounds.mins + Vector3( cellHeight, 0.0f, 0.0f ),
			bounds.maxs,
			bounds.mins + Vector3( 0.0f, 0.0f, cellHeight ),
			font->GetTexture(),
			glyphTexCoords.mins,
			glyphTexCoords.maxs,
			tint );
		bounds.mins.x += cellHeight;
		bounds.maxs.x += cellHeight;
	}
}


//-----------------------------------------------------------------------------------------------
void Renderer::SetOrtho( const Vector2& bottomLeft, const Vector2& topRight )
{
	glLoadIdentity();
	glOrtho( bottomLeft.x, topRight.x, bottomLeft.y, topRight.y, 0.f, 1.f );
}


//-----------------------------------------------------------------------------------------------
void Renderer::SetPerspective( float fovDegreesY, float aspect, float nearDist, float farDist )
{
	glLoadIdentity();
	gluPerspective( fovDegreesY, aspect, nearDist, farDist );
}


//-----------------------------------------------------------------------------------------------
void Renderer::ClearBuffer()
{
	glClearColor( 0.f, 0.0f, 0.f, 1.f );
	glClear( GL_COLOR_BUFFER_BIT );
}


//-----------------------------------------------------------------------------------------------
void Renderer::ClearScreen( const Rgba& clearColor )
{
	glClearColor( RangeMapZeroToOne( ( float ) clearColor.r, 0.0f, 255.0f ),
		RangeMapZeroToOne( ( float ) clearColor.g, 0.0f, 255.0f ),
		RangeMapZeroToOne( ( float ) clearColor.b, 0.0f, 255.0f ),
		RangeMapZeroToOne( ( float ) clearColor.a, 0.0f, 255.0f ) );
	glClearDepth( 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawQuad( const Vector2& bottomLeft, const Vector2& topRight, const Rgba& tint )
{
	Vertex_PCT verts[ 4 ];

	verts[ 0 ].m_position = Vector3( bottomLeft.x, bottomLeft.y, 0.0f );
	verts[ 0 ].m_color = tint;
	verts[ 0 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 1 ].m_position = Vector3( topRight.x, bottomLeft.y, 0.0f );
	verts[ 1 ].m_color = tint;
	verts[ 1 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 2 ].m_position = Vector3( topRight.x, topRight.y, 0.0f );
	verts[ 2 ].m_color = tint;
	verts[ 2 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 3 ].m_position = Vector3( bottomLeft.x, topRight.y, 0.0f );
	verts[ 3 ].m_color = tint;
	verts[ 3 ].m_texCoords = Vector2( 0.0f, 0.0f );

	BindTexture( Texture::CreateOrGetTexture( "Data/Images/PlainWhiteTexture.png" )[ 0 ] );

	DrawVertexArray_PCT( verts, 4, PRIMITIVE_QUADS );
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawQuad3D( const Vector3& bottomLeft, const Vector3& topRight, const Rgba& tint )
{
	Vertex_PCT verts[ 4 ];

	verts[ 0 ].m_position = Vector3( bottomLeft.x, bottomLeft.y, bottomLeft.z );
	verts[ 0 ].m_color = tint;
	verts[ 0 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 1 ].m_position = Vector3( topRight.x, bottomLeft.y, bottomLeft.z );
	verts[ 1 ].m_color = tint;
	verts[ 1 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 2 ].m_position = Vector3( topRight.x, topRight.y, topRight.z );
	verts[ 2 ].m_color = tint;
	verts[ 2 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 3 ].m_position = Vector3( bottomLeft.x, topRight.y, topRight.z );
	verts[ 3 ].m_color = tint;
	verts[ 3 ].m_texCoords = Vector2( 0.0f, 0.0f );

	BindTexture( Texture::CreateOrGetTexture( "Data/Images/PlainWhiteTexture.png" )[ 0 ] );

	DrawVertexArray_PCT( verts, 4, PRIMITIVE_QUADS );
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawTexturedQuad3D( const Vector3& bottomLeft,
	const Vector3& bottomRight,
	const Vector3& topRight,
	const Vector3& topLeft,
	const Texture& texture, const Vector2& texCoordMins, const Vector2& texCoordMaxs, const Rgba& tint )
{
	glEnable( GL_DEPTH_TEST );
	glDisable( GL_CULL_FACE );

	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	Vertex_PCT verts[ 4 ];

	verts[ 0 ].m_position = Vector3( bottomLeft );
	verts[ 0 ].m_color = tint;
	verts[ 0 ].m_texCoords = Vector2( texCoordMins.x, texCoordMaxs.y );

	verts[ 1 ].m_position = Vector3( bottomRight );
	verts[ 1 ].m_color = tint;
	verts[ 1 ].m_texCoords = Vector2( texCoordMaxs.x, texCoordMaxs.y );

	verts[ 2 ].m_position = Vector3( topRight );
	verts[ 2 ].m_color = tint;
	verts[ 2 ].m_texCoords = Vector2( texCoordMaxs.x, texCoordMins.y );

	verts[ 3 ].m_position = Vector3( topLeft );
	verts[ 3 ].m_color = tint;
	verts[ 3 ].m_texCoords = Vector2( texCoordMins.x, texCoordMins.y );

	BindTexture( texture );

	DrawVertexArray_PCT( verts, 4, PRIMITIVE_QUADS );
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawTexturedAABB2( const AABB2& bounds, const Texture& texture, const Vector2& texCoordMins, const Vector2& texCoordMaxs, const Rgba& tint )
{
	Vertex_PCT verts[ 4 ];

	verts[ 0 ].m_position = Vector3( bounds.mins.x, bounds.mins.y, 0.0f );
	verts[ 0 ].m_color = tint;
	verts[ 0 ].m_texCoords = Vector2( texCoordMins.x, texCoordMaxs.y );

	verts[ 1 ].m_position = Vector3( bounds.maxs.x, bounds.mins.y, 0.0f );
	verts[ 1 ].m_color = tint;
	verts[ 1 ].m_texCoords = Vector2( texCoordMaxs.x, texCoordMaxs.y );

	verts[ 2 ].m_position = Vector3( bounds.maxs.x, bounds.maxs.y, 0.0f );
	verts[ 2 ].m_color = tint;
	verts[ 2 ].m_texCoords = Vector2( texCoordMaxs.x, texCoordMins.y );

	verts[ 3 ].m_position = Vector3( bounds.mins.x, bounds.maxs.y, 0.0f );
	verts[ 3 ].m_color = tint;
	verts[ 3 ].m_texCoords = Vector2( texCoordMins.x, texCoordMins.y );

	BindTexture( texture );

	DrawVertexArray_PCT( verts, 4, PRIMITIVE_QUADS );
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawDebugAxisLines()
{

	static BitmapFont* fixedFont = BitmapFont::CreateOrGetFont( "Data/Fonts/SquirrelFixedFont.png" );

	glDepthMask( GL_FALSE );
	g_theRenderer->DrawText3D( Vector3( 2.0f, 0.0f, -0.05f ), "+X", 0.1f, Rgba::WHITE, fixedFont );
	g_theRenderer->DrawText3D( Vector3( -0.1f, 2.0f, -0.05f ), "+Y", 0.1f, Rgba::WHITE, fixedFont );
	g_theRenderer->DrawText3D( Vector3( -0.1f, 0.0f, 2.0f ), "+Z", 0.1f, Rgba::WHITE, fixedFont );
	glDepthMask( GL_TRUE );

	glLineWidth( 3.0f );
	glEnable( GL_DEPTH_TEST );

	Vertex_PCT verts[ 6 ];

	verts[ 0 ].m_position = Vector3( 0.0f, 0.0f, 0.0f );
	verts[ 0 ].m_color = Rgba::RED;
	verts[ 0 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 1 ].m_position = Vector3( 2.0f, 0.0f, 0.0f );
	verts[ 1 ].m_color = Rgba::RED;
	verts[ 1 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 2 ].m_position = Vector3( 0.0f, 0.0f, 0.0f );
	verts[ 2 ].m_color = Rgba::GREEN;
	verts[ 2 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 3 ].m_position = Vector3( 0.0f, 2.0f, 0.0f );
	verts[ 3 ].m_color = Rgba::GREEN;
	verts[ 3 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 4 ].m_position = Vector3( 0.0f, 0.0f, 0.0f );
	verts[ 4 ].m_color = Rgba::BLUE;
	verts[ 4 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 5 ].m_position = Vector3( 0.0f, 0.0f, 2.0f );
	verts[ 5 ].m_color = Rgba::BLUE;
	verts[ 5 ].m_texCoords = Vector2( 0.0f, 0.0f );

	BindTexture( Texture::CreateOrGetTexture( "Data/Images/PlainWhiteTexture.png" )[ 0 ] );

	DrawVertexArray_PCT( verts, 6, PRIMITIVE_LINES );

	glLineWidth( 1.0f );
	glDisable( GL_DEPTH_TEST );

	verts[ 0 ].m_position = Vector3( 0.0f, 0.0f, 0.0f );
	verts[ 0 ].m_color = Rgba::RED;
	verts[ 0 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 1 ].m_position = Vector3( 2.0f, 0.0f, 0.0f );
	verts[ 1 ].m_color = Rgba::RED;
	verts[ 1 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 2 ].m_position = Vector3( 0.0f, 0.0f, 0.0f );
	verts[ 2 ].m_color = Rgba::GREEN;
	verts[ 2 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 3 ].m_position = Vector3( 0.0f, 2.0f, 0.0f );
	verts[ 3 ].m_color = Rgba::GREEN;
	verts[ 3 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 4 ].m_position = Vector3( 0.0f, 0.0f, 0.0f );
	verts[ 4 ].m_color = Rgba::BLUE;
	verts[ 4 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 5 ].m_position = Vector3( 0.0f, 0.0f, 2.0f );
	verts[ 5 ].m_color = Rgba::BLUE;
	verts[ 5 ].m_texCoords = Vector2( 0.0f, 0.0f );

	DrawVertexArray_PCT( verts, 6, PRIMITIVE_LINES );
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawCrosshairs( const Vector2& screenPosition, const Rgba& tint )
{
	glLineWidth( 5.0f );
	glBlendFunc( GL_ONE_MINUS_DST_COLOR, GL_ZERO );

	Vertex_PCT verts[ 4 ];

	verts[ 0 ].m_position = Vector3( screenPosition.x - 20.0f, screenPosition.y, 0.0f );
	verts[ 0 ].m_color = tint;
	verts[ 0 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 1 ].m_position = Vector3( screenPosition.x + 20.0f, screenPosition.y, 0.0f );
	verts[ 1 ].m_color = tint;
	verts[ 1 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 2 ].m_position = Vector3( screenPosition.x, screenPosition.y - 20.0f, 0.0f );
	verts[ 2 ].m_color = tint;
	verts[ 2 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 3 ].m_position = Vector3( screenPosition.x, screenPosition.y + 20.0f, 0.0f );
	verts[ 3 ].m_color = tint;
	verts[ 3 ].m_texCoords = Vector2( 0.0f, 0.0f );

	BindTexture( Texture::CreateOrGetTexture( "Data/Images/PlainWhiteTexture.png" )[ 0 ] );

	DrawVertexArray_PCT( verts, 4, PRIMITIVE_LINES );

	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}


//-----------------------------------------------------------------------------------------------
void Renderer::SetActiveTextureTo0() const
{
	glActiveTexture( GL_TEXTURE0 );
}


//-----------------------------------------------------------------------------------------------
void Renderer::SetLineWidth( float lineWidth ) const
{
	glLineWidth( lineWidth );
}


//-----------------------------------------------------------------------------------------------
void Renderer::DepthTestingEnabled( bool enabled ) const
{
	if ( enabled )
	{
		glEnable( GL_DEPTH_TEST );
	}
	else
	{
		glDisable( GL_DEPTH_TEST );
	}
}

//-----------------------------------------------------------------------------------------------
void Renderer::BackFaceCullingEnabled( bool enabled ) const
{
	if ( enabled )
	{
		glEnable( GL_CULL_FACE );
	}
	else
	{
		glDisable( GL_CULL_FACE );
	}
}


//-----------------------------------------------------------------------------------------------
void Renderer::AlphaTestingEnabled( bool enabled ) const
{
	if ( enabled )
	{
		glEnable( GL_ALPHA_TEST );
	}
	else
	{
		glDisable( GL_ALPHA_TEST );
	}
}


//-----------------------------------------------------------------------------------------------
void Renderer::ZWriteEnabled( bool enabled ) const
{
	if ( enabled )
	{
		glDepthMask( GL_TRUE );
	}
	else
	{
		glDepthMask( GL_FALSE );
	}
}


//-----------------------------------------------------------------------------------------------
void Renderer::BindTexture( const Texture& texture )
{
	glBindTexture( GL_TEXTURE_2D, texture.m_openglTextureID );
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawVertexArray_PCT( const Vertex_PCT* verts, int numVerts, PrimitiveType primitiveType )
{
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	glVertexPointer( 3, GL_FLOAT, sizeof( Vertex_PCT ), &verts[ 0 ].m_position );
	glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( Vertex_PCT ), &verts[ 0 ].m_color );
	glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex_PCT ), &verts[ 0 ].m_texCoords );

	switch ( primitiveType )
	{
	case 0: // PRIMITIVE_POINTS
		glDrawArrays( GL_POINTS, 0, numVerts );
		break;
	case 1: // PRIMITIVE_LINES
		glDrawArrays( GL_LINES, 0, numVerts );
		break;
	case 2: // PRIMITIVE_LINE_LOOP
		glDrawArrays( GL_LINE_LOOP, 0, numVerts );
		break;
	case 3: // PRIMITIVE_TRIANGLES
		glDrawArrays( GL_TRIANGLES, 0, numVerts );
		break;
	case 4: // PRIMITIVE_TRIANGLE_FAN
		glDrawArrays( GL_TRIANGLE_FAN, 0, numVerts );
		break;
	case 5: // PRIMITIVE_QUADS
		glDrawArrays( GL_QUADS, 0, numVerts );
		break;
	}

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawSquareLineLoop3D( const Vector3& pos1, const Vector3& pos2, const Vector3& pos3, const Vector3& pos4, const Rgba& tint )
{
	SetLineWidth( 2.0f );
	DepthTestingEnabled( true );
	BackFaceCullingEnabled( false );

	Vertex_PCT verts[ 4 ];

	verts[ 0 ].m_position = Vector3( pos1.x, pos1.y, pos1.z );
	verts[ 0 ].m_color = tint;
	verts[ 0 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 1 ].m_position = Vector3( pos2.x, pos2.y, pos2.z );
	verts[ 1 ].m_color = tint;
	verts[ 1 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 2 ].m_position = Vector3( pos3.x, pos3.y, pos3.z );
	verts[ 2 ].m_color = tint;
	verts[ 2 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 3 ].m_position = Vector3( pos4.x, pos4.y, pos4.z );
	verts[ 3 ].m_color = tint;
	verts[ 3 ].m_texCoords = Vector2( 0.0f, 0.0f );

	BindTexture( Texture::CreateOrGetTexture( "Data/Images/PlainWhiteTexture.png" )[ 0 ] );

	DrawVertexArray_PCT( verts, 4, PRIMITIVE_LINE_LOOP );
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawSquareLineLoop3DXray( const Vector3& pos1, const Vector3& pos2, const Vector3& pos3, const Vector3& pos4, const Rgba& tint )
{
	SetLineWidth( 2.0f );
	DepthTestingEnabled( true );
	BackFaceCullingEnabled( false );

	Vertex_PCT verts[ 4 ];

	verts[ 0 ].m_position = Vector3( pos1.x, pos1.y, pos1.z );
	verts[ 0 ].m_color = tint;
	verts[ 0 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 1 ].m_position = Vector3( pos2.x, pos2.y, pos2.z );
	verts[ 1 ].m_color = tint;
	verts[ 1 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 2 ].m_position = Vector3( pos3.x, pos3.y, pos3.z );
	verts[ 2 ].m_color = tint;
	verts[ 2 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 3 ].m_position = Vector3( pos4.x, pos4.y, pos4.z );
	verts[ 3 ].m_color = tint;
	verts[ 3 ].m_texCoords = Vector2( 0.0f, 0.0f );

	BindTexture( Texture::CreateOrGetTexture( "Data/Images/PlainWhiteTexture.png" )[ 0 ] );

	DrawVertexArray_PCT( verts, 4, PRIMITIVE_LINE_LOOP );

	SetLineWidth( 1.0f );
	DepthTestingEnabled( false );

	verts[ 0 ].m_position = Vector3( pos1.x, pos1.y, pos1.z );
	verts[ 0 ].m_color = tint;
	verts[ 0 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 1 ].m_position = Vector3( pos2.x, pos2.y, pos2.z );
	verts[ 1 ].m_color = tint;
	verts[ 1 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 2 ].m_position = Vector3( pos3.x, pos3.y, pos3.z );
	verts[ 2 ].m_color = tint;
	verts[ 2 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 3 ].m_position = Vector3( pos4.x, pos4.y, pos4.z );
	verts[ 3 ].m_color = tint;
	verts[ 3 ].m_texCoords = Vector2( 0.0f, 0.0f );

	DrawVertexArray_PCT( verts, 4, PRIMITIVE_LINE_LOOP );
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawPlayerRaycastLine3DXray( const Vector3& pos1, const Vector3& pos2, const Rgba& tint )
{
	SetLineWidth( 3.0f );
	DepthTestingEnabled( true );

	Vertex_PCT verts[ 2 ];

	verts[ 0 ].m_position = Vector3( pos1.x, pos1.y, pos1.z );
	verts[ 0 ].m_color = tint;
	verts[ 0 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 1 ].m_position = Vector3( pos2.x, pos2.y, pos2.z );
	verts[ 1 ].m_color = tint;
	verts[ 1 ].m_texCoords = Vector2( 0.0f, 0.0f );

	BindTexture( Texture::CreateOrGetTexture( "Data/Images/PlainWhiteTexture.png" )[ 0 ] );

	DrawVertexArray_PCT( verts, 4, PRIMITIVE_LINES );

	SetLineWidth( 1.0f );
	DepthTestingEnabled( false );

	verts[ 2 ];

	verts[ 0 ].m_position = Vector3( pos1.x, pos1.y, pos1.z );
	verts[ 0 ].m_color = tint;
	verts[ 0 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 1 ].m_position = Vector3( pos2.x, pos2.y, pos2.z );
	verts[ 1 ].m_color = tint;
	verts[ 1 ].m_texCoords = Vector2( 0.0f, 0.0f );

	DrawVertexArray_PCT( verts, 4, PRIMITIVE_LINES );
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawVBO_PCT( unsigned int vboID, int numVerts, PrimitiveType primitiveType )
{
	glBindBuffer( GL_ARRAY_BUFFER, vboID );

	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_COLOR_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	glVertexPointer( 3, GL_FLOAT, sizeof( Vertex_PCT ), ( const GLvoid* ) offsetof( Vertex_PCT, m_position ) );
	glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( Vertex_PCT ), ( const GLvoid* ) offsetof( Vertex_PCT, m_color ) );
	glTexCoordPointer( 2, GL_FLOAT, sizeof( Vertex_PCT ), ( const GLvoid* ) offsetof( Vertex_PCT, m_texCoords ) );

	switch ( primitiveType )
	{
	case 0: // PRIMITIVE_POINTS
		glDrawArrays( GL_POINTS, 0, numVerts );
		break;
	case 1: // PRIMITIVE_LINES
		glDrawArrays( GL_LINES, 0, numVerts );
		break;
	case 2: // PRIMITIVE_LINE_LOOP
		glDrawArrays( GL_LINE_LOOP, 0, numVerts );
		break;
	case 3: // PRIMITIVE_TRIANGLES
		glDrawArrays( GL_TRIANGLES, 0, numVerts );
		break;
	case 4: // PRIMITIVE_QUADS
		glDrawArrays( GL_QUADS, 0, numVerts );
		break;
	}

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_COLOR_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}


//-----------------------------------------------------------------------------------------------
void Renderer::BindBuffer( unsigned int vboID )
{
	glBindBuffer( GL_ARRAY_BUFFER, vboID );
}


//-----------------------------------------------------------------------------------------------
void Renderer::BufferData( GLsizeiptr vertexArrayNumBytes, const GLvoid* vertexArrayData )
{
	glBufferData( GL_ARRAY_BUFFER, vertexArrayNumBytes, vertexArrayData, GL_STATIC_DRAW );
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawPoint3D( const Vector3& position, const Rgba& tint )
{
	Vertex_PCT verts[ 6 ];

	verts[ 0 ].m_position = Vector3( position.x - 0.25f, position.y, position.z );
	verts[ 0 ].m_color = tint;
	verts[ 0 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 1 ].m_position = Vector3( position.x + 0.25f, position.y, position.z );
	verts[ 1 ].m_color = tint;
	verts[ 1 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 2 ].m_position = Vector3( position.x, position.y - 0.25f, position.z );
	verts[ 2 ].m_color = tint;
	verts[ 2 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 3 ].m_position = Vector3( position.x, position.y + 0.25f, position.z );
	verts[ 3 ].m_color = tint;
	verts[ 3 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 4 ].m_position = Vector3( position.x, position.y, position.z - 0.25f );
	verts[ 4 ].m_color = tint;
	verts[ 4 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 5 ].m_position = Vector3( position.x, position.y, position.z + 0.25f );
	verts[ 5 ].m_color = tint;
	verts[ 5 ].m_texCoords = Vector2( 0.0f, 0.0f );

	BindTexture( Texture::CreateOrGetTexture( "Data/Images/PlainWhiteTexture.png" )[ 0 ] );

	DrawVertexArray_PCT( verts, 6, PRIMITIVE_LINES );
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawLine3D( const Vector3& startPosition, const Vector3& endPosition, const Rgba& tint, float lineWidth )
{
	glLineWidth( lineWidth );

	Vertex_PCT verts[ 2 ];

	verts[ 0 ].m_position = startPosition;
	verts[ 0 ].m_color = tint;
	verts[ 0 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 1 ].m_position = endPosition;
	verts[ 1 ].m_color = tint;
	verts[ 1 ].m_texCoords = Vector2( 0.0f, 0.0f );

	BindTexture( Texture::CreateOrGetTexture( "Data/Images/PlainWhiteTexture.png" )[ 0 ] );

	DrawVertexArray_PCT( verts, 2, PRIMITIVE_LINES );
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawAABB3( const Vector3& mins, const Vector3& maxs, const Rgba& edgeDrawColor, const Rgba& faceDrawColor )
{
	// Bottom face
	Vertex_PCT verts[ 4 ];

	verts[ 0 ].m_position = Vector3( mins.x, mins.y, mins.z );
	verts[ 0 ].m_color = edgeDrawColor;
	verts[ 0 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 1 ].m_position = Vector3( maxs.x, mins.y, mins.z );
	verts[ 1 ].m_color = edgeDrawColor;
	verts[ 1 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 2 ].m_position = Vector3( maxs.x, maxs.y, mins.z );
	verts[ 2 ].m_color = edgeDrawColor;
	verts[ 2 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 3 ].m_position = Vector3( mins.x, maxs.y, mins.z );
	verts[ 3 ].m_color = edgeDrawColor;
	verts[ 3 ].m_texCoords = Vector2( 0.0f, 0.0f );

	BindTexture( Texture::CreateOrGetTexture( "Data/Images/PlainWhiteTexture.png" )[ 0 ] );

	DrawVertexArray_PCT( verts, 4, PRIMITIVE_LINE_LOOP );

	DrawQuad3D( mins, Vector3( maxs.x, maxs.y, mins.z ), faceDrawColor );

	// Top face
	verts[ 0 ].m_position = Vector3( mins.x, mins.y, maxs.z );
	verts[ 0 ].m_color = edgeDrawColor;
	verts[ 0 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 1 ].m_position = Vector3( maxs.x, mins.y, maxs.z );
	verts[ 1 ].m_color = edgeDrawColor;
	verts[ 1 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 2 ].m_position = Vector3( maxs.x, maxs.y, maxs.z );
	verts[ 2 ].m_color = edgeDrawColor;
	verts[ 2 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 3 ].m_position = Vector3( mins.x, maxs.y, maxs.z );
	verts[ 3 ].m_color = edgeDrawColor;
	verts[ 3 ].m_texCoords = Vector2( 0.0f, 0.0f );

	DrawVertexArray_PCT( verts, 4, PRIMITIVE_LINE_LOOP );

	DrawQuad3D( Vector3( mins.x, mins.y, maxs.z ), Vector3( maxs.x, maxs.y, maxs.z ), faceDrawColor );

	// North face
	verts[ 0 ].m_position = Vector3( mins.x, maxs.y, maxs.z );
	verts[ 0 ].m_color = edgeDrawColor;
	verts[ 0 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 1 ].m_position = Vector3( maxs.x, maxs.y, maxs.z );
	verts[ 1 ].m_color = edgeDrawColor;
	verts[ 1 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 2 ].m_position = Vector3( maxs.x, maxs.y, mins.z );
	verts[ 2 ].m_color = edgeDrawColor;
	verts[ 2 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 3 ].m_position = Vector3( mins.x, maxs.y, mins.z );
	verts[ 3 ].m_color = edgeDrawColor;
	verts[ 3 ].m_texCoords = Vector2( 0.0f, 0.0f );

	DrawVertexArray_PCT( verts, 4, PRIMITIVE_LINE_LOOP );

	DrawQuad3D( Vector3( mins.x, maxs.y, mins.z ), Vector3( maxs.x, maxs.y, maxs.z ), faceDrawColor );

	// East face
	verts[ 0 ].m_position = Vector3( maxs.x, mins.y, maxs.z );
	verts[ 0 ].m_color = edgeDrawColor;
	verts[ 0 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 1 ].m_position = Vector3( maxs.x, maxs.y, maxs.z );
	verts[ 1 ].m_color = edgeDrawColor;
	verts[ 1 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 2 ].m_position = Vector3( maxs.x, maxs.y, mins.z );
	verts[ 2 ].m_color = edgeDrawColor;
	verts[ 2 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 3 ].m_position = Vector3( maxs.x, mins.y, mins.z );
	verts[ 3 ].m_color = edgeDrawColor;
	verts[ 3 ].m_texCoords = Vector2( 0.0f, 0.0f );

	DrawVertexArray_PCT( verts, 4, PRIMITIVE_LINE_LOOP );

	//////////////////////////////////////////////////// FIX ME ////////////////////////////////////////////////
	verts[ 0 ].m_position = Vector3( maxs.x, mins.y, mins.z );
	verts[ 0 ].m_color = faceDrawColor;
	verts[ 0 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 1 ].m_position = Vector3( maxs.x, maxs.y, mins.z );
	verts[ 1 ].m_color = faceDrawColor;
	verts[ 1 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 2 ].m_position = Vector3( maxs.x, maxs.y, maxs.z );
	verts[ 2 ].m_color = faceDrawColor;
	verts[ 2 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 3 ].m_position = Vector3( maxs.x, mins.y, maxs.z );
	verts[ 3 ].m_color = faceDrawColor;
	verts[ 3 ].m_texCoords = Vector2( 0.0f, 0.0f );

	DrawVertexArray_PCT( verts, 4, PRIMITIVE_QUADS );
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// South face
	verts[ 0 ].m_position = Vector3( mins.x, mins.y, maxs.z );
	verts[ 0 ].m_color = edgeDrawColor;
	verts[ 0 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 1 ].m_position = Vector3( maxs.x, mins.y, maxs.z );
	verts[ 1 ].m_color = edgeDrawColor;
	verts[ 1 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 2 ].m_position = Vector3( maxs.x, mins.y, mins.z );
	verts[ 2 ].m_color = edgeDrawColor;
	verts[ 2 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 3 ].m_position = Vector3( mins.x, mins.y, mins.z );
	verts[ 3 ].m_color = edgeDrawColor;
	verts[ 3 ].m_texCoords = Vector2( 0.0f, 0.0f );

	DrawVertexArray_PCT( verts, 4, PRIMITIVE_LINE_LOOP );

	DrawQuad3D( Vector3( mins.x, mins.y, mins.z ), Vector3( maxs.x, mins.y, maxs.z ), faceDrawColor );

	// West face
	verts[ 0 ].m_position = Vector3( mins.x, mins.y, maxs.z );
	verts[ 0 ].m_color = edgeDrawColor;
	verts[ 0 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 1 ].m_position = Vector3( mins.x, mins.y, maxs.z );
	verts[ 1 ].m_color = edgeDrawColor;
	verts[ 1 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 2 ].m_position = Vector3( mins.x, mins.y, mins.z );
	verts[ 2 ].m_color = edgeDrawColor;
	verts[ 2 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 3 ].m_position = Vector3( mins.x, mins.y, mins.z );
	verts[ 3 ].m_color = edgeDrawColor;
	verts[ 3 ].m_texCoords = Vector2( 0.0f, 0.0f );

	DrawVertexArray_PCT( verts, 4, PRIMITIVE_LINE_LOOP );

	//////////////////////////////////////////////////// FIX ME ////////////////////////////////////////////////
	verts[ 0 ].m_position = Vector3( mins.x, maxs.y, mins.z );
	verts[ 0 ].m_color = faceDrawColor;
	verts[ 0 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 1 ].m_position = Vector3( mins.x, mins.y, mins.z );
	verts[ 1 ].m_color = faceDrawColor;
	verts[ 1 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 2 ].m_position = Vector3( mins.x, mins.y, maxs.z );
	verts[ 2 ].m_color = faceDrawColor;
	verts[ 2 ].m_texCoords = Vector2( 0.0f, 0.0f );

	verts[ 3 ].m_position = Vector3( mins.x, maxs.y, maxs.z );
	verts[ 3 ].m_color = faceDrawColor;
	verts[ 3 ].m_texCoords = Vector2( 0.0f, 0.0f );

	DrawVertexArray_PCT( verts, 4, PRIMITIVE_QUADS );
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
}


//-----------------------------------------------------------------------------------------------
void Renderer::DebugDrawPoint( const Vector3& position, const Rgba& drawColor, float drawDurationSeconds, DebugDrawMode drawMode )
{
	RenderCommand* renderCommand = new RenderCommand();
	renderCommand->m_drawShape = DRAW_SHAPE_DEBUG_POINT;
	renderCommand->m_startPosition = position;
	renderCommand->m_drawColor = drawColor;
	renderCommand->m_drawDurationSeconds = drawDurationSeconds;
	renderCommand->m_drawMode = drawMode;
	m_renderCommands.push_back( renderCommand );
}


//-----------------------------------------------------------------------------------------------
void Renderer::DebugDrawLine( const Vector3& startPosition, const Vector3& endPosition, const Rgba& drawColor, float drawDurationSeconds, DebugDrawMode drawMode )
{
	RenderCommand* renderCommand = new RenderCommand();
	renderCommand->m_drawShape = DRAW_SHAPE_DEBUG_LINE;
	renderCommand->m_startPosition = startPosition;
	renderCommand->m_endPosition = endPosition;
	renderCommand->m_drawColor = drawColor;
	renderCommand->m_drawDurationSeconds = drawDurationSeconds;
	renderCommand->m_drawMode = drawMode;
	m_renderCommands.push_back( renderCommand );
}


//-----------------------------------------------------------------------------------------------
void Renderer::DebugDrawArrow( const Vector3& startPosition, const Vector3& endPosition, const Rgba& drawColor, float drawDurationSeconds, DebugDrawMode drawMode )
{
	RenderCommand* renderCommand = new RenderCommand();
	renderCommand->m_drawShape = DRAW_SHAPE_DEBUG_ARROW;
	renderCommand->m_startPosition = startPosition;
	renderCommand->m_endPosition = endPosition;
	renderCommand->m_drawColor = drawColor;
	renderCommand->m_drawDurationSeconds = drawDurationSeconds;
	renderCommand->m_drawMode = drawMode;
	m_renderCommands.push_back( renderCommand );
}


//-----------------------------------------------------------------------------------------------
void Renderer::DebugDrawAABB3( const Vector3& mins, const Vector3& maxs, const Rgba& edgeDrawColor, const Rgba& faceDrawColor, float drawDurationSeconds, DebugDrawMode drawMode )
{
	RenderCommand* renderCommand = new RenderCommand();
	renderCommand->m_drawShape = DRAW_SHAPE_DEBUG_AABB3;
	renderCommand->m_boundingBoxMins = mins;
	renderCommand->m_boundingBoxMaxs = maxs;
	renderCommand->m_drawColor = edgeDrawColor;
	renderCommand->m_faceDrawColor = faceDrawColor;
	renderCommand->m_drawDurationSeconds = drawDurationSeconds;
	renderCommand->m_drawMode = drawMode;
	m_renderCommands.push_back( renderCommand );
}


//-----------------------------------------------------------------------------------------------
void Renderer::DebugDrawSphere( const Vector3& position, float radius, const Rgba& drawColor, float drawDurationSeconds, DebugDrawMode drawMode )
{
	RenderCommand* renderCommand = new RenderCommand();
	renderCommand->m_drawShape = DRAW_SHAPE_DEBUG_SPHERE;
	renderCommand->m_startPosition = position;
	renderCommand->m_sphereRadius = radius;
	renderCommand->m_drawColor = drawColor;
	renderCommand->m_drawDurationSeconds = drawDurationSeconds;
	renderCommand->m_drawMode = drawMode;
	m_renderCommands.push_back( renderCommand );
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawDebugRenderCommands( float deltaSeconds )
{
	for ( auto commandIterator = m_renderCommands.begin(); commandIterator != m_renderCommands.end(); ++commandIterator )
	{
		if ( ( *commandIterator )->m_renderingEnabled )
		{
			if ( ( *commandIterator )->m_drawMode == DRAW_MODE_DEPTH_TESTING_ON )
			{
				DepthTestingEnabled( true );
			}
			else if ( ( *commandIterator )->m_drawMode == DRAW_MODE_DEPTH_TESTING_OFF )
			{
				DepthTestingEnabled( false );
			}

			switch ( ( *commandIterator )->m_drawShape )
			{
			case DRAW_SHAPE_DEBUG_POINT:
				DrawPoint3D( ( *commandIterator )->m_startPosition, ( *commandIterator )->m_drawColor );
				break;
			case DRAW_SHAPE_DEBUG_LINE:
				DrawLine3D( ( *commandIterator )->m_startPosition, ( *commandIterator )->m_endPosition, ( *commandIterator )->m_drawColor, 1.0f );
				break;
			case DRAW_SHAPE_DEBUG_ARROW:
				DrawLine3D( ( *commandIterator )->m_startPosition, ( *commandIterator )->m_endPosition, ( *commandIterator )->m_drawColor, 1.0f );
				DrawPoint3D( ( *commandIterator )->m_endPosition, ( *commandIterator )->m_drawColor );
				break;
			case DRAW_SHAPE_DEBUG_AABB3:
				DrawAABB3( ( *commandIterator )->m_boundingBoxMins, ( *commandIterator )->m_boundingBoxMaxs, ( *commandIterator )->m_drawColor, ( *commandIterator )->m_faceDrawColor );
				break;
			case DRAW_SHAPE_DEBUG_SPHERE:
				DrawSphere( ( *commandIterator )->m_startPosition, ( *commandIterator )->m_sphereRadius, ( *commandIterator )->m_drawColor );
				break;
			}
		}

		float remainingDrawDurationSeconds = ( *commandIterator )->m_drawDurationSeconds;
		if ( remainingDrawDurationSeconds != -1.0f ) // If not drawing debug shape indefinitely...
		{
			remainingDrawDurationSeconds -= deltaSeconds;

			if ( remainingDrawDurationSeconds <= 0.0f ) // Disable rendering of render command
			{
				( *commandIterator )->m_renderingEnabled = false;
			}
			else // Update remaining draw duration seconds of render command
			{
				( *commandIterator )->m_drawDurationSeconds = remainingDrawDurationSeconds;
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawSphere( const Vector3& position, float radius, const Rgba& tint )
{
	int numSides = 25; // # of triangles used to draw polygon

	float twicePi = 2.0f * pi;

	Vertex_PCT verts[ 25 ];

	for ( int i = 0; i < numSides; i++ )
	{
		verts[ i ].m_position = Vector3( position.x + ( radius * cos( i *  twicePi / numSides ) ), position.y + ( radius* sin( i * twicePi / numSides ) ), position.z );
		verts[ i ].m_color = tint;
		verts[ i ].m_texCoords = Vector2( 0.0f, 0.0f );
	}

	DrawVertexArray_PCT( verts, 25, PRIMITIVE_LINE_LOOP );

	for ( int i = 0; i < numSides; i++ )
	{
		verts[ i ].m_position = Vector3( position.x + ( radius * cos( i *  twicePi / numSides ) ), position.y, position.z + ( radius* sin( i * twicePi / numSides ) ) );
		verts[ i ].m_color = tint;
		verts[ i ].m_texCoords = Vector2( 0.0f, 0.0f );
	}

	DrawVertexArray_PCT( verts, 25, PRIMITIVE_LINE_LOOP );

	for ( int i = 0; i < numSides; i++ )
	{
		verts[ i ].m_position = Vector3( position.x, position.y + ( radius * cos( i *  twicePi / numSides ) ), position.z + ( radius* sin( i * twicePi / numSides ) ) );
		verts[ i ].m_color = tint;
		verts[ i ].m_texCoords = Vector2( 0.0f, 0.0f );
	}

	DrawVertexArray_PCT( verts, 25, PRIMITIVE_LINE_LOOP );
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawGrid()
{
	Vector3 v1 = Vector3( 0.0f, 0.0f, 0.0f );
	Vector3 v2 = Vector3( 0.0f, 0.0f, 0.0f );

	for ( int xCounter = 0; xCounter < 20; xCounter++ )
	{
		v1.x = ( v1.x + 1.0f );
		v2.x = v1.x;
		v2.y = 20.0f;
		DrawLine3D( v1, v2, Rgba::WHITE, 1.0f );
	}

	v1 = Vector3( 0.0f, 0.0f, 0.0f );
	v2 = Vector3( 0.0f, 0.0f, 0.0f );

	for ( int yCounter = 0; yCounter < 20; yCounter++ )
	{
		v1.y = ( v1.y + 1 );
		v2.y = v1.y;
		v2.x = 20.0f;
		DrawLine3D( v1, v2, Rgba::WHITE, 1.0f );
	}
}


//-----------------------------------------------------------------------------------------------
void Renderer::SetLineWidth( float lineWidth )
{
	glLineWidth( lineWidth );
}


//-----------------------------------------------------------------------------------------------
void Renderer::SetDrawColor( const Rgba& color )
{
	glColor4ub( color.r, color.g, color.b, color.a );
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawFilledPolygon( int numSides, float centerX, float centerY, float radius )
{
	int triangleAmount = numSides; // # of triangles used to draw polygon

	float twicePi = 2.0f * pi;

	glBegin( GL_TRIANGLE_FAN );
	glDisable( GL_TEXTURE_2D ); // #Eiserloh: Need to disable texturing if you're not providing texture coordinates
	glVertex2f( centerX, centerY ); // center of polygon
	for ( int i = 0; i <= triangleAmount; i++ )
	{
		glVertex2f(
			centerX + ( radius * cos( i *  twicePi / triangleAmount ) ),
			centerY + ( radius * sin( i * twicePi / triangleAmount ) )
			);
	}
	glEnd();
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawFilledPolygon( int numSides, float centerX, float centerY, float radius, const Rgba& drawColor /*= Rgba::WHITE */ )
{
	std::vector< Vertex_PCT > verts;

	int triangleAmount = numSides; // # of triangles used to draw polygon

	float twicePi = 2.0f * pi;

	Vertex_PCT vert;
	vert.m_position.x = centerX;
	vert.m_position.y = centerY;
	vert.m_color = drawColor;
	vert.m_texCoords = Vector2( 0.0f, 0.0f );
	verts.push_back( vert );

	for ( int i = 0; i <= triangleAmount; i++ )
	{
		vert.m_position.x = centerX + ( radius * cos( i *  twicePi / triangleAmount ) );
		vert.m_position.y = centerY + ( radius* sin( i * twicePi / triangleAmount ) );
		vert.m_color = drawColor;
		vert.m_texCoords = Vector2( 0.0f, 0.0f );
		verts.push_back( vert );
	}

	BindTexture( Texture::CreateOrGetTexture( "Data/Images/PlainWhiteTexture.png" )[ 0 ] );

	DrawVertexArray_PCT( &verts[ 0 ], verts.size(), PRIMITIVE_TRIANGLE_FAN );
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawHollowPolygon( int numSides, float centerX, float centerY, float radius )
{
	int lineAmount = numSides; // # of triangles used to draw polygon

	float twicePi = 2.0f * pi;

	glBegin( GL_LINE_LOOP );
	glDisable( GL_TEXTURE_2D ); // #Eiserloh: Need to disable texturing if you're not providing texture coordinates
	for ( int i = 0; i <= lineAmount; i++ )
	{
		glVertex2f(
			centerX + ( radius * cos( i *  twicePi / lineAmount ) ),
			centerY + ( radius* sin( i * twicePi / lineAmount ) )
			);
	}
	glEnd();
}


//-----------------------------------------------------------------------------------------------
void Renderer::DrawHollowPolygon( int numSides, float centerX, float centerY, float radius, const Rgba& drawColor /*= Rgba::WHITE */ )
{
	std::vector< Vertex_PCT > verts;

	int lineAmount = numSides; // # of triangles used to draw polygon

	float twicePi = 2.0f * pi;

	for ( int i = 0; i <= lineAmount; i++ )
	{
		Vertex_PCT vert;
		vert.m_position.x = centerX + ( radius * cos( i *  twicePi / lineAmount ) );
		vert.m_position.y = centerY + ( radius* sin( i * twicePi / lineAmount ) );
		vert.m_color = drawColor;
		vert.m_texCoords = Vector2( 0.0f, 0.0f );
		verts.push_back( vert );
	}

	BindTexture( Texture::CreateOrGetTexture( "Data/Images/PlainWhiteTexture.png" )[ 0 ] );

	DrawVertexArray_PCT( &verts[ 0 ], verts.size(), PRIMITIVE_LINE_LOOP );
}


//-----------------------------------------------------------------------------------------------
void Renderer::ScaleView( float uniformScale )
{
	glScalef( uniformScale, uniformScale, 1.f );
}


//-----------------------------------------------------------------------------------------------
void Renderer::EnableGLAlphaBlending()
{
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}


//-----------------------------------------------------------------------------------------------
void Renderer::EnableGLAdditiveBlending()
{
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );
}


//-----------------------------------------------------------------------------------------------
void Renderer::RenderCleanup()
{
	// 	glDeleteProgram( gShaderProgram );
	// 	glDeleteVertexArrays( 1, &m_cubeVAO );
	// 	glDeleteBuffers( 1, &m_cubeIBO );
	// 	glDeleteVertexArrays( 1, &m_sphereVAO );
	// 	glDeleteBuffers( 1, &m_sphereIBO );
}


//-----------------------------------------------------------------------------------------------
void Renderer::RenderBufferDestroy( GLuint buffer )
{
	glDeleteBuffers( 1, &buffer );
}


//-----------------------------------------------------------------------------------------------
void Renderer::FramebufferBind( Framebuffer* fbo )
{
	if ( m_activeFBO == fbo )
	{
		return;
	}

	m_activeFBO = fbo;
	if ( fbo == nullptr )
	{
		glBindFramebuffer( GL_FRAMEBUFFER, NULL );
		glViewport( 0, 0, 1600, 900 ); // #FIXME: Replace with GetScreenWidth(), GetScreenHeight()
	}
	else
	{
		glBindFramebuffer( GL_FRAMEBUFFER, fbo->m_fboHandle );
		glViewport( 0, 0, fbo->m_pixelWidth, fbo->m_pixelHeight );

		GLenum renderTargets[ 32 ];
		for ( uint32_t i = 0; i < fbo->m_colorTargets.size(); ++i )
		{
			renderTargets[ i ] = GL_COLOR_ATTACHMENT0 + i;
		}
		glDrawBuffers( fbo->m_colorTargets.size(), renderTargets );
	}
}


//-----------------------------------------------------------------------------------------------
void Renderer::PostProcess( Framebuffer* fbo )
{
	DepthTestingEnabled( true );

	mat44_fl quadModelMatrix = mat44_fl::identity;
	mat44_fl quadViewMatrix = mat44_fl::identity;
	mat44_fl quadProjMatrix = mat44_fl::identity;

	m_fboToDrawWith->SetUniform( "gModel", &quadModelMatrix );
	m_fboToDrawWith->SetUniform( "gView", &quadViewMatrix );
	m_fboToDrawWith->SetUniform( "gProj", &quadProjMatrix );

	m_fboToDrawWith->BindTexture( "gTexDiffuse", fbo->m_colorTargets[ 0 ], 0 );
	m_fboToDrawWith->BindTexture( "gTexDepth", fbo->m_depthStencilTarget, 1 );
	// 	m_fboToDrawWith->BindTexture( "gTexNoise", &m_dissolveTex, 2 );
	// 	m_fboToDrawWith->BindTexture( "gTexMask", &m_maskTex, 3 );

	m_screenQuad->DrawWithMaterial( m_fboToDrawWith, PRIMITIVE_TRIANGLES );

	glActiveTexture( GL_TEXTURE0 );
}


//-----------------------------------------------------------------------------------------------
void Renderer::CreateMaterials()
{
	// Shader files
	std::string postVertexShaderFile = "../../../Engine/Code/Engine/Shaders/post.vert";
	std::string postFragmentShaderFile = "../../../Engine/Code/Engine/Shaders/post_red.frag";
	std::string postRippleFragmentShader = "../../../Engine/Code/Engine/Shaders/post_ripple.frag";
	std::string postObamaFragmentShader = "../../../Engine/Code/Engine/Shaders/post_obama.frag";
	std::string postPassthroughFragmentShader = "../../../Engine/Code/Engine/Shaders/post_passthrough.frag";
	std::string postNightvisionFragmentShader = "../../../Engine/Code/Engine/Shaders/post_nightvision.frag";
	std::string postSwirlFragmentShader = "../../../Engine/Code/Engine/Shaders/post_swirl.frag";
	std::string postGaussianFragmentShader = "../../../Engine/Code/Engine/Shaders/post_gaussian.frag";

	m_fboMaterial0 = new Material( postVertexShaderFile, postPassthroughFragmentShader );
	m_fboMaterial1 = new Material( postVertexShaderFile, postFragmentShaderFile );
	m_fboMaterial2 = new Material( postVertexShaderFile, postRippleFragmentShader );
	m_fboMaterial3 = new Material( postVertexShaderFile, postObamaFragmentShader );
	m_fboMaterial4 = new Material( postVertexShaderFile, postNightvisionFragmentShader );
	m_fboMaterial5 = new Material( postVertexShaderFile, postSwirlFragmentShader );
	m_fboMaterial6 = new Material( postVertexShaderFile, postGaussianFragmentShader );
	m_fboToDrawWith = m_fboMaterial0;

	Vector4 materialColor( 1.0f, 1.0f, 1.0f, 1.0f );
	m_fboToDrawWith->SetUniform( "gColor", &materialColor );

	MakeScreenQuad();

	m_fboToDrawWith->SetUniform( "gEffectState", &g_effectState );

	Texture* diffuseTexture = new Texture( 1600, 900, eTextureFormat_RGBA8 );
	Texture* depthStencilTexture = new Texture( 1600, 900, eTextureFormat_D24S8 );

	m_fbo = new Framebuffer( 1, &diffuseTexture, depthStencilTexture );
}


//-----------------------------------------------------------------------------------------------
void Renderer::FramebufferCopyToBack( Framebuffer* fbo )
{
	if ( fbo == nullptr )
	{
		return;
	}

	GLuint fboHandle = fbo->m_fboHandle;
	glBindFramebuffer( GL_READ_FRAMEBUFFER, fboHandle );
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, NULL );

	uint32_t readWidth = fbo->m_pixelWidth;
	uint32_t readHeight = fbo->m_pixelHeight;

	uint32_t drawWidth = 1600; // #FIXME: GetScreenWidth();
	uint32_t drawHeight = 900; // #FIXME: GetScreenHeight();

	glBlitFramebuffer( 0, 0, // lower left corner pixel
		readWidth, readHeight, // top right corner pixel
		0, 0, // lower left pixel of read buffer
		drawWidth, drawHeight, // top right pixel of read buffer
		GL_COLOR_BUFFER_BIT,
		GL_NEAREST );
}