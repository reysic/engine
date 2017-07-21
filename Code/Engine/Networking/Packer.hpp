#pragma once

#include "Engine/Core/EngineCommon.hpp"


//-----------------------------------------------------------------------------------------------
class Packer
{
public:
	Packer();
	Packer( void* buffer, size_t currentContentSize, size_t maxWriteSize, Endianness packerEndianness );
	size_t AdvanceWrite( size_t size );
	size_t AdvanceRead( size_t size ) const;
	template < typename T >
	size_t Write( T const& T )
	{
		const size_t data_size = sizeof( T );
		if ( GetWritableBytes() >= data_size )
		{
			if ( g_engineEndianness == m_endianness )
			{
				return WriteForwardAlongBuffer( &T, data_size );
			}
			else
			{
				return WriteBackwardAlongBuffer( &T, data_size );
			}
		}
		else
		{
			return 0;
		}
	}
	template < typename T >
	size_t Read( T *out_data ) const
	{
		const size_t data_size = sizeof( T );
		if ( GetReadableBytes() >= data_size )
		{
			if ( g_engineEndianness == m_endianness )
			{
				return ReadForwardAlongBuffer( out_data, data_size );
			}
			else
			{
				return ReadBackwardAlongBuffer( out_data, data_size );
			}
		}
		else
		{
			return 0;
		}
	}
	template <typename T>
	size_t Overwrite( size_t& location, void* data )
	{
		const size_t dataSize = sizeof( T );
		size_t writtenAmount = 0u;

		size_t oldOffset = m_offset;
		m_offset = location;

		if ( g_engineEndianness == m_endianness )
		{
			writtenAmount = WriteForwardAlongBuffer( data, dataSize );
		}
		else
		{
			writtenAmount = WriteBackwardAlongBuffer( data, dataSize );
		}

		RewindWrite( dataSize );
		m_offset = oldOffset;
		return writtenAmount;
	}
	void* GetHead() const { return ( ( unsigned char* ) m_buffer + m_offset ); }
	size_t GetReadableBytes() const { return m_currentContentSize - m_offset; }
	size_t GetTotalReadableBytes() const { return m_currentContentSize; }
	size_t GetWritableBytes() const { return GetTotalWritableBytes() - m_offset; }
	size_t GetTotalWritableBytes() const { return m_maxBufferSize; }
	size_t WriteForwardAlongBuffer( void const *data, size_t const data_size );
	size_t WriteBackwardAlongBuffer( void const *data, size_t const data_size );
	size_t ReadForwardAlongBuffer( void *out_data, size_t const data_size ) const;
	size_t ReadBackwardAlongBuffer( void *out_data, size_t const data_size ) const;
	void WriteString( const char *string );
	const char* ReadString();
	void RewindWrite( const size_t& amount );


public:
	void* m_buffer;
	size_t m_maxBufferSize; // can only write up to this size, buffer size, total
	size_t m_currentContentSize; // can only read up to this size, starts at 0 on creation, how
						         // much is valid data
	Endianness m_endianness;


	mutable size_t m_offset; // start of memory where currently reading or writing, can
							 // write up to buffer's size, can read up m_maxReadSize (varies
							 // with message ID)
};