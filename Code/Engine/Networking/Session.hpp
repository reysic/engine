#pragma once

#include "Engine/Networking/NetworkingSystem.hpp"
#include "Engine/Networking/Packet.hpp"
#include "Engine/Networking/Message.hpp"
#include "Engine/Networking/Connection.hpp"
#include "Engine/Networking/UDPSocket.hpp"
#include "Engine/Networking/PacketChannel.hpp"

#define MAX_CONNECTIONS 10


//-----------------------------------------------------------------------------------------------
class Session;
class NetObject;
class NetPlayer;


//-----------------------------------------------------------------------------------------------
enum SessionState
{
	SESSION_STATE_UINITIALIZED, // Session is created, but has not yet bound a socket
	SESSION_STATE_UNCONNECTED, // Can send/receive packets, but no connections yet
	SESSION_STATE_CONNECTED, // Session has valid connection object created for ourselves
	NUM_SESSION_STATES
};


//-----------------------------------------------------------------------------------------------
struct Sender
{
	Session* sharedSession;
	sockaddr_in msgSrcAddr;
	Connection* connection;
};


//-----------------------------------------------------------------------------------------------
enum ControlFlags
{
	CONTROL_FLAG_CONNECTED = 0,
	CONTROL_FLAG_CONNECTIONLESS = 1,
	CONTROL_FLAG_INVALID = 2
};


//-----------------------------------------------------------------------------------------------
enum OptionFlags
{
	OPTION_FLAG_INVALID = 0,
	OPTION_FLAG_RELIABLE = 1,
	OPTION_FLAG_UNRELIABLE = 2,
	OPTION_FLAG_ORDERED_RELIABLE = 3
};


//-----------------------------------------------------------------------------------------------
typedef void( MessageCallback )( const Sender&, const Message& );
struct MessageDefinition
{
	uint8_t messageIndex;
	const char* debugName;
	MessageCallback* cb;
	uint8_t controlFlag; // 0 - connected, 1 - connectionless, 2 - invalid
	uint8_t optionFlag; // 0 - invalid, 1 - reliable, 2 - unreliable

	MessageDefinition()
		: messageIndex( NETMSG_INVALID )
		, debugName( "Invalid" )
		, cb( nullptr )
		, controlFlag( CONTROL_FLAG_CONNECTED )
		, optionFlag( OPTION_FLAG_INVALID ) 
	{};
};


//-----------------------------------------------------------------------------------------------
typedef void( ConnectionCallback )( Connection* );
typedef void( ConnectionTickCallback )( Connection*, const float );
class Session
{
public:
	Session();
	~Session();
	void Update( float deltaSeconds );
	void SendMessageDirect( sockaddr_in addr, Message& msg );
	void ProcessIncomingPackets();
	bool ReadNextPacketFromSocket( Packet* recv_packet, sockaddr_in* from_addr );
	void RegisterMessage( uint8_t message_id, const char* debug_name, MessageCallback* cb, 
		uint8_t controlFlag, uint8_t optionFlag );
	void AddDefinition( uint8_t message_index, MessageDefinition& message_definition );
	MessageDefinition* FindDefinition( short messageID );

	// New for A3
	Connection* CreateConnection( uint8_t index, const char* guid, sockaddr_in addr );
	void DestroyConnection( uint8_t index );
	uint8_t GetMyConnectionIndex();
	NetObject* FindObject( uint8_t connectionIndex );
	void Start();
	void HandleConnectionTick( Connection* connection, float deltaSeconds );
	Connection* GetConnection( uint8_t index );
	void SetLag( float additionalLagMilliseconds );
	void SetLoss( float dropRatePercentage );

	// New for A4
	bool MessageCanBeProcessed( const Sender& sender, const Message& message );

	// New for A6
	void SendMessageToOthers( Message& message );

public:
	PacketChannel* m_packetChannel;
	MessageDefinition m_messageDefinitions[ 256 ];

	// New for A3
	Connection* m_connections[ MAX_CONNECTIONS ]; // a list of connection pointers
												  // only time a pointer exists in here is when
												  // is has a valid connection index
	Connection* m_myConnection; // this also goes into the above array once assigned a valid connection
								// index, has to be cleaned up in both places, pointer is newed
								// off whenever we create ourselves
	float m_tickRate;
	bool m_hasStarted;
	sockaddr_in m_socketAddr;

	// Debugging Variables
	double m_timeDataLastSent;
	double m_timeDataLastReceived;
	float m_simLagMilliseconds;
	float m_simLossPercent;
	SessionState m_sessionState;
};