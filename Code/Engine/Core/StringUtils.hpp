#pragma once

#include <string>
#include <vector>


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );
const std::string GetAsLowerCase( const std::string& mixedCaseString );
const std::string ExtractTokenFromString( const std::string& inputString, 
	const std::string& stringBeforeToken, const std::string& delimiter = "\n" );
std::vector< std::string > TokenizeStringOnDelimiter( const std::string& inputString, 
	const std::string& delimiter = "," );


//-----------------------------------------------------------------------------------------------
void SetTypeFromString( int& outValue, const std::string& valueAsString );
void SetTypeFromString( float& outValue, const std::string& valueAsString );
void SetTypeFromString( char& outValue, const std::string& valueAsString );
void SetTypeFromString( unsigned char& outValue, const std::string& valueAsString );
void SetTypeFromString( std::string& outValue, const std::string& valueAsString );
void SetTypeFromString( bool& outValue, const std::string& valueAsString );