#include "Engine/Networking/Packer.hpp"


//-----------------------------------------------------------------------------------------------
Packer::Packer( void* buffer, size_t currentContentSize, size_t maxWriteSize, Endianness packerEndianness )
{
	m_buffer = buffer;
	m_maxBufferSize = maxWriteSize;
	m_currentContentSize = currentContentSize;
	m_endianness = packerEndianness;
	m_offset = 0;
}


//-----------------------------------------------------------------------------------------------
Packer::Packer()
{
}

//-----------------------------------------------------------------------------------------------
size_t Packer::AdvanceWrite( size_t size )
{
	m_offset += size;
	return m_offset;
}


//-----------------------------------------------------------------------------------------------
size_t Packer::AdvanceRead( size_t size ) const
{
	m_offset += size;
	return m_offset;
}


//-----------------------------------------------------------------------------------------------
size_t Packer::WriteForwardAlongBuffer( void const *data, size_t const data_size )
{
	memcpy( ( unsigned char* ) m_buffer + m_offset, data, data_size );

	AdvanceWrite( data_size );

	m_currentContentSize += data_size;

	return data_size;
}


//-----------------------------------------------------------------------------------------------
size_t Packer::WriteBackwardAlongBuffer( void const *data, size_t const data_size )
{
	unsigned char* write = ( unsigned char* ) m_buffer + m_offset;
	unsigned char* read = ( unsigned char* ) data + ( data_size - 1 );

	for ( size_t index = 0; index < data_size; ++index )
	{
		*( ( unsigned char* ) write ) = *( ( unsigned char* ) read );
		++write;
		--read;
	}

	m_currentContentSize += data_size;

	AdvanceWrite( data_size );

	return data_size;
}


//-----------------------------------------------------------------------------------------------
size_t Packer::ReadForwardAlongBuffer( void *out_data, size_t const data_size ) const
{
	memcpy( ( unsigned char* ) out_data, ( unsigned char* ) m_buffer + m_offset, data_size );
	AdvanceRead( data_size );
	return data_size;
}


//-----------------------------------------------------------------------------------------------
size_t Packer::ReadBackwardAlongBuffer( void *out_data, size_t const data_size ) const
{
	unsigned char* write = ( unsigned char* ) out_data;
	unsigned char* read = ( unsigned char* ) m_buffer + m_offset + ( data_size - 1 );

	for ( size_t index = 0; index < data_size; ++index )
	{
		*( ( unsigned char* ) write ) = *( ( unsigned char* ) read );
		++write;
		--read;
	}

	AdvanceRead( data_size );

	return data_size;
}


//-----------------------------------------------------------------------------------------------
void Packer::WriteString( const char *string )
{
	if ( string == nullptr || string == NULL )
	{
		//Write< unsigned char >( ( unsigned char* ) 0xff );
	}
	else
	{
		size_t length = strlen( string ) + 1;
		WriteForwardAlongBuffer( string, length );
	}
}


//-----------------------------------------------------------------------------------------------
const char* Packer::ReadString()
{
	if ( GetReadableBytes() < 1 )
	{
		return nullptr;
	}
	unsigned char c;
	Read< unsigned char >( &c );
	if ( c == ( unsigned char ) 0xff )
	{
		return nullptr;
	}
	else
	{
		char* buffer = ( char* ) GetHead() - 1;
		size_t max_size = GetReadableBytes() + 1;
		size_t length = 0;
		while ( length < max_size && ( c != NULL ) )
		{
			++length;
			Read< unsigned char >( &c );
		}
		if ( length <= max_size )
		{
			return buffer;
		}
		else
		{
			return nullptr;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Packer::RewindWrite( const size_t& amount )
{
	m_currentContentSize -= amount;
}