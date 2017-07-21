#include "io.h"
#include <stdio.h>

#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"


//-----------------------------------------------------------------------------------------------
bool LoadBinaryFileToBuffer( const std::string& filePath, std::vector< unsigned char >& out_buffer )
{
	FILE* file = nullptr;
	fopen_s( &file, filePath.c_str(), "rb" );

	if ( !file )
		return false;

	fseek( file, 0, SEEK_END );
	long size = ftell( file );
	rewind( file );
	out_buffer.resize( size );
	fread( &out_buffer[ 0 ], 1, size, file ); // 1 = sizeof( unsigned char );
	fclose( file );

	return true;
}


//-----------------------------------------------------------------------------------------------
bool LoadBinaryFileToStringBuffer( const std::string& filePath, std::string& out_buffer )
{
	FILE* file = nullptr;
	fopen_s( &file, filePath.c_str(), "rb" );

	if ( !file )
		return false;

	fseek( file, 0, SEEK_END );
	long size = ftell( file );
	rewind( file );
	out_buffer.resize( size );
	fread( &out_buffer[ 0 ], 1, size, file ); // 1 = sizeof( unsigned char );
	fclose( file );

	return true;
}


//-----------------------------------------------------------------------------------------------
bool LoadBinaryFileToBufferFloat( const std::string& filePath, std::vector< float >& out_buffer )
{
	FILE* file = nullptr;
	fopen_s( &file, filePath.c_str(), "rb" );

	if ( !file )
		return false;

	fseek( file, 0, SEEK_END );
	long size = ftell( file );
	rewind( file );
	out_buffer.resize( size );
	fread( &out_buffer[ 0 ], 4, size, file ); // 1 = sizeof( unsigned char );
	fclose( file );

	return true;
}


//-----------------------------------------------------------------------------------------------
bool SaveBufferToBinaryFile( const std::string& filePath, const std::vector< unsigned char >& buffer )
{
	FILE* file = nullptr;
	fopen_s( &file, filePath.c_str(), "wb" );

	if ( !file )
		return false;

	fwrite( &buffer[ 0 ], 1, buffer.size(), file );
	fclose( file );

	return true;
}


//-----------------------------------------------------------------------------------------------
bool SaveBufferToBinaryFileFloat( const std::string& filePath, const std::vector< float >& buffer )
{
	FILE* file = nullptr;
	fopen_s( &file, filePath.c_str(), "wb" );

	if ( !file )
		return false;

	fwrite( &buffer[ 0 ], 4, buffer.size(), file );
	fclose( file );

	return true;
}


//-----------------------------------------------------------------------------------------------
std::vector< std::string > EnumerateFilesInFolder( const std::string& relativeDirectoryPath, 
	const std::string& filePattern )
{
	std::string searchPathPattern = relativeDirectoryPath + "/" + filePattern;
	std::vector< std::string > foundFiles;

	int error = 0;
	struct _finddata_t fileInfo;
	intptr_t searchHandle = _findfirst( searchPathPattern.c_str(), &fileInfo );
	while ( searchHandle != -1 && !error )
	{
		std::string relativePathToFile = Stringf( "%s/%s", relativeDirectoryPath.c_str(), fileInfo.name );
		bool    isDirectory = fileInfo.attrib & _A_SUBDIR ? true : false;
		bool    isHidden = fileInfo.attrib & _A_HIDDEN ? true : false;

		if ( !isDirectory && !isHidden )
			foundFiles.push_back( relativePathToFile );

		error = _findnext( searchHandle, &fileInfo );
	}
	_findclose( searchHandle );

	return foundFiles;
}


//-----------------------------------------------------------------------------------------------
std::vector< std::string > EnumerateFiles( const std::string& baseFolder, const std::string& filePattern, bool recurseSubFolders )
{
	std::string searchPathPattern = baseFolder + "/" + filePattern;
	std::vector< std::string > foundFiles;

	int error = 0;
	struct _finddata_t fileInfo;
	intptr_t searchHandle = _findfirst( searchPathPattern.c_str(), &fileInfo );
	while ( searchHandle != -1 && !error )
	{
		std::string relativePathToFile = Stringf( "%s/%s", baseFolder.c_str(), fileInfo.name );
		bool    isDirectory = fileInfo.attrib & _A_SUBDIR ? true : false;
		bool    isHidden = fileInfo.attrib & _A_HIDDEN ? true : false;

		if ( !isDirectory && !isHidden )
			foundFiles.push_back( relativePathToFile );

		error = _findnext( searchHandle, &fileInfo );
	}
	_findclose( searchHandle );

	if ( recurseSubFolders )
	{
		std::vector< std::string > foundSubFolders;
		foundSubFolders = EnumerateSubFoldersInFolder( baseFolder );

		for ( const std::string& foundSubfolder : foundSubFolders )
		{
			std::vector< std::string > additionalFiles = EnumerateFiles( foundSubfolder, filePattern, true );
			foundFiles.insert( foundFiles.end(), additionalFiles.begin(), additionalFiles.end() );
		}
	}

	return foundFiles;
}


//-----------------------------------------------------------------------------------------------
std::vector< std::string > EnumerateSubFoldersInFolder( const std::string& baseFolder )
{
	std::string searchPathPattern = baseFolder + "/*";
	std::vector< std::string > foundSubFolders;

	int error = 0;
	struct _finddata_t fileInfo;
	intptr_t searchHandle = _findfirst( searchPathPattern.c_str(), &fileInfo );
	while ( searchHandle != -1 && !error )
	{
		std::string relativePathToFile = Stringf( "%s/%s", baseFolder.c_str(), fileInfo.name );
		bool    isDirectory = fileInfo.attrib & _A_SUBDIR ? true : false;
		bool    isHidden = fileInfo.attrib & _A_HIDDEN ? true : false;

		if ( !isHidden && isDirectory )
		{
			if ( strcmp( fileInfo.name, "." ) != 0 && strcmp( fileInfo.name, ".." ) != 0 )
			{
				foundSubFolders.push_back( relativePathToFile );
			}
		}

		error = _findnext( searchHandle, &fileInfo );
	}
	_findclose( searchHandle );

	return foundSubFolders;
}


//-----------------------------------------------------------------------------------------------
bool FileBinaryReader::Open( std::string const &filename )
{
	char const *mode = "rb";

	errno_t error = fopen_s( &fileHandle, filename.c_str(), mode );
	if ( error != 0 ) {
		return false;
	}

	return true;
}


//-----------------------------------------------------------------------------------------------
void FileBinaryReader::Close()
{
	if ( fileHandle != nullptr ) {
		fclose( fileHandle );
		fileHandle = nullptr;
	}
}


//-----------------------------------------------------------------------------------------------
size_t FileBinaryReader::ReadBytes( void *out_buffer, size_t const numBytes )
{
	return fread( out_buffer, sizeof( byte_t ), numBytes, fileHandle );
}


//-----------------------------------------------------------------------------------------------
bool FileBinaryWriter::Open( std::string const &filename, bool append /*= false*/ )
{
	char const *mode;
	if ( append ) {
		mode = "ab"; // append binary mode (adds to end of file)
	}
	else {
		mode = "wb"; // write binary mode (new file) (overwrites file)
	}

	errno_t error = fopen_s( &fileHandle, filename.c_str(), mode );
	if ( error != 0 ) {
		return false;
	}

	return true;
}


//-----------------------------------------------------------------------------------------------
void FileBinaryWriter::Close()
{
	if ( fileHandle != nullptr ) {
		fclose( fileHandle );
		fileHandle = nullptr;
	}
}


//-----------------------------------------------------------------------------------------------
void FileBinaryWriter::Flush()
{
	if ( fileHandle != nullptr )
	{
		fflush( fileHandle );
	}
}


//-----------------------------------------------------------------------------------------------
size_t FileBinaryWriter::WriteBytes( void const *src, size_t const numBytes )
{
	return fwrite( src, sizeof( byte_t ), numBytes, fileHandle );
}


//-----------------------------------------------------------------------------------------------
bool IBinaryWriter::WriteString( std::string const &string )
{
	size_t length = string.size();
	size_t bufferLength = length + 1;
	//Write how much data I'm going to write, then write the data
	return Write<uint32_t>( bufferLength ) && ( WriteBytes( &string[ 0 ], bufferLength ) == bufferLength );
}


//-----------------------------------------------------------------------------------------------
bool IBinaryWriter::WriteStringText( std::string const &string )
{
	size_t length = string.size();
	size_t bufferLength = length;
	return ( WriteBytes( &string[ 0 ], bufferLength ) == bufferLength );
}


//-----------------------------------------------------------------------------------------------
bool IBinaryWriter::WriteFloats( std::vector< float > const &floats )
{
	size_t len = floats.size();
	if ( len == 0 )
	{
		return Write<uint32_t>( len );
	}
	return Write<uint32_t>( len ) && ( WriteBytes( &floats[ 0 ], len ) == len );
}


//-----------------------------------------------------------------------------------------------
bool IBinaryWriter::WriteInts( std::vector< int > const &ints )
{
	size_t len = ints.size();
	if ( len == 0 )
	{
		return Write<uint32_t>( len );
	}
	return Write<uint32_t>( len ) && ( WriteBytes( &ints[ 0 ], len ) == len );
}


//-----------------------------------------------------------------------------------------------
size_t IBinaryReader::ReadString( std::string *out_buffer )
{
	uint32_t numBytes;
	Read<uint32_t>( &numBytes );
	out_buffer->resize( numBytes );
	return ReadBytes( &( *out_buffer )[ 0 ], numBytes );
}


//-----------------------------------------------------------------------------------------------
size_t IBinaryReader::ReadFloats( std::vector<float> *out_buffer )
{
	uint32_t len = 0;
	Read( &len );

	if ( len != 0 )
	{
		out_buffer->resize( len );
		ReadBytes( &( *out_buffer )[ 0 ], len );
	}

	return len;
}


//-----------------------------------------------------------------------------------------------
size_t IBinaryReader::ReadInts( std::vector<int> *out_buffer )
{
	uint32_t len = 0;
	Read( &len );

	if ( len != 0 )
	{
		out_buffer->resize( len );
		ReadBytes( &( *out_buffer )[ 0 ], len );
	}

	return len;
}


//-----------------------------------------------------------------------------------------------
void ByteSwap( void *data, size_t const dataSize )
{
	// #TODO: Implement byte swap
	UNUSED( data );
	UNUSED( dataSize );
}