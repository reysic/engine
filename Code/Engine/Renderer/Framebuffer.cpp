#include "Engine/Renderer/Framebuffer.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


//-----------------------------------------------------------------------------------------------
Framebuffer::Framebuffer( size_t colorCount, Texture** colorTargets, Texture* depthStencilTarget )
{
	// You can use a vector instead of Texture** colorTargets and then you don't have to pass
	// in colorCount
	ASSERT_OR_DIE( colorCount > 0, "colorCount <= 0" );
	Texture* color0 = colorTargets[ 0 ];
	uint32_t width = color0->m_texelSize.x;
	uint32_t height = color0->m_texelSize.y;

	for ( uint32_t i = 0; i < colorCount; ++i )
	{
		Texture* color = colorTargets[ i ];
		ASSERT_OR_DIE( ( ( unsigned int ) color->m_texelSize.x == width ) && ( ( unsigned int ) color->m_texelSize.y == height ), "Width/Height Error" );
	}

	if ( nullptr != depthStencilTarget )
	{
		ASSERT_OR_DIE( ( ( unsigned int ) depthStencilTarget->m_texelSize.x == width ) && ( ( unsigned int ) depthStencilTarget->m_texelSize.y == height ), "Width/Height Error" );
	}

	// Create the Framebuffer Object
	GLuint fboHandle;
	glGenFramebuffers( 1, &fboHandle );
	ASSERT_OR_DIE( fboHandle != NULL, "fboHandle = NULL" );

	m_fboHandle = fboHandle;
	m_pixelWidth = width;
	m_pixelHeight = height;

	// Set our textures
	for ( uint32_t i = 0; i < colorCount; ++i )
	{
		m_colorTargets.push_back( colorTargets[ i ] );
	}
	m_depthStencilTarget = depthStencilTarget;

	// OpenGL Initialization... stuff...
	// If you bound a framebuffer to your Renderer -
	// be careful you didn't unbind just now...
	glBindFramebuffer( GL_FRAMEBUFFER, m_fboHandle );

	// Bind our color targets to our FBO
	for ( uint32_t i = 0; i < colorCount; ++i )
	{
		Texture* tex = m_colorTargets[ i ];
		glFramebufferTexture( GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0 + i,
			tex->m_openglTextureID,
			0 /*level - probably mipmap level.. whatever...*/
			);
	}

	// Bind depth stencil if I have it.
	if ( nullptr != m_depthStencilTarget )
	{
		glFramebufferTexture( GL_FRAMEBUFFER,
			GL_DEPTH_STENCIL_ATTACHMENT,
			m_depthStencilTarget->m_openglTextureID,
			0
			);
	}

	// Make sure everything was bound correctly - no errors!
	GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
	{
		//FramebufferDelete( fbo );
		ASSERT_OR_DIE( true, "FBO not bound correctly." );
		//return nullptr;
	}

	// Revert to old state
	glBindFramebuffer( GL_FRAMEBUFFER, NULL );
}


//-----------------------------------------------------------------------------------------------
Framebuffer::Framebuffer( uint32_t width, uint32_t height, std::vector< eTextureFormat> colorFormats, eTextureFormat depthStencilFormat )
{
	width;
	height;
	colorFormats;
	depthStencilFormat;
	// 1. Create the textures,
	// 2. Then do steps in original constructor
	// 3. Have destructor also destroy the textures created
	std::vector< Texture* > textures;
	for ( uint32_t i = 0; i < colorFormats.size(); ++i )
	{
		eTextureFormat format = colorFormats[ i ];
		textures.push_back( new Texture( width, height, format ) );
	}

	m_depthStencilTarget = new Texture( width, height, depthStencilFormat );
}


//-----------------------------------------------------------------------------------------------
Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers( 1, &m_fboHandle );
	delete this;
}