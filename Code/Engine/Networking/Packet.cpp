#include "Engine/Networking/Packet.hpp"
#include "Engine/Networking/Message.hpp"
#include "Engine/Networking/NetworkingSystem.hpp"
#include "Engine/Networking/Session.hpp"


//-----------------------------------------------------------------------------------------------
Packet::Packet()
	: Packer( m_buffer, 0, MAX_PACKET_SIZE, ENDIANNESS_BIG )
	, m_numberOfMessages( 0 )
	, m_ack( INVALID_PACKET_ACK )
	, m_highestReceivedAck( INVALID_PACKET_ACK )
	, m_previousReceivedAcksBitfield( 0 )
{
}


//-----------------------------------------------------------------------------------------------
void Packet::SetContentSizeFromBuffer()
{
	m_currentContentSize = MAX_PACKET_SIZE;
	m_offset = 0;

	size_t totalSize = Read< uint8_t >( &m_fromConnectionIndex );
	totalSize += Read< uint16_t >( &m_ack );
	totalSize += Read< uint16_t >( &m_highestReceivedAck );
	totalSize += Read< uint16_t >( &m_previousReceivedAcksBitfield );
	totalSize += Read< uint8_t >( &m_numberOfMessages );

	for ( uint8_t i = 0; i < m_numberOfMessages; i++ )
	{
		// Read the next message's length
		short msgLen;
		totalSize += Read< short >( &msgLen );
		size_t msgLenSize = ( size_t ) msgLen;
		m_offset += msgLenSize;
		totalSize += msgLenSize;
	}

	m_currentContentSize = totalSize;
	m_offset = 0;
}


//-----------------------------------------------------------------------------------------------
bool Packet::CanWriteMessageToPacket( Message* messageToWrite )
{
	return ( GetWritableBytes() >= messageToWrite->m_currentContentSize );
}


//-----------------------------------------------------------------------------------------------
void Packet::WriteMessageToPacket( const Message* messageToWrite )
{
	const uint16_t messageTotalSize = ( uint16_t ) messageToWrite->GetHeaderSize() + ( uint16_t ) messageToWrite->GetPayloadSize();
	Write< uint16_t >( messageTotalSize );
	Write< uint8_t >( messageToWrite->m_messageID );
	if ( messageToWrite->IsReliable() )
	{
		Write< uint16_t >( messageToWrite->m_reliableID );
		if ( messageToWrite->IsOrdered() )
		{
			Write< uint16_t >( messageToWrite->m_sequenceID );
		}
	}
	WriteForwardAlongBuffer( messageToWrite->m_buffer, messageToWrite->GetPayloadSize() );
}


//-----------------------------------------------------------------------------------------------
void Packet::ReadMessageFromPacket( Message* messageToRead )
{
	uint16_t messageTotalSize;
	Read< uint16_t >( &messageTotalSize );
	Read< uint8_t >( &messageToRead->m_messageID );
	messageToRead->m_messageDefinition = NetworkingSystem::GetLocalSession()->FindDefinition( messageToRead->m_messageID ); // How call function on session?
	if ( messageToRead->IsReliable() )
	{
		Read< uint16_t >( &messageToRead->m_reliableID );
		if ( messageToRead->IsOrdered() )
		{
			Read< uint16_t >( &messageToRead->m_sequenceID );
		}
	}
	size_t payloadSize = ( size_t ) messageTotalSize - messageToRead->GetHeaderSize();
	ReadForwardAlongBuffer( messageToRead->m_buffer, payloadSize );
	messageToRead->m_currentContentSize = payloadSize;
}