#include "Engine/Networking/Connection.hpp"
#include "Engine/Networking/Session.hpp"
#include "Engine/Networking/Message.hpp"
#include "Engine/Networking/Packet.hpp"
#include "Engine/Core/Time.hpp"


//-----------------------------------------------------------------------------------------------
AckBundle::AckBundle()
	: m_ackID( INVALID_PACKET_ACK )
{
}


//-----------------------------------------------------------------------------------------------
void AckBundle::AddReliableID( uint16_t reliableID )
{
	for ( uint16_t thisID : m_sentReliableIDs )
	{
		if ( thisID == reliableID )
		{
			// Don't want to add the same reliable ID
			return;
		}
	}
	m_sentReliableIDs.push_back( reliableID );
}


//-----------------------------------------------------------------------------------------------
Connection::Connection( uint8_t index, Session* session, sockaddr_in address, char guid[] )
	: m_index( index )
	, m_session( session )
	, m_address( address )
	, m_nextSentAck( 0 )
	, m_highestReceivedAck( INVALID_PACKET_ACK )
	, m_previousReceivedAcksBitfield( 0 )
	, m_nextSentReliableID( 0 )
	, m_nextExpectedReliableID( 0 )
	, m_nextSentSequenceID( 0 )
	, m_nextExpectedSequenceID( 0 )
{
	strncpy_s( m_guid, guid, MAX_GUID_LENGTH );
}


//-----------------------------------------------------------------------------------------------
bool Connection::IsMyConnection( uint8_t index )
{
	if ( index == m_index )
	{
		return true;
	}
	else
	{
		return false;
	}
}


//-----------------------------------------------------------------------------------------------
void Connection::AddMessage( Message& msg )
{

	msg.m_messageDefinition = m_session->FindDefinition( msg.m_messageID );
	ASSERT_OR_DIE( msg.m_messageDefinition != nullptr, "messageDefinition = nullptr" );

	Message* copiedMessage = new Message( &msg );

	if ( copiedMessage->IsReliable() )
	{
		if ( copiedMessage->IsOrdered() )
		{
			copiedMessage->m_sequenceID = m_nextSentSequenceID;
			++m_nextSentSequenceID;
		}
		m_unsentReliables.push( copiedMessage );
	}
	else
	{
		m_unreliables.push( copiedMessage );
	}
}


//-----------------------------------------------------------------------------------------------
void Connection::FreeAllUnreliables()
{
	while ( !m_unreliables.empty() )
	{
		Message* thisMessage = m_unreliables.front();
		delete thisMessage;
		m_unreliables.pop();
	}
}


//-----------------------------------------------------------------------------------------------
void Connection::SendPacket()
{
	Packet packet;

	// Write connection index
	packet.Write< uint8_t >( m_session->m_myConnection->m_index );
	packet.m_fromConnectionIndex = m_session->m_myConnection->m_index;

	// Write ack
	uint16_t nextAck = GetNextAck();
	packet.Write< uint16_t >( nextAck );
	packet.m_ack = nextAck;

	// Write highest received ack
	packet.Write< uint16_t >( m_highestReceivedAck );
	packet.m_highestReceivedAck = m_highestReceivedAck;

	// Write previous received ack's bitfield
	packet.Write< uint16_t >( m_previousReceivedAcksBitfield );
	packet.m_previousReceivedAcksBitfield = m_previousReceivedAcksBitfield;

	// Write number of messages
	size_t numMessagesBookmark = packet.m_offset;
	uint8_t numMessagesSent = 0u;
	packet.Write< uint8_t >( numMessagesSent );

	// Get ack bundle
	AckBundle* bundle = MakeAckBundle( nextAck );

	// Send reliable messages before unreliables
	numMessagesSent += ResendSentReliables( packet, bundle );
	numMessagesSent += SendUnsentReliables( packet, bundle );
	numMessagesSent += SendUnreliables( packet );

	FreeAllUnreliables();
	packet.Overwrite< uint8_t >( numMessagesBookmark, &numMessagesSent );
	packet.m_numberOfMessages = numMessagesSent;

	// Send the packet
	m_session->m_packetChannel->SendTo( m_address, packet.m_buffer, 
		packet.GetTotalReadableBytes() );
	m_session->m_timeDataLastSent = GetCurrentTimeSeconds();
}


//-----------------------------------------------------------------------------------------------
Message* Connection::CreateMessageCopy( const Message& msg )
{
	Message* copy = new Message();
	copy->m_messageID = msg.m_messageID;
	if ( msg.IsReliable() )
	{
		copy->m_reliableID = msg.m_reliableID;
	}
	if ( msg.IsOrdered() )
	{
		copy->m_sequenceID = msg.m_sequenceID;
	}
	memcpy( copy->m_messageBuffer, msg.m_messageBuffer, sizeof( msg.m_messageBuffer ) );
	return copy;
}


//-----------------------------------------------------------------------------------------------
uint16_t Connection::GetNextAck()
{
	uint16_t ack = m_nextSentAck;
	m_nextSentAck++;
	if ( m_nextSentAck == INVALID_PACKET_ACK )
	{
		++m_nextSentAck;
	}
	return ack;
}


//-----------------------------------------------------------------------------------------------
AckBundle* Connection::MakeAckBundle( uint16_t ackID )
{
	uint16_t indexIntoArray = ackID % MAX_ACK_BUNDLES;
	AckBundle* bundle = &( m_ackBundles[ indexIntoArray ] );
	bundle->m_ackID = ackID;
	bundle->m_sentReliableIDs.clear(); // Clear contents since could have been used before
	return bundle;
}


//-----------------------------------------------------------------------------------------------
AckBundle* Connection::FindAckBundle( uint16_t ackID )
{
	uint16_t indexIntoArray = ackID % MAX_ACK_BUNDLES;
	AckBundle* bundle = &( m_ackBundles[ indexIntoArray ] );
	if ( bundle->m_ackID == ackID )
	{
		// Is this the same ack?
		return bundle;
	}
	return nullptr;
}


//-----------------------------------------------------------------------------------------------
uint8_t Connection::ResendSentReliables( Packet& packet, AckBundle* bundle )
{
	uint8_t numMessagesSent = 0;

	while ( !m_sentReliables.empty() )
	{
		Message* thisMessage = m_sentReliables.front();
		if ( IsReliableIDConfirmed( thisMessage->m_reliableID ) )
		{
			m_sentReliables.pop();
			delete thisMessage;
			continue;
		}
		 
		if ( MessageIsOld( thisMessage ) && packet.CanWriteMessageToPacket( thisMessage ) )
		{
			m_sentReliables.pop(); // Pop off for processing, stick back at end
			thisMessage->m_lastSentTime = ( uint32_t ) ( GetCurrentTimeSeconds() * 1000.0 );
			packet.WriteMessageToPacket( thisMessage );
			++numMessagesSent;
			bundle->AddReliableID( thisMessage->m_reliableID );
			m_sentReliables.push( thisMessage );
		}
		else
		{
			break;
		}
	}

	return numMessagesSent;
}


//-----------------------------------------------------------------------------------------------
uint8_t Connection::SendUnsentReliables( Packet& packet, AckBundle* bundle )
{
	uint8_t numMessagesSent = 0;

	while ( !m_unsentReliables.empty() && CanSendNewReliable() )
	{
		Message* thisMessage = m_unsentReliables.front();

		if ( packet.CanWriteMessageToPacket( thisMessage ) )
		{
			thisMessage->m_reliableID = GetNextReliableID();
			thisMessage->m_lastSentTime = ( uint32_t ) ( GetCurrentTimeSeconds() * 1000.0 );
			packet.WriteMessageToPacket( thisMessage );
			++numMessagesSent;
			bundle->AddReliableID( thisMessage->m_reliableID );
			m_unsentReliables.pop();
			m_sentReliables.push( thisMessage );
		}
		else
		{
			break;
		}
	}

	return numMessagesSent;
}


//-----------------------------------------------------------------------------------------------
uint8_t Connection::SendUnreliables( Packet& packet )
{
	uint8_t numMessagesSent = 0;

	while ( !m_unreliables.empty() )
	{
		Message* thisMessage = m_unreliables.front();

		if ( packet.CanWriteMessageToPacket( thisMessage ) )
		{
			packet.WriteMessageToPacket( thisMessage );
			++numMessagesSent;
			m_unreliables.pop();
		}
		else
		{
			break;
		}
	}

	return numMessagesSent;
}


//-----------------------------------------------------------------------------------------------
bool Connection::CanSendNewReliable()
{
	return ( m_nextSentReliableID - m_oldestUnconfirmedReliableID < MAX_RELIABLE_RANGE );
}


//-----------------------------------------------------------------------------------------------
uint16_t Connection::GetNextReliableID()
{
	uint16_t reliableID = m_nextSentReliableID;
	m_nextSentReliableID++;

	return reliableID;
}


//-----------------------------------------------------------------------------------------------
bool Connection::IsReliableIDConfirmed( uint16_t reliableID )
{
	for ( uint16_t thisID : m_confirmedReliableIDs )
	{
		if ( thisID == reliableID )
		{
			return true;
		}
	}

	if ( reliableID < m_oldestUnconfirmedReliableID )
	{
		return true;
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
bool Connection::MessageIsOld( Message* message )
{
	uint32_t currentTime = ( uint32_t ) ( GetCurrentTimeSeconds() * 1000.0 );
	return ( ( currentTime - message->m_lastSentTime ) > MAX_MESSAGE_AGE );
}


//-----------------------------------------------------------------------------------------------
bool Connection::HasReceivedReliable( uint16_t reliableID ) const
{
	for ( uint16_t thisID : m_receivedReliableIDs )
	{
		if ( thisID == reliableID )
		{
			return true;
		}
	}
	
	return false;
}


//-----------------------------------------------------------------------------------------------
void Connection::ProcessMessage( const Sender& sender, const Message& message )
{
	if ( message.IsOrdered() )
	{
		ProcessOrderedMessage( sender, message );
	}
	else
	{
		message.ProcessMessage( sender );
	}
}


//-----------------------------------------------------------------------------------------------
bool CompareSequenceIDs( Message* firstMessage, Message* secondMessage )
{
	return ( firstMessage->m_sequenceID < secondMessage->m_sequenceID );
}


//-----------------------------------------------------------------------------------------------
void Connection::ProcessOrderedMessage( const Sender& sender, const Message& message )
{
	if ( m_nextExpectedSequenceID == message.m_sequenceID )
	{
		// Process message
		message.ProcessMessage( sender );

		// Increment next expected sequence ID
		++m_nextExpectedSequenceID;

		// Process and then remove any messages matching sequence ID from vector
		std::vector< Message* >::iterator outOfOrderReceivedIterator;
		for ( outOfOrderReceivedIterator = m_outOfOrderReceivedSequencedMessages.begin();
			outOfOrderReceivedIterator != m_outOfOrderReceivedSequencedMessages.end();
			++outOfOrderReceivedIterator )
		{
			if ( ( *outOfOrderReceivedIterator )->m_sequenceID == m_nextExpectedSequenceID )
			{
				( *outOfOrderReceivedIterator )->ProcessMessage( sender );
				++m_nextExpectedSequenceID;
			}
		}

		// Clear buffer as all messages should have been processed
		m_outOfOrderReceivedSequencedMessages.clear();
	}
	else
	{
		Message* copy = CreateMessageCopy( message );
		if ( m_outOfOrderReceivedSequencedMessages.size() == 0 )
		{
			// If no elements in vector, simply push back
			m_outOfOrderReceivedSequencedMessages.push_back( copy );
		}
		else
		{
			bool insertedIntoBuffer = false;
			std::vector< Message* >::iterator outOfOrderReceivedIterator;
			for ( outOfOrderReceivedIterator = m_outOfOrderReceivedSequencedMessages.begin();
				outOfOrderReceivedIterator != m_outOfOrderReceivedSequencedMessages.end();
				++outOfOrderReceivedIterator )
			{
				if ( copy->m_sequenceID < ( *outOfOrderReceivedIterator )->m_sequenceID )
				{
					// Message's sequence ID is less than sequence ID of message in buffer
					m_outOfOrderReceivedSequencedMessages.insert( outOfOrderReceivedIterator, copy );
					insertedIntoBuffer = true;
					break;
				}
			}

			if ( !insertedIntoBuffer )
			{
				// Message's sequence ID is greater than sequence ID of all other messages in buffer,
				// simply push back
				m_outOfOrderReceivedSequencedMessages.push_back( copy );
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Connection::MarkMessageReceived( const Message& message )
{
	if ( message.IsReliable() )
	{
		MarkReliableReceived( message.m_reliableID );
	}
}


//-----------------------------------------------------------------------------------------------
void Connection::MarkReliableReceived( uint16_t reliableID )
{
	if ( GreaterThanOrEqualToCyclic( reliableID, m_nextExpectedReliableID ) )
	{
		uint16_t difference = reliableID - m_nextExpectedReliableID;
		ASSERT_OR_DIE( difference <= MAX_RELIABLE_RANGE, "reliableID out of range" );

		m_nextExpectedReliableID = reliableID + 1;
		RemoveReliablesLessThan( reliableID - MAX_RELIABLE_RANGE );
	}
	else
	{
		uint16_t difference = m_nextExpectedReliableID - reliableID;
		if ( difference < MAX_RELIABLE_RANGE )
		{
			m_receivedReliableIDs.push_back( reliableID );
		}
	}
}


//-----------------------------------------------------------------------------------------------
bool Connection::GreaterThanOrEqualToCyclic( uint16_t reliableID, uint16_t maxReliableID ) const
{
	const uint16_t HALF_UINT16 = 0x7fff;
	uint16_t difference = reliableID - maxReliableID;

	return ( ( difference > 0 ) && ( difference <= HALF_UINT16 ) );
}


//-----------------------------------------------------------------------------------------------
bool Connection::LessThanCyclic( uint16_t receivedReliableID, uint16_t reliableIDLowerBound ) const
{
	const uint16_t HALF_UINT16 = 0x7fff;
	uint16_t difference = reliableIDLowerBound - receivedReliableID;

	return ( ( difference > 0 ) && ( difference <= HALF_UINT16 ) );
}


//-----------------------------------------------------------------------------------------------
void Connection::RemoveReliablesLessThan( uint16_t reliableIDLowerBound )
{
	std::vector< uint16_t >::iterator receivedReliablesIterator;
	for ( receivedReliablesIterator = m_receivedReliableIDs.begin(); 
		receivedReliablesIterator != m_receivedReliableIDs.end(); )
	{
		if ( LessThanCyclic( *receivedReliablesIterator, reliableIDLowerBound ) )
		{
			receivedReliablesIterator = m_receivedReliableIDs.erase( receivedReliablesIterator );
			continue;
		}

		receivedReliablesIterator++;
	}
}

//-----------------------------------------------------------------------------------------------
void Connection::MarkPacketReceived( const Packet* packet )
{
	UpdateHighestAckAndPreviousReceivedAcksBitfield( packet->m_ack );
	ConfirmAck( packet->m_highestReceivedAck );
	for ( size_t bitIndex = 0; bitIndex < sizeof( packet->m_previousReceivedAcksBitfield ); ++bitIndex )
	{
		if ( IsBitSetAtIndex( packet->m_previousReceivedAcksBitfield, bitIndex ) )
		{
			ConfirmAck( packet->m_highestReceivedAck - ( uint16_t ) ( bitIndex ) + 1 );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Connection::UpdateHighestAckAndPreviousReceivedAcksBitfield( uint16_t ack )
{
	if ( GreaterThanOrEqualToCyclic( ack, m_highestReceivedAck ) )
	{
		uint16_t shiftOffset = ack - m_highestReceivedAck;
		m_previousReceivedAcksBitfield = m_previousReceivedAcksBitfield << shiftOffset;
		m_highestReceivedAck = ack;
		m_nextExpectedAck = m_highestReceivedAck + 1;
		SetBitAtIndex( m_previousReceivedAcksBitfield, shiftOffset - 1 );
	}
	else
	{
		// Ack is less than m_highestReceivedAck
		uint16_t shiftOffset = m_highestReceivedAck - ack;
		const size_t BITS_IN_A_BYTE = 8;
		if ( shiftOffset <= ( sizeof( uint16_t ) * BITS_IN_A_BYTE ) )
		{
			SetBitAtIndex( m_previousReceivedAcksBitfield, shiftOffset - 1 );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Connection::ConfirmAck( uint16_t ack )
{
	AckBundle* bundle = FindAckBundle( ack );
	if ( bundle != nullptr )
	{
		for ( uint16_t currentID : bundle->m_sentReliableIDs )
		{
			ConfirmReliableID( currentID );
		}
	}
}


//-----------------------------------------------------------------------------------------------
bool Connection::IsBitSetAtIndex( uint16_t bitfield, size_t index )
{
	return ( ( bitfield & ( 1 << index ) ) != 0 );
}



//-----------------------------------------------------------------------------------------------
void Connection::SetBitAtIndex( uint16_t& bitfield, size_t index )
{
	bitfield |= ( 1 << index );
}


//-----------------------------------------------------------------------------------------------
void Connection::ConfirmReliableID( uint16_t reliableID )
{
	if ( reliableID < m_oldestUnconfirmedReliableID )
	{
		return;
	}
	else if ( reliableID == m_oldestUnconfirmedReliableID )
	{
		m_confirmedReliableIDs.push_back( reliableID );
		while ( IsReliableIDConfirmed( m_oldestUnconfirmedReliableID ) )
		{
			RemoveConfirmedReliableID( m_oldestUnconfirmedReliableID );
			m_oldestUnconfirmedReliableID++;
		}
	}
	else
	{
		// reliableID > m_oldestUnconfirmedReliableID
		m_confirmedReliableIDs.push_back( reliableID );
	}
}


//-----------------------------------------------------------------------------------------------
void Connection::RemoveConfirmedReliableID( uint16_t reliableID )
{
	std::vector< uint16_t >::iterator confirmedReliablesIterator;
	for ( confirmedReliablesIterator = m_confirmedReliableIDs.begin();
		confirmedReliablesIterator != m_confirmedReliableIDs.end();
		confirmedReliablesIterator++ )
	{
		if ( *confirmedReliablesIterator == reliableID )
		{
			m_confirmedReliableIDs.erase( confirmedReliablesIterator );
			break;
		}
	}
}