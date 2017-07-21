#pragma once

#include <vector>
#include <string>

#include "Engine/Renderer/Shaders/Uniform.hpp"
#include "Engine/Renderer/Shaders/Attribute.hpp"
#include "Engine/Renderer/Shaders/ShaderProgram.hpp"
#include "Engine/Renderer/Lights/Light.hpp"


//-----------------------------------------------------------------------------------------------
class Material
{
public:
	Material( std::string vertexShaderFile, std::string fragmentShaderFile );
	Uniform* GetNewUniformByType( GLenum type );
	Attribute* GetNewAttributeByType( GLenum type );
	void SetUniform( std::string name, void* value, int numElements = 1 );
	void SetAttribute( std::string name );
	void SetAttribute( std::string name, GLsizei stride, const GLvoid* pointer );
	GLuint CreateSampler( GLenum min_filter, GLenum mag_filter, GLenum u_wrap, GLenum v_wrap );
	void Material::BindTexture( std::string name, void* value, const int port );
	void SetLight( Light& light );
	void SetLights( std::vector< Light > lights );

public:
	ShaderProgram* m_shaderProgram;
	std::vector< Uniform* > m_uniforms;
	std::vector< Attribute* > m_attributes;
	GLuint m_samplerID;
};