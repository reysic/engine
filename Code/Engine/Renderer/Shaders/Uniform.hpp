#pragma once

#include <string>

#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Matrix4x4.hpp"
#include "Engine/Renderer/OpenGLExtensions.hpp"


//-----------------------------------------------------------------------------------------------
class Uniform
{
public:
	Uniform();
	virtual bool BindUniform( int numElements = 1 );

public:
	GLint m_location;
	std::string m_name;
	GLenum m_type;
};


//-----------------------------------------------------------------------------------------------
class UniformInt : public Uniform
{
public:
	UniformInt();
	bool BindUniform( int numElements = 1 );

public:
	int* m_value;
};


//-----------------------------------------------------------------------------------------------
class UniformFloat : public Uniform
{
public:
	UniformFloat();
	bool BindUniform( int numElements = 1 );

public:
	float* m_value;
};


//-----------------------------------------------------------------------------------------------
class UniformVec3 : public Uniform
{
public:
	UniformVec3();
	bool BindUniform( int numElements = 1 );

public:
	Vector3* m_value;
};


//-----------------------------------------------------------------------------------------------
class UniformVec4 : public Uniform
{
public:
	UniformVec4();
	bool BindUniform( int numElements = 1 );

public:
	Vector4* m_value;
};


//-----------------------------------------------------------------------------------------------
class UniformMat4 : public Uniform
{
public:
	UniformMat4();
	bool BindUniform( int numElements = 1 );

public:
	mat44_fl* m_value;
};