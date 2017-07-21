#pragma once

#include "Engine/Networking/Packer.hpp"

#define MAX_PACKET_SIZE 1232 // Bytes
#define INVALID_PACKET_ACK 0xffff


//-----------------------------------------------------------------------------------------------
class Message;


//-----------------------------------------------------------------------------------------------
class Packet : public Packer
{
public:
	Packet();
	void SetContentSizeFromBuffer();
	bool CanWriteMessageToPacket( Message* messageToWrite );
	void WriteMessageToPacket( const Message* messageToWrite );
	void ReadMessageFromPacket( Message* messageToRead );

public:
	uint8_t m_buffer[ MAX_PACKET_SIZE ];
	uint8_t m_numberOfMessages;

	// New for A3
	uint8_t m_fromConnectionIndex;

	// New for A4
	uint16_t m_ack;
	uint16_t m_highestReceivedAck; // this is the ack I received from them
	uint16_t m_previousReceivedAcksBitfield;
};