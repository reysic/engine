#include "Engine/Core/StringUtils.hpp"
#include <stdarg.h>


//-----------------------------------------------------------------------------------------------
const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const int maxLength, const char* format, ... )
{
	char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if (maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH)
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if (maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH)
		delete[] textLiteral;

	return returnValue;
}


//-----------------------------------------------------------------------------------------------
const std::string GetAsLowerCase( const std::string& mixedCaseString )
{
	std::string lowerCaseVersion;

	for ( unsigned int i = 0; i < mixedCaseString.length(); i++ )
	{
		lowerCaseVersion[ i ] = (char) tolower( mixedCaseString[ i ] );
	}

	return lowerCaseVersion;
}


//-----------------------------------------------------------------------------------------------
const std::string ExtractTokenFromString( const std::string& inputString, const std::string& stringBeforeToken, const std::string& delimiter )
{
	std::string token;

	int stringBeforeTokenPos = inputString.find( stringBeforeToken );
	int stringBeforeTokenLength = stringBeforeToken.length();
	int delimiterPos = inputString.find( delimiter, stringBeforeTokenPos );

	int substrStart = stringBeforeTokenPos + stringBeforeTokenLength;
	int substrLength = delimiterPos - ( substrStart );
	token = inputString.substr( substrStart, substrLength );

	return token;
}


//-----------------------------------------------------------------------------------------------
std::vector< std::string > TokenizeStringOnDelimiter( const std::string& inputString, 
	const std::string& delimiter )
{

	size_t found = 1;
	std::string curString = inputString;
	std::vector< std::string > outStrings;

	while ( found != std::string::npos )
	{
		std::string subStr;
		found = curString.find( delimiter );
		if ( found == std::string::npos )
		{
			subStr = curString;
		}
		else
		{
			subStr = curString.substr( 0, found );
			curString.erase( 0, found + 1 );
		}

		if ( subStr != "" )
		{
			outStrings.push_back( subStr );
		}
	}

	return outStrings;
}


//-----------------------------------------------------------------------------------------------
void SetTypeFromString( int& outValue, const std::string& valueAsString )
{
	outValue = atoi( valueAsString.c_str() );
}


//-----------------------------------------------------------------------------------------------
void SetTypeFromString( float& outValue, const std::string& valueAsString )
{
	outValue = ( float ) atof( valueAsString.c_str() );
}


//-----------------------------------------------------------------------------------------------
void SetTypeFromString( char& outValue, const std::string& valueAsString )
{
	outValue = valueAsString[ 0 ];
}


//-----------------------------------------------------------------------------------------------
void SetTypeFromString( unsigned char& outValue, const std::string& valueAsString )
{
	outValue = ( unsigned char ) atoi( valueAsString.c_str() );
}


//-----------------------------------------------------------------------------------------------
void SetTypeFromString( std::string& outValue, const std::string& valueAsString )
{
	outValue = valueAsString;
}


//-----------------------------------------------------------------------------------------------
void SetTypeFromString( bool& outValue, const std::string& valueAsString )
{
	if ( valueAsString == "true" || valueAsString == "1" || valueAsString == "yes" )
	{
		outValue = true;
	}
	else
	{
		outValue = false;
	}
}