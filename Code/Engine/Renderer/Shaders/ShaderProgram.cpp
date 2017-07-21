#include <direct.h>
#include <sstream>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Shaders/ShaderProgram.hpp"


//-----------------------------------------------------------------------------------------------
ShaderProgram::ShaderProgram( std::string vertexShaderFile, std::string fragmentShaderFile )
	: m_programID( CreateAndLinkProgram( vertexShaderFile, fragmentShaderFile ) )
{
}


//-----------------------------------------------------------------------------------------------
GLuint ShaderProgram::CreateAndLinkProgram( std::string vertexShaderFile, std::string fragmentShaderFile )
{
	GLuint programID = glCreateProgram();

	GLuint vs = LoadShader( vertexShaderFile.c_str(), GL_VERTEX_SHADER );
	GLuint fs = LoadShader( fragmentShaderFile.c_str(), GL_FRAGMENT_SHADER );

	ASSERT_OR_DIE( programID != NULL, "Error creating shader program" );

	glAttachShader( programID, vs );
	glAttachShader( programID, fs );

	glLinkProgram( programID );

	GLint status;
	glGetProgramiv( programID, GL_LINK_STATUS, &status );
	if ( GL_FALSE == status )
	{
		GLint length;
		glGetProgramiv( programID, GL_INFO_LOG_LENGTH, &length );

		char *errorBuffer = new char[ length + 1 ];
		glGetProgramInfoLog( programID, length, &length, errorBuffer );

		errorBuffer[ length ] = NULL;

		///////////////////////////////////////////////////////////
		std::string errorBufferStr( errorBuffer );

		std::stringstream errorStream( errorBufferStr );
		std::vector<std::string> errorLines;

		std::string line;
		while ( std::getline( errorStream, line ) )
		{
			errorLines.push_back( line );
		}

		for ( auto lineIter = errorLines.begin(); lineIter != errorLines.end(); ++lineIter )
		{
			if ( lineIter->find( "error" ) != std::string::npos )
			{
				//Error Found
				std::string prunedStr = "";
				std::string lineNumber = "0";
				std::string filePathStr( fragmentShaderFile );

				std::string errorText = ExtractTokenFromString( *lineIter, ":", "" );
				errorText = ExtractTokenFromString( errorText, ":", "" );
				prunedStr += errorText;
				prunedStr += "\n";
				char temp[ _MAX_PATH ];
				std::string workingDirStr = _getcwd( temp, MAX_PATH );
				filePathStr = workingDirStr + "\\" + filePathStr;
				DebuggerPrintf( "%s(%d): %s", filePathStr.c_str(), std::stoi( lineNumber ), prunedStr.c_str() );
				const char* versionString = ( const char* ) glGetString( GL_VERSION );
				std::string oglVer = Stringf( "OpenGL version: %s\n", versionString );
				const char* glslString = ( const char* ) glGetString( GL_SHADING_LANGUAGE_VERSION );
				std::string glslVer = Stringf( "GLSL version: %s\n", glslString );
				ASSERT_OR_DIE( false, "GLSL Linking Error!\n\nError: "
					+ prunedStr + "\nLine Number: " + lineNumber
					+ "\n\nIn file: " +
					filePathStr + "\n\n" + errorBufferStr +
					"\n" + oglVer + "\n" + glslVer );
			}
			else
			{
				//Warnings
			}

		}
		///////////////////////////////////////////////////////////

		delete errorBuffer;
		glDeleteProgram( programID );
		return NULL;
	}
	else
	{
		glDetachShader( programID, vs );
		glDetachShader( programID, fs );
	}

	return programID;
}


//-----------------------------------------------------------------------------------------------
GLuint ShaderProgram::LoadShader( char const *filename, GLenum shaderType )
{
	std::vector< unsigned char > buffer;
	LoadBinaryFileToBuffer( filename, buffer );
	buffer.push_back( '\0' );

	GLuint shaderID = glCreateShader( shaderType );
	ASSERT_OR_DIE( shaderID != NULL, "Error creating shader" );

	GLint src_length = buffer.size();
	GLchar* bufferStart = ( GLchar* ) &buffer[ 0 ];
	glShaderSource( shaderID, 1, &bufferStart, &src_length );

	glCompileShader( shaderID );

	GLint status;
	glGetShaderiv( shaderID, GL_COMPILE_STATUS, &status );
	if ( GL_FALSE == status )
	{
		GLint length;
		glGetShaderiv( shaderID, GL_INFO_LOG_LENGTH, &length );

		char *errorBuffer = new char[ length + 1 ];

		glGetShaderInfoLog( shaderID, length, &length, errorBuffer );
		errorBuffer[ length ] = NULL;

		///////////////////////////////////////////////////////////////////////////////////////////
		std::string errorBufferStr( errorBuffer );

		std::stringstream errorStream( errorBufferStr );
		std::vector<std::string> errorLines;

		std::string line;
		while ( std::getline( errorStream, line ) )
		{
			errorLines.push_back( line );
		}

		for ( auto lineIter = errorLines.begin(); lineIter != errorLines.end(); ++lineIter )
		{
			if ( lineIter->find( "error" ) != std::string::npos )
			{
				//Error Found
				std::string prunedStr = "";
				std::string lineNumber = ExtractTokenFromString( *lineIter, "(", ")" );
				lineNumber = std::to_string( std::stoi( lineNumber ) - 1 );
				std::string filePathStr( filename );

				std::string errorText = ExtractTokenFromString( *lineIter, ":", "" );
				errorText = ExtractTokenFromString( errorText, ":", "" );
				prunedStr += errorText;
				prunedStr += "\n";
				char temp[ _MAX_PATH ];
				std::string workingDirStr = _getcwd( temp, MAX_PATH );
				filePathStr = workingDirStr + "\\" + filePathStr;
				DebuggerPrintf( "%s(%d): %s", filePathStr.c_str(), std::stoi( lineNumber ), prunedStr.c_str() );
				const char* versionString = ( const char* ) glGetString( GL_VERSION );
				std::string oglVer = Stringf( "OpenGL version: %s\n", versionString );
				const char* glslString = ( const char* ) glGetString( GL_SHADING_LANGUAGE_VERSION );
				std::string glslVer = Stringf( "GLSL version: %s\n", glslString );
				ASSERT_OR_DIE( false, "GLSL Compilation Error!\n\nError: "
					+ prunedStr + "\nLine Number: " + lineNumber
					+ "\n\nIn file: " +
					filePathStr + "\n\n" + errorBufferStr +
					"\n" + oglVer + "\n" + glslVer );
			}
			else
			{
				//Warnings
			}

		}
		///////////////////////////////////////////////////////////

		delete errorBuffer;
		glDeleteShader( shaderID );
		return 0;
	}

	return shaderID;
}