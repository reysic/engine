#include "Engine/Renderer/Material.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


//-----------------------------------------------------------------------------------------------
Material::Material( std::string vertexShaderFile, std::string fragmentShaderFile )
	: m_shaderProgram( new ShaderProgram( vertexShaderFile, fragmentShaderFile ) )
{
	m_samplerID = CreateSampler( GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT );
	GLint numberOfActiveUniforms = 0;
	GLint numberOfActiveAttributes = 0;
	GLint uniformMaxLength = 0;
	GLuint programID = m_shaderProgram->m_programID;
	glUseProgram( programID );
	glGetProgramiv( programID, GL_ACTIVE_UNIFORMS, &numberOfActiveUniforms );
	glGetProgramiv( programID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformMaxLength );

	for ( int i = 0; i < numberOfActiveUniforms; ++i )
	{
		GLsizei length;
		GLint size;
		GLenum type;
		std::vector< GLchar > nameData( 256 );
		glGetActiveUniform( programID, i, nameData.size(), &length, &size, &type, &nameData[ 0 ] );

		GLint location = -1;
		location = glGetUniformLocation( programID, &nameData[ 0 ] );

		if ( size > 1 )
		{
			char* pos = strchr( &nameData[ 0 ], '[' );
			int arrayCharLength = strlen( pos );
			*pos = NULL;
			length -= arrayCharLength;
		}

		Uniform* newUniform = GetNewUniformByType( type );
		newUniform->m_type = type;
		newUniform->m_name = std::string( nameData.begin(), nameData.begin() + length );
		newUniform->m_location = location;
		m_uniforms.push_back( newUniform );
	}

	glGetProgramiv( programID, GL_ACTIVE_ATTRIBUTES, &numberOfActiveAttributes );
	for ( int i = 0; i < numberOfActiveAttributes; ++i )
	{
		GLsizei length;
		GLint size;
		GLenum type;
		std::vector< GLchar > nameData( 256 );
		glGetActiveAttrib( programID, i, nameData.size(), &length, &size, &type, &nameData[ 0 ] );

		GLint location = -1;
		location = glGetAttribLocation( programID, &nameData[ 0 ] );

		Attribute* newAttrib = GetNewAttributeByType( type );
		newAttrib->m_type = type;
		newAttrib->m_name = std::string( nameData.begin(), nameData.begin() + length );
		newAttrib->m_location = location;
		m_attributes.push_back( newAttrib );
	}
}


//-----------------------------------------------------------------------------------------------
Uniform* Material::GetNewUniformByType( GLenum type )
{
	switch ( type )
	{
	case GL_INT:
		return new UniformInt();
		break;
	case GL_FLOAT:
		return new UniformFloat();
		break;
	case GL_FLOAT_VEC3:
		return new UniformVec3();
		break;
	case GL_FLOAT_VEC4:
		return new UniformVec4();
		break;
	case GL_FLOAT_MAT4:
		return new UniformMat4();
		break;
	case GL_SAMPLER_2D:
		return new UniformInt();
		break;
	default:
		return nullptr;
		break;
	}
}


//-----------------------------------------------------------------------------------------------
Attribute* Material::GetNewAttributeByType( GLenum type )
{
	switch ( type )
	{
	case GL_FLOAT_VEC2:
		return new AttributeVec2();
		break;
	case GL_FLOAT_VEC3:
		return new AttributeVec3();
		break;
	case GL_FLOAT_VEC4:
		return new AttributeVec4();
		break;
	default:
		return nullptr;
		break;
	}
}


//-----------------------------------------------------------------------------------------------
void Material::SetUniform( std::string name, void* value, int numElements )
{
	bool isFound = false;
	for ( auto nameIter = m_uniforms.begin(); nameIter != m_uniforms.end(); ++nameIter )
	{
		std::string uniformName = ( *nameIter )->m_name;
		if ( uniformName == name )
		{
			GLenum type = ( *nameIter )->m_type;

			switch ( type )
			{
			case GL_INT:
			{
				int* v = static_cast< int* >( value );
				UniformInt* newUni = dynamic_cast< UniformInt* >( ( *nameIter ) );
				newUni->m_value = v;
				glUseProgram( m_shaderProgram->m_programID );
				newUni->BindUniform( numElements );
				isFound = true;
				break;
			}
			case GL_FLOAT:
			{
				float* v = static_cast< float* >( value );
				UniformFloat* newUni = dynamic_cast< UniformFloat* >( ( *nameIter ) );
				newUni->m_value = v;
				glUseProgram( m_shaderProgram->m_programID );
				newUni->BindUniform( numElements );
				isFound = true;
				break;
			}
			case GL_FLOAT_VEC3:
			{
				Vector3* v = static_cast< Vector3* >( value );
				UniformVec3* newUni = dynamic_cast< UniformVec3* >( ( *nameIter ) );
				newUni->m_value = v;
				glUseProgram( m_shaderProgram->m_programID );
				newUni->BindUniform( numElements );
				isFound = true;
				break;
			}
			case GL_FLOAT_VEC4:
			{
				Vector4* v = static_cast< Vector4* >( value );
				UniformVec4* newUni = dynamic_cast< UniformVec4* >( ( *nameIter ) );
				newUni->m_value = v;
				glUseProgram( m_shaderProgram->m_programID );
				newUni->BindUniform( numElements );
				isFound = true;
				break;
			}
			case GL_FLOAT_MAT4:
			{
				mat44_fl* v = static_cast< mat44_fl* >( value );
				UniformMat4* newUni = dynamic_cast< UniformMat4* >( ( *nameIter ) );
				newUni->m_value = v;
				glUseProgram( m_shaderProgram->m_programID );
				newUni->BindUniform( numElements );
				isFound = true;
				break;
			}
			case GL_SAMPLER_2D:
			{
				int* v = static_cast< int* >( value );
				UniformInt* newUni = dynamic_cast< UniformInt* >( ( *nameIter ) );
				newUni->m_value = v;
				glUseProgram( m_shaderProgram->m_programID );
				newUni->BindUniform( numElements );
				isFound = true;
				break;
			}
			default:
				break;
			}

			break;
		}
	}
	if ( !isFound )
	{
		DebuggerPrintf( "%s not found.\n", name.c_str() );
	}
}


//-----------------------------------------------------------------------------------------------
void Material::SetAttribute( std::string name )
{
	for ( auto nameIter = m_attributes.begin(); nameIter != m_attributes.end(); ++nameIter )
	{
		std::string attribName = ( *nameIter )->m_name;
		if ( attribName == name )
		{
			GLenum type = ( *nameIter )->m_type;

			switch ( type )
			{
			case GL_FLOAT_VEC2:
			{
				AttributeVec2* newAttrib = dynamic_cast< AttributeVec2* >( ( *nameIter ) );
				glUseProgram( m_shaderProgram->m_programID );
				newAttrib->BindAttribute();
				break;
			}
			case GL_FLOAT_VEC3:
			{
				AttributeVec3* newAttrib = dynamic_cast< AttributeVec3* >( ( *nameIter ) );
				glUseProgram( m_shaderProgram->m_programID );
				newAttrib->BindAttribute();
				break;
			}
			case GL_FLOAT_VEC4:
			{
				AttributeVec4* newAttrib = dynamic_cast< AttributeVec4* >( ( *nameIter ) );
				glUseProgram( m_shaderProgram->m_programID );
				newAttrib->BindAttribute();
				break;
			}
			default:
				break;
			}

			break;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Material::SetAttribute( std::string name, GLsizei stride, const GLvoid* pointer )
{
	for ( auto nameIter = m_attributes.begin(); nameIter != m_attributes.end(); ++nameIter )
	{
		std::string attribName = ( *nameIter )->m_name;
		if ( attribName == name )
		{
			GLenum type = ( *nameIter )->m_type;

			switch ( type )
			{
			case GL_FLOAT_VEC2:
			{
				AttributeVec2* newAttrib = dynamic_cast< AttributeVec2* >( ( *nameIter ) );
				glUseProgram( m_shaderProgram->m_programID );
				if ( newAttrib->m_location >= 0 )
				{
					glEnableVertexAttribArray( newAttrib->m_location );
					glVertexAttribPointer( newAttrib->m_location, 2, GL_FLOAT, GL_FALSE, stride, pointer );

				}
				break;
			}
			case GL_FLOAT_VEC3:
			{
				AttributeVec3* newAttrib = dynamic_cast< AttributeVec3* >( ( *nameIter ) );
				glUseProgram( m_shaderProgram->m_programID );
				if ( newAttrib->m_location >= 0 )
				{
					glEnableVertexAttribArray( newAttrib->m_location );
					glVertexAttribPointer( newAttrib->m_location, 3, GL_FLOAT, GL_FALSE, stride, pointer );

				}
				break;
			}
			case GL_FLOAT_VEC4:
			{
				AttributeVec4* newAttrib = dynamic_cast< AttributeVec4* >( ( *nameIter ) );
				glUseProgram( m_shaderProgram->m_programID );
				if ( newAttrib->m_location >= 0 )
				{
					glEnableVertexAttribArray( newAttrib->m_location );
					glVertexAttribPointer( newAttrib->m_location, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride, pointer );

				}
				break;
			}
			default:
				break;
			}

			break;
		}
	}
}


//-----------------------------------------------------------------------------------------------
GLuint Material::CreateSampler( GLenum min_filter, GLenum mag_filter, GLenum u_wrap, GLenum v_wrap )
{
	GLuint id;
	glGenSamplers( 1, &id );

	glSamplerParameteri( id, GL_TEXTURE_MIN_FILTER, min_filter );
	glSamplerParameteri( id, GL_TEXTURE_MAG_FILTER, mag_filter );
	glSamplerParameteri( id, GL_TEXTURE_WRAP_S, u_wrap );
	glSamplerParameteri( id, GL_TEXTURE_WRAP_T, v_wrap );

	return id;
}


//-----------------------------------------------------------------------------------------------
void Material::BindTexture( std::string name, void* value, const int port )
{
	for ( auto nameIter = m_uniforms.begin(); nameIter != m_uniforms.end(); ++nameIter )
	{
		std::string uniformName = ( *nameIter )->m_name;
		if ( uniformName == name )
		{
			GLenum type = ( *nameIter )->m_type;

			if ( type == GL_SAMPLER_2D )
			{
				int* v = static_cast< int* >( value );
				UniformInt* newUni = dynamic_cast< UniformInt* >( ( *nameIter ) );

				glActiveTexture( GL_TEXTURE0 + port );
				glBindTexture( GL_TEXTURE_2D, *v );
				glBindSampler( port, m_samplerID );
				*( newUni->m_value ) = port;
				newUni->BindUniform();

			}

			break;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Material::SetLight( Light& light )
{
	SetUniform( "gLightColor", &light.m_color );
	SetUniform( "gLightPosition", &light.m_position );
}


//-----------------------------------------------------------------------------------------------
void Material::SetLights( std::vector< Light > lights )
{
	int vectorSize = lights.size();
	Vector4 lightColors[ 16 ];
	Vector3 lightPositions[ 16 ];
	Vector3 lightDirections[ 16 ];
	float	minLightDistance[ 16 ];
	float	maxLightDistance[ 16 ];
	float	powerAtMinDistance[ 16 ];
	float	powerAtMaxDistance[ 16 ];
	float	directionalInterp[ 16 ];
	float   thetaInner[ 16 ];
	float	thetaOuter[ 16 ];
	float 	thetaInnerPower[ 16 ];
	float 	thetaOuterPower[ 16 ];

	for ( int i = 0; i < vectorSize; i++ )
	{
		lightColors[ i ] = lights[ i ].m_color;
		lightPositions[ i ] = lights[ i ].m_position;
		lightDirections[ i ] = lights[ i ].m_direction;
		minLightDistance[ i ] = lights[ i ].m_minLightDistance;
		maxLightDistance[ i ] = lights[ i ].m_maxLightDistance;
		powerAtMinDistance[ i ] = lights[ i ].m_powerAtMinDistance;
		powerAtMaxDistance[ i ] = lights[ i ].m_powerAtMaxDistance;
		directionalInterp[ i ] = lights[ i ].m_directionalInterp;
		thetaInner[ i ] = lights[ i ].m_thetaInner;
		thetaOuter[ i ] = lights[ i ].m_thetaOuter;
		thetaInnerPower[ i ] = lights[ i ].m_thetaInnerPower;
		thetaOuterPower[ i ] = lights[ i ].m_thetaOuterPower;
	}

	SetUniform( "gLightColor", &lightColors[ 0 ], vectorSize );
	SetUniform( "gLightPosition", &lightPositions[ 0 ], vectorSize );
	SetUniform( "gLightDirection", &lightDirections[ 0 ], vectorSize );
	SetUniform( "gNearDistance", &minLightDistance[ 0 ], vectorSize );
	SetUniform( "gFarDistance", &maxLightDistance[ 0 ], vectorSize );
	SetUniform( "gNearFactor", &powerAtMinDistance[ 0 ], vectorSize );
	SetUniform( "gFarFactor", &powerAtMaxDistance[ 0 ], vectorSize );
	SetUniform( "gLightDirectionFactor", &directionalInterp[ 0 ], vectorSize );
	SetUniform( "gInnerAngle", &thetaInner[ 0 ], vectorSize );
	SetUniform( "gOuterAngle", &thetaOuter[ 0 ], vectorSize );
	SetUniform( "gInnerFactor", &thetaInnerPower[ 0 ], vectorSize );
	SetUniform( "gOuterFactor", &thetaOuterPower[ 0 ], vectorSize );
}