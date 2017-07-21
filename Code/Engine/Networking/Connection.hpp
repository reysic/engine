#pragma once

#include <set>

#include "Engine/Networking/NetworkingSystem.hpp"

#define MAX_GUID_LENGTH 32 // bytes
#define MAX_ACK_BUNDLES 128 // 2 seconds of memory is a good goal
#define MAX_MESSAGE_AGE 150 // max wait time before resending in milliseconds
#define MAX_RELIABLE_RANGE 1024 // maximum number of reliables that can be active


//-----------------------------------------------------------------------------------------------
class Session;
class Message;
class Packet;
struct Sender;


//-----------------------------------------------------------------------------------------------
class AckBundle
{
public:
	AckBundle();

	void AddReliableID( uint16_t reliableID );

public:
	uint16_t m_ackID;
	std::vector< uint16_t > m_sentReliableIDs;
};


//-----------------------------------------------------------------------------------------------
class Connection
{
public:
	Connection( uint8_t index, Session* session, sockaddr_in address, char guid[] );
	bool IsMyConnection( uint8_t index );
	void AddMessage( Message& msg );
	void FreeAllUnreliables();
	void SendPacket();
	Message* CreateMessageCopy( const Message& msg );

	// New for A4
	uint16_t GetNextAck();
	AckBundle* MakeAckBundle( uint16_t ackID );
	AckBundle* FindAckBundle( uint16_t ackID );
	uint8_t ResendSentReliables( Packet& packet, AckBundle* bundle );
	uint8_t SendUnsentReliables( Packet& packet, AckBundle* bundle );
	uint8_t SendUnreliables( Packet& packet );
	bool CanSendNewReliable();
	uint16_t GetNextReliableID();
	bool IsReliableIDConfirmed( uint16_t reliableID );
	bool MessageIsOld( Message* message );
	bool HasReceivedReliable( uint16_t reliableID ) const;
	void ProcessMessage( const Sender& sender, const Message& message );
	void ProcessOrderedMessage( const Sender& sender, const Message& message );
	void MarkMessageReceived( const Message& message );
	void MarkReliableReceived( uint16_t reliableID );
	bool GreaterThanOrEqualToCyclic( uint16_t reliableID, uint16_t maxReliableID ) const;
	bool LessThanCyclic( uint16_t receivedReliableID, uint16_t reliableIDLowerBound ) const;
	void RemoveReliablesLessThan( uint16_t reliableIDLowerBound );
	void MarkPacketReceived( const Packet* packet );
	void UpdateHighestAckAndPreviousReceivedAcksBitfield( uint16_t ack );
	void ConfirmAck( uint16_t ack );
	bool IsBitSetAtIndex( uint16_t bitfield, size_t index );
	void SetBitAtIndex( uint16_t& bitfield, size_t index );
	void ConfirmReliableID( uint16_t reliableID );
	void RemoveConfirmedReliableID( uint16_t reliableID );

public:
	// ID information
	uint8_t m_index;
	Session* m_session;
	sockaddr_in m_address;
	char m_guid[ MAX_GUID_LENGTH ]; // use a define, good practice
	// Bookkeeping information

	// New for A4
	// These are on the sending side, what the connection cares about
	uint16_t m_nextSentAck;
	AckBundle m_ackBundles[ MAX_ACK_BUNDLES ];

	// These are on the receiving side for the acks
	uint16_t m_highestReceivedAck;
	uint16_t m_nextExpectedAck;
	uint16_t m_previousReceivedAcksBitfield;

	// These are for sending side reliable IDs (which are for messages)
	// Reliable IDs are for messages, acks are for packets
	uint16_t m_nextSentReliableID;
	uint16_t m_oldestUnconfirmedReliableID;
	std::vector< uint16_t > m_confirmedReliableIDs;

	// These are for receiving reliable IDs
	uint16_t m_nextExpectedReliableID;
	std::vector< uint16_t > m_receivedReliableIDs;

	std::queue< Message* > m_unsentReliables;
	std::queue< Message* > m_sentReliables;
	std::queue< Message* > m_unreliables;

	// New for A5
	// Sending
	uint16_t m_nextSentSequenceID;

	// Receiving
	uint16_t m_nextExpectedSequenceID;
	std::vector< Message* > m_outOfOrderReceivedSequencedMessages;
};