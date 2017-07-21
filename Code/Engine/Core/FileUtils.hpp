#pragma once

#include <string>
#include <vector>

#include "Engine/Core/EngineCommon.hpp"


//-----------------------------------------------------------------------------------------------
enum EndianMode
{
	LITTLE_ENDIAN,
	BIG_ENDIAN,
};


//-----------------------------------------------------------------------------------------------
bool LoadBinaryFileToBuffer( const std::string& filePath, std::vector< unsigned char >& out_buffer );
bool LoadBinaryFileToStringBuffer( const std::string& filePath, std::string& out_buffer );
bool LoadBinaryFileToBufferFloat( const std::string& filePath, std::vector< float >& out_buffer );
bool SaveBufferToBinaryFile( const std::string& filePath, const std::vector< unsigned char >& buffer );
bool SaveBufferToBinaryFileFloat( const std::string& filePath, const std::vector< float >& buffer );
std::vector< std::string > EnumerateFilesInFolder( const std::string& relativeDirectoryPath, 
	const std::string& filePattern ); // filePattern can be Data/Environments/*.Environments.xml
std::vector< std::string > EnumerateFiles( const std::string& baseFolder,
	const std::string& filePattern, bool recurseSubFolders );
std::vector< std::string > EnumerateSubFoldersInFolder( const std::string& baseFolder );



typedef unsigned char byte_t;
//-------------------------------------------------------------------------------------------------
// Binary Reader/Writer Interfaces
//-------------------------------------------------------------------------------------------------
class IBinaryReader
{
private:
	EndianMode endianMode;

public:
	virtual size_t ReadBytes( void *out_buffer, size_t const numBytes ) = 0;

	void SetEndianess( EndianMode mode ) { endianMode = mode; }
	size_t ReadString( std::string *out_buffer );
	size_t ReadFloats( std::vector< float > *out_buffer );
	size_t ReadInts( std::vector< int > *out_buffer );

	template <typename DataType>
	bool Read( DataType *data )
	{
		//#TODO: add Swapping for Endianness
		return ReadBytes( data, sizeof( DataType ) ) == sizeof( DataType );
	}
};


//-------------------------------------------------------------------------------------------------
//Interface. Do not implement anything here.
class IBinaryWriter
{
private:
	EndianMode endianMode;

public:
	virtual size_t WriteBytes( void const *src, size_t const numBytes ) = 0;

	void SetEndianess( EndianMode mode ) { endianMode = mode; }
	bool WriteString( std::string const &string );
	bool WriteStringText( std::string const &string );
	bool WriteFloats( std::vector< float > const &floats );
	bool WriteInts( std::vector< int > const &ints );
	EndianMode GetLocalEndianMode() { return LITTLE_ENDIAN; };

	template<typename DataType>
	bool Write( DataType const &v )
	{
		DataType copy = v;
		if ( GetLocalEndianMode() == endianMode )
		{
			ByteSwap( &copy, sizeof( DataType ) );
		}
		return WriteBytes( &copy, sizeof( DataType ) ) == sizeof( DataType );
	}
};


//-------------------------------------------------------------------------------------------------
// File Reader/Writer Classes
//-------------------------------------------------------------------------------------------------
class FileBinaryReader
	: public IBinaryReader
{
public:
	FILE *fileHandle;

public:
	FileBinaryReader() {};

	bool Open( std::string const &filename );
	void Close();

public:
	virtual size_t ReadBytes( void *out_buffer, size_t const numBytes ) override;
};


//-------------------------------------------------------------------------------------------------
class FileBinaryWriter
	: public IBinaryWriter
{
public:
	FILE *fileHandle;

public:
	FileBinaryWriter() {};

	bool Open( std::string const &filename, bool append = false );
	void Close();
	void Flush();

public:
	virtual size_t WriteBytes( void const *src, size_t const numBytes ) override;
};


//-----------------------------------------------------------------------------------------------
void ByteSwap( void *data, size_t const dataSize );