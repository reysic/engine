#include "Engine/Networking/Message.hpp"
#include "Engine/Networking/Session.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


//-----------------------------------------------------------------------------------------------
Message::Message()
	: Packer( m_messageBuffer, 0, MESSAGE_MTU, ENDIANNESS_BIG )
	, m_messageID( 255 ) // Invalid message ID by default
	, m_reliableID( 0 )
	, m_lastSentTime( 0 )
	, m_messageDefinition( nullptr )
	, m_sequenceID( 0 )
{
}


//-----------------------------------------------------------------------------------------------
Message::Message( uint8_t messageType )
	: Packer( m_messageBuffer, 0, MESSAGE_MTU, ENDIANNESS_BIG )
	, m_messageID( messageType )
	, m_reliableID( 0 )
	, m_lastSentTime( 0 )
	, m_messageDefinition( nullptr )
	, m_sequenceID( 0 )
{
}


//-----------------------------------------------------------------------------------------------
Message::Message( Message* message )
	: Packer( m_messageBuffer, 0, MESSAGE_MTU, ENDIANNESS_BIG )
	, m_messageID( message->m_messageID )
	, m_reliableID( message->m_reliableID )
	, m_lastSentTime( message->m_lastSentTime )
	, m_messageDefinition( message->m_messageDefinition )
{
	WriteForwardAlongBuffer( message->m_buffer, message->GetPayloadSize() );
}


//-----------------------------------------------------------------------------------------------
void Message::ResetOffset() const
{
	m_offset = 0;
}


//-----------------------------------------------------------------------------------------------
size_t Message::GetHeaderSize() const
{
	if ( IsReliable() )
	{
		if ( IsOrdered() )
		{
			return ( sizeof( uint8_t ) + sizeof( uint16_t ) + sizeof( uint16_t ) );
		}
		else
		{
			return ( sizeof( uint8_t ) + sizeof( uint16_t ) );
		}
	}
	else
	{
		return sizeof( uint8_t );
	}
}


//-----------------------------------------------------------------------------------------------
size_t Message::GetPayloadSize() const
{
	return m_currentContentSize;
}


//-----------------------------------------------------------------------------------------------
size_t Message::GetTypePayloadSize()
{
	return ( GetPayloadSize() + sizeof( uint8_t ) );
}


//-----------------------------------------------------------------------------------------------
bool Message::IsReliable() const
{
	ASSERT_OR_DIE( m_messageDefinition != nullptr, "Message definition was nullptr!" );
	ASSERT_OR_DIE( m_messageDefinition->optionFlag != OPTION_FLAG_INVALID, "Invalid message option flag!" );
	if ( m_messageDefinition->optionFlag == OPTION_FLAG_RELIABLE )
	{
		return true;
	}
	return false;
}


//-----------------------------------------------------------------------------------------------
bool Message::NeedsConnection() const
{
	ASSERT_OR_DIE( m_messageDefinition != nullptr, "Message definition was nullptr!" );
	ASSERT_OR_DIE( m_messageDefinition->controlFlag != CONTROL_FLAG_INVALID, "Invalid message control flag!" );
	if ( m_messageDefinition->controlFlag == CONTROL_FLAG_CONNECTED )
	{
		return true;
	}
	return false;
}


//-----------------------------------------------------------------------------------------------
void Message::ProcessMessage( const Sender& sender ) const
{
	m_messageDefinition->cb( sender, *this );
}


//-----------------------------------------------------------------------------------------------
bool Message::IsOrdered() const
{
	ASSERT_OR_DIE( m_messageDefinition != nullptr, "Message definition was nullptr!" );
	ASSERT_OR_DIE( m_messageDefinition->optionFlag != OPTION_FLAG_INVALID, "Invalid message option flag!" );
	if ( m_messageDefinition->optionFlag == OPTION_FLAG_ORDERED_RELIABLE )
	{
		return true;
	}
	return false;
}


//-----------------------------------------------------------------------------------------------
bool Message::operator < ( const Message& rhs ) const
{
	if ( m_sequenceID < rhs.m_sequenceID )
	{
		return true;
	}
	return false;
}
