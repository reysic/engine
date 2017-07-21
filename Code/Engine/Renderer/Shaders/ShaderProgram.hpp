#pragma once

#include <string>

#include "Engine/Renderer/OpenGLExtensions.hpp"
#include "Engine/Renderer/Meshes/Mesh.hpp"


//-----------------------------------------------------------------------------------------------
class ShaderProgram
{
public:
	ShaderProgram( std::string vertexShaderFile, std::string fragmentShaderFile );
	GLuint CreateAndLinkProgram( std::string vertexShaderFile, std::string fragmentShaderFile );
	GLuint LoadShader( char const *filename, GLenum shaderType );

public:
	GLuint m_programID;
};