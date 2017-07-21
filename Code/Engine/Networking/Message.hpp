#pragma once

#include "Engine/Networking/Packer.hpp"

#define MESSAGE_MTU 1024 // Bytes


//-----------------------------------------------------------------------------------------------
enum NETMSG : uint8_t
{
	NETMSG_PING = 0,
	NETMSG_PONG = 1,
	NETMSG_LAST,
	NETMSG_INVALID = 0xff
};


//-----------------------------------------------------------------------------------------------
enum GAMENETMSG : uint8_t
{
	GAMENETMSG_UPDATE = NETMSG_LAST,
	GAMENETMSG_SPAWNBULLET,
	GAMENETMSG_DESTROYBULLET,
	GAMENETMSG_INCREMENTREDSCORE,
	GAMENETMSG_INCREMENTGREENSCORE,
	GAMENETMSG_INCREMENTBLUESCORE
};


//-----------------------------------------------------------------------------------------------
struct MessageDefinition;
struct Sender;


//-----------------------------------------------------------------------------------------------
class Message : public Packer
{
public:
	Message();
	Message( uint8_t messageType );
	Message( Message* message );
	Message( Message const& ) = delete;
	void ResetOffset() const;
	size_t GetHeaderSize() const;
	size_t GetPayloadSize() const;
	size_t GetTypePayloadSize();

	// New for A4
	bool IsReliable() const;
	bool NeedsConnection() const;
	void ProcessMessage( const Sender& sender ) const;

	// New for A5
	bool IsOrdered() const;
	bool Message::operator < ( const Message& rhs ) const;

public:
	uint8_t m_messageID;
	uint8_t m_messageBuffer[ MESSAGE_MTU ];
	//short m_messageLength;

	// New for A4
	uint16_t m_reliableID;
	uint32_t m_lastSentTime;
	MessageDefinition* m_messageDefinition;

	// New for A5
	uint16_t m_sequenceID;
};