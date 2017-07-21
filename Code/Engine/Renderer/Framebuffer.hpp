#pragma once

#include <vector>

#include "Engine/Renderer/OpenGLExtensions.hpp"
#include "Engine/Renderer/Texture.hpp"


//-----------------------------------------------------------------------------------------------
class Framebuffer
{
public:
	Framebuffer( size_t colorCount, Texture** colorTargets, Texture* depthStencilTarget );
	// Alternate Constructor - FBO manages memory for textures.
	Framebuffer( uint32_t width, uint32_t height, std::vector< eTextureFormat> colorFormats, eTextureFormat depthStencilFormat );
	~Framebuffer();

public:
	GLuint m_fboHandle;

	std::vector< Texture* > m_colorTargets;
	Texture* m_depthStencilTarget;

	uint32_t m_pixelWidth;
	uint32_t m_pixelHeight;
};