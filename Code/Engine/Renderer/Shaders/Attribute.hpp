#pragma once

#include <string>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Matrix4x4.hpp"
#include "Engine/Renderer/OpenGLExtensions.hpp"


//-----------------------------------------------------------------------------------------------
class Attribute
{
public:
	Attribute();
	virtual bool BindAttribute();
public:
	GLint m_location;
	std::string m_name;
	GLenum m_type;
};


//-----------------------------------------------------------------------------------------------
class AttributeVec2 : public Attribute
{
public:
	AttributeVec2();
	bool BindAttribute();

public:
	Vector2 m_value;
};


//-----------------------------------------------------------------------------------------------
class AttributeVec3 : public Attribute
{
public:
	AttributeVec3();
	bool BindAttribute();

public:
	Vector3 m_value;
};


//-----------------------------------------------------------------------------------------------
class AttributeVec4 : public Attribute
{
public:
	AttributeVec4();
	bool BindAttribute();

public:
	Vector4 m_value;
};