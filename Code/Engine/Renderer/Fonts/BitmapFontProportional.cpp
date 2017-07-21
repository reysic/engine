#include <map>
#include <string>
#include <sstream>

#include "Engine/Renderer/Fonts/BitmapFontProportional.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/Vector3.hpp"


//-----------------------------------------------------------------------------------------------
#define STATIC // Do-nothing indicator that method/member is static in class definition


//-----------------------------------------------------------------------------------------------
/*STATIC std::map< std::string, BitmapFontProportional* > BitmapFontProportional::s_fontRegistry;*/
STATIC StringToBitmapFontProportionalMap BitmapFontProportional::s_fontRegistry;


//-----------------------------------------------------------------------------------------------
STATIC void BitmapFontProportional::DestroyFontRegistry()
{
	for ( auto &p : s_fontRegistry )
	{
		delete p.second;
		p.second = nullptr;
	}
}


//-----------------------------------------------------------------------------------------------
BitmapFontProportional* BitmapFontProportional::CreateOrGetFont( const std::string& BitmapFontProportionalName )
{
	auto foundIter = s_fontRegistry.find( std::hash< std::string >()( BitmapFontProportionalName ) );
	if ( foundIter != s_fontRegistry.end() )
	{
		return foundIter->second;
	}

	BitmapFontProportional* newFont = new BitmapFontProportional( BitmapFontProportionalName );
	s_fontRegistry[ std::hash< std::string >()( BitmapFontProportionalName ) ] = newFont;
	return newFont;

	// #TODO: return nullptr if FNF
}


//-----------------------------------------------------------------------------------------------
Texture* BitmapFontProportional::GetTexture() const
{
	return m_fontTexture;

}


//-----------------------------------------------------------------------------------------------
BitmapFontProportional::BitmapFontProportional( const std::string& BitmapFontProportionalName )
{
	ReadMetaFile( BitmapFontProportionalName ); // m_hasKerning is set here
	std::string fontName = BitmapFontProportionalName;
	fontName.erase( fontName.size() - 4 );
	fontName += "_0.png";
	m_fontTexture = Texture::CreateOrGetTexture( fontName.c_str() );
}


//-----------------------------------------------------------------------------------------------
bool BitmapFontProportional::ReadMetaFile( const std::string& metaFileName )
{
	std::string fontFileData;
	bool fileFound = LoadBinaryFileToStringBuffer( metaFileName, fontFileData );
	if ( fileFound )
	{
		std::stringstream metaFileStream( fontFileData );
		std::vector<std::string> metaFileLines;

		std::string line;
		while ( std::getline( metaFileStream, line ) )
		{
			metaFileLines.push_back( line );
		}

		std::string scaleLine = metaFileLines[ 1 ];
		std::string val = ExtractTokenFromString( scaleLine, "scaleW=", " " );
		m_textureWidth = std::stoi( val );
		val = ExtractTokenFromString( scaleLine, "scaleH=", " " );
		m_textureHeight = std::stoi( val );

		std::string charCountLine = metaFileLines[ 3 ];
		val = ExtractTokenFromString( charCountLine, "chars count=", "\r" );
		int charCount = std::stoi( val );

		for ( int i = 4; i <= charCount + 3; i++ )
		{
			std::string glyphLine = metaFileLines[ i ];
			Glyph* thisGlyph = new Glyph();

			std::string glyphLineToken = ExtractTokenFromString( glyphLine, "char id=", " " );
			thisGlyph->m_charId = std::stoi( glyphLineToken );

			glyphLineToken = ExtractTokenFromString( glyphLine, "x=", " " );
			thisGlyph->m_xPos = std::stoi( glyphLineToken );

			glyphLineToken = ExtractTokenFromString( glyphLine, "y=", " " );
			thisGlyph->m_yPos = std::stoi( glyphLineToken );

			glyphLineToken = ExtractTokenFromString( glyphLine, "width=", " " );
			thisGlyph->m_width = std::stoi( glyphLineToken );

			glyphLineToken = ExtractTokenFromString( glyphLine, "height=", " " );
			thisGlyph->m_height = std::stoi( glyphLineToken );

			glyphLineToken = ExtractTokenFromString( glyphLine, "xoffset=", " " );
			thisGlyph->m_xOffset = std::stoi( glyphLineToken );

			glyphLineToken = ExtractTokenFromString( glyphLine, "yoffset=", " " );
			thisGlyph->m_yOffset = std::stoi( glyphLineToken );

			glyphLineToken = ExtractTokenFromString( glyphLine, "xadvance=", " " );
			thisGlyph->m_xAdvance = std::stoi( glyphLineToken );

			m_glyphRegistry.insert( std::pair < int, Glyph* >( thisGlyph->m_charId, thisGlyph ) );
		}

		int kerningLineIndex = charCount + 3 + 1;

		if ( metaFileLines.size() < ( unsigned int ) ( kerningLineIndex + 1 ) )
		{
			// This font file does not have kerning information
			m_hasKerning = false;
		}
		else
		{
			// This font file does have kerning information
			m_hasKerning = true;

			std::string kerningCountLine = metaFileLines[ kerningLineIndex ];
			val = ExtractTokenFromString( kerningCountLine, "kernings count=", "\r" );
			int kerningCount = std::stoi( val );

			for ( int i = kerningLineIndex + 1; i <= kerningLineIndex + kerningCount; i++ )
			{
				std::string kerningLine = metaFileLines[ i ];

				std::string kerningLineToken = ExtractTokenFromString( kerningLine, "kerning first=", " " );
				int firstKerning = std::stoi( kerningLineToken );

				kerningLineToken = ExtractTokenFromString( kerningLine, "second=", " " );
				int secondKerning = std::stoi( kerningLineToken );

				kerningLineToken = ExtractTokenFromString( kerningLine, "amount=", " " );
				int value = std::stoi( kerningLineToken );

				std::pair<int, int> kerningPair = std::pair<int, int>( firstKerning, secondKerning );

				m_kerningRegistry.insert( std::pair < std::pair<int, int>, int >( kerningPair, value ) );
			}
		}
	}
	return fileFound;
}


//-----------------------------------------------------------------------------------------------
Glyph* BitmapFontProportional::GetGlyph( const char c )
{
	int val = ( int ) c;
	return m_glyphRegistry.at( val );
}


//-----------------------------------------------------------------------------------------------
const bool BitmapFontProportional::GetKerning( const int first, const int second, int& out_val ) const
{
	std::pair<int, int> kerningPair = std::pair<int, int>( first, second );
	std::map< std::pair<int, int>, int >::const_iterator it;
	it = m_kerningRegistry.find( kerningPair );
	if ( it != m_kerningRegistry.end() )
	{
		out_val = it->second;
		return true;
	}
	out_val = 0;
	return false;
}


//-----------------------------------------------------------------------------------------------
float BitmapFontProportional::CalcTextWidth( const std::string& asciiText, const float scale )
{
	Vector3 cursor = Vector3::ZERO;
	Vector3 right( 1.0f, 0.0f, 0.0f );
	Vector3 up( 0.0f, 1.0f, 0.0 );

	Glyph* prev_glyph = nullptr;

	for ( std::string::const_iterator it = asciiText.begin(); it != asciiText.end(); ++it )
	{
		Glyph* thisGlyph = m_glyphRegistry.at( *it );

		if ( thisGlyph == nullptr )
			continue;
		int kerningVal = 0;
		if ( prev_glyph != nullptr )
		{
			GetKerning( prev_glyph->m_charId, thisGlyph->m_charId, kerningVal );
		}

		cursor += ( right * scale * ( float ) kerningVal );

		cursor += right * scale * ( float ) thisGlyph->m_xAdvance;
		prev_glyph = thisGlyph;
	}

	return cursor.x;
}