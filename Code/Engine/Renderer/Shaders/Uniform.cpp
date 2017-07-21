#include "Engine/Renderer/Shaders/Uniform.hpp"
#include "Engine/Core/EngineCommon.hpp"


//-----------------------------------------------------------------------------------------------
Uniform::Uniform()
	: m_location( -1 )
	, m_name( "" )
	, m_type( GL_INT )
{
}


//-----------------------------------------------------------------------------------------------
bool Uniform::BindUniform( int numElements )
{
	numElements;
	return true;
}


//-----------------------------------------------------------------------------------------------
UniformInt::UniformInt()
	: m_value( new int( 0 ) )
{
}


//-----------------------------------------------------------------------------------------------
bool UniformInt::BindUniform( int numElements )
{
	UNUSED( numElements );

	if ( m_location >= 0 )
	{
		glUniform1iv( m_location, 1, ( GLint* ) &m_value[ 0 ] );
		return true;
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
UniformFloat::UniformFloat()
	: m_value( new float( 0.0f ) )
{
}


//-----------------------------------------------------------------------------------------------
bool UniformFloat::BindUniform( int numElements )
{
	if ( m_location >= 0 )
	{
		glUniform1fv( m_location, numElements, ( GLfloat* ) &m_value[ 0 ] );
		return true;
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
UniformVec3::UniformVec3()
	: m_value( new Vector3( Vector3::ZERO ) )
{
}


//-----------------------------------------------------------------------------------------------
bool UniformVec3::BindUniform( int numElements )
{
	if ( m_location >= 0 )
	{
		glUniform3fv( m_location, numElements, ( GLfloat* ) &m_value[ 0 ] );
		return true;
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
UniformVec4::UniformVec4()
	: m_value( new Vector4( Vector4::ZERO ) )
{
}


//-----------------------------------------------------------------------------------------------
bool UniformVec4::BindUniform( int numElements )
{
	if ( m_location >= 0 )
	{
		glUniform4fv( m_location, numElements, ( GLfloat* ) &m_value[ 0 ] );
		return true;
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
UniformMat4::UniformMat4()
	: m_value( &mat44_fl::identity )
{
}


//-----------------------------------------------------------------------------------------------
bool UniformMat4::BindUniform( int numElements )
{
	if ( m_location >= 0 )
	{
		glUniformMatrix4fv( m_location, numElements, GL_FALSE, ( GLfloat* ) &m_value[ 0 ] );
		return true;
	}

	return false;
}