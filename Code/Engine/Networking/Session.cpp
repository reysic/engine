#ifdef NETWORKING_SYSTEM


#include "Engine/Networking/Session.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/BuildConfig.hpp"


// Include from Game, how to avoid?
#include "Game/Core/TheGame.hpp"


//-----------------------------------------------------------------------------------------------
// Globals
static float g_timeSinceLastUpdate = 0.0f;


//-----------------------------------------------------------------------------------------------
void OnConnectionJoin( Connection* connection )
{
	// Check if you already have a connection with the same index

	if ( g_theGame->m_myConnection == nullptr )
	{
		g_theGame->m_myConnection = connection;
	}

	// Create a local object for myself
	g_theGame->m_netObjects[ connection->m_index ] = new NetPlayer( connection->m_index, 
		connection->m_index, Vector2( 800.0f, 450.0f ) );

// 	if ( connection->IsMyConnection( connection->m_index ) )
// 	{
// 		g_myObject = g_netObjects[ connection->m_index ];
// 	}
}


//-----------------------------------------------------------------------------------------------
void OnConnectionLeave( Connection* connection )
{
	delete g_theGame->m_netObjects[ connection->m_index ];
	g_theGame->m_netObjects[ connection->m_index ] = nullptr;
}


//-----------------------------------------------------------------------------------------------
Session::Session()
	: m_tickRate( 1.0f / 60.0f ) // Tick rate of 60 Hz, global tick rate is fine, optional is to do
								 // per connection. You process every frame, but only send on your
								 // tick rate
	, m_hasStarted( false )
	, m_packetChannel( nullptr )
	, m_timeDataLastSent( 0.0 )
	, m_timeDataLastReceived( 0.0 )
	, m_simLagMilliseconds( 0.0f )
	, m_simLossPercent( 0.0f )
	, m_sessionState( SESSION_STATE_UINITIALIZED )
{
	// Initialize connection objects to nullptr
	m_myConnection = nullptr;

	for ( int index = 0; index < MAX_CONNECTIONS; ++index )
	{
		m_connections[ index ] = nullptr;
	}

	g_theGame->m_mySession = this;
}


//-----------------------------------------------------------------------------------------------
Session::~Session()
{
	delete m_packetChannel;
	m_packetChannel = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Session::Update( float deltaSeconds )
{
	ProcessIncomingPackets();

	// New for A3
	g_timeSinceLastUpdate += deltaSeconds;
	if ( g_timeSinceLastUpdate >= m_tickRate )
	{
		for ( int index = 0; index < MAX_CONNECTIONS; ++index )
		{
			// Iterate over Connections and call HandleConnectionTick() and SendPacket()
			if ( m_connections[ index ] != nullptr && m_hasStarted )
			{
				HandleConnectionTick( m_connections[ index ], deltaSeconds );
				m_connections[ index ]->SendPacket();
			}
		}
		g_timeSinceLastUpdate = 0.0f;
	}
}


//-----------------------------------------------------------------------------------------------
void Session::SendMessageDirect( sockaddr_in addr, Message& msg )
{
	Packet packet;
	msg.m_messageDefinition = FindDefinition( msg.m_messageID );

	if ( msg.m_messageDefinition == nullptr )
	{
		return;
	}

	const uint16_t my_size = ( uint16_t ) msg.GetHeaderSize() + ( uint16_t ) msg.GetPayloadSize();
	if ( packet.GetWritableBytes() >= my_size )
	{
		if ( m_myConnection != nullptr )
		{
			packet.Write< uint8_t >( m_myConnection->m_index );
		}
		else
		{
			uint8_t invalidByte = 0xff;
			packet.Write< uint8_t >( invalidByte );
		}
		uint8_t message_count = 1;
		packet.Write< uint8_t >( message_count );
		const uint16_t msg_size = ( uint16_t ) msg.GetTypePayloadSize();
		packet.Write< uint16_t >( msg_size ); // message size
		packet.Write< uint8_t >( msg.m_messageID ); // message header
		void *buffer = ( uint8_t* ) msg.m_buffer + msg.GetHeaderSize();
		packet.WriteForwardAlongBuffer( buffer, msg.GetPayloadSize() ); // message payload
	}
	m_packetChannel->SendTo( addr, packet.m_buffer, packet.GetTotalReadableBytes() );
}


//-----------------------------------------------------------------------------------------------
void Session::ProcessIncomingPackets()
{
	Packet packet;
	Sender from;
	from.sharedSession = this;

	while ( ReadNextPacketFromSocket( &packet , &from.msgSrcAddr ) )
	{
		// Read from connection index
		uint8_t fromConnectionIndex;
		packet.Read< uint8_t >( &fromConnectionIndex );
		from.connection = GetConnection( fromConnectionIndex );
		packet.m_fromConnectionIndex = fromConnectionIndex;

		// Read ack
		uint16_t ack;
		packet.Read< uint16_t >( &ack );
		packet.m_ack = ack;

		// Read highest received ack
		uint16_t highestReceivedAck;
		packet.Read< uint16_t >( &highestReceivedAck );
		packet.m_highestReceivedAck = highestReceivedAck;

		// Read previous received ack's bitfield
		uint16_t previousReceivedAcksBitfield;
		packet.Read< uint16_t >( &previousReceivedAcksBitfield );
		packet.m_previousReceivedAcksBitfield = previousReceivedAcksBitfield;

		// Read number of messages
		uint8_t numMessages;
		packet.Read< uint8_t >( &numMessages );
		packet.m_numberOfMessages = numMessages;

		while ( numMessages > 0 )
		{
			Message msg;
			--numMessages;
			packet.ReadMessageFromPacket( &msg );

			if ( MessageCanBeProcessed( from, msg ) )
			{
				if ( from.connection != nullptr )
				{
					// We have a connection
					from.connection->ProcessMessage( from, msg );
					from.connection->MarkMessageReceived( msg );
				}
				else
				{
					// We do not have a connection
					msg.ProcessMessage( from );
				}
			}

			msg.ResetOffset(); // Reading a new message, so want to reset offset
		}

		if ( from.connection != nullptr )
		{
			// Coming from a connection, so have to mark packet as received
			from.connection->MarkPacketReceived( &packet );
		}
	}
}


//-----------------------------------------------------------------------------------------------
bool Session::ReadNextPacketFromSocket( Packet* recv_packet, sockaddr_in* from_addr )
{
	size_t read = m_packetChannel->ReceiveFrom( from_addr, recv_packet->m_buffer, 
		MAX_PACKET_SIZE );

	if ( read == 0 )
	{
		return false;
	}
	recv_packet->SetContentSizeFromBuffer();
	return true;
}


//-----------------------------------------------------------------------------------------------
void Session::RegisterMessage( uint8_t message_id, const char* debug_name, MessageCallback* cb,
	uint8_t controlFlag, uint8_t optionFlag )
{
	if ( m_hasStarted )
	{
		// Don't allow registration of messages after session has started
		return;
	}

	MessageDefinition defn;
	defn.messageIndex = message_id;
	defn.debugName = debug_name;
	defn.cb = cb;
	defn.controlFlag = controlFlag;
	defn.optionFlag = optionFlag;

	if ( FindDefinition( message_id ) == nullptr ) 
	{
		AddDefinition( message_id, defn );
	}
}


//-----------------------------------------------------------------------------------------------
void Session::AddDefinition( uint8_t message_index, MessageDefinition& message_definition )
{
	m_messageDefinitions[ message_index ] = message_definition;
}


//-----------------------------------------------------------------------------------------------
MessageDefinition* Session::FindDefinition( short message_type )
{
	MessageDefinition* definition = &m_messageDefinitions[ message_type ];
	if ( definition->messageIndex == NETMSG_INVALID )
	{
		return nullptr;
	}
	else
	{
		return definition;
	}
}


//-----------------------------------------------------------------------------------------------
Connection* Session::CreateConnection( uint8_t index, const char* guid, sockaddr_in addr )
{
	for ( int i = 0; i < MAX_CONNECTIONS; ++i )
	{
		if ( m_connections[ i ] != nullptr )
		{
			if ( m_connections[ i ]->m_index == index )
			{
				g_theDeveloperConsole->ConsolePrint( "Connection with specified index already exists.", Rgba::RED );
				return nullptr;
			}
		}
	}

	// New all connections here, one will be your own
	uint8_t numCharactersToCopy = ( uint8_t ) strlen( guid );
	char guidAsArray[ MAX_GUID_LENGTH ];
	strncpy( guidAsArray, guid, numCharactersToCopy + 1 );
	Connection* newConnection = new Connection( index, this, addr, guidAsArray );

	m_connections[ index ] = newConnection;

	// Also set m_myConnection to the newly created Connection
	if ( ( addr.sin_addr.S_un.S_addr == m_socketAddr.sin_addr.S_un.S_addr ) &&
		( addr.sin_port == m_socketAddr.sin_port ) )
	{
		m_myConnection = newConnection;
	}

	// Call OnConnectionJoin() event
	OnConnectionJoin( newConnection );
	m_sessionState = SESSION_STATE_CONNECTED;

	return newConnection;
}


//-----------------------------------------------------------------------------------------------
void Session::DestroyConnection( uint8_t index )
{
	for ( int i = 0; i < MAX_CONNECTIONS; ++i )
	{
		if ( m_connections[ i ] != nullptr )
		{
			if ( m_connections[ i ]->m_index == index )
			{
				// Connection with specified index found

				// Call OnConnectionLeave() event
				OnConnectionLeave( m_connections[ i ] );

				// delete m_connections[ i ]; // Why isn't this working?
				m_connections[ i ] = nullptr;
				delete m_myConnection;
				m_myConnection = nullptr;
				g_theDeveloperConsole->ConsolePrint( "Connection destroyed with index " + std::to_string( index ), Rgba::GREEN );

				return;
			}
		}
	}
	
	g_theDeveloperConsole->ConsolePrint( "Connection with specified index not found.", Rgba::RED );
}


//-----------------------------------------------------------------------------------------------
uint8_t Session::GetMyConnectionIndex()
{
	return m_myConnection->m_index;
}


//-----------------------------------------------------------------------------------------------
NetObject* Session::FindObject( uint8_t connectionIndex )
{
	return g_theGame->m_netObjects[ connectionIndex ];
}


//-----------------------------------------------------------------------------------------------
void Session::Start()
{
	m_packetChannel = new PacketChannel();
	m_packetChannel->Create( g_theNetworkingSystem->GetLocalHostName(), GAME_PORT, &m_socketAddr );

	// Increment port number if specified port is already in use
	uint16_t i = 1;
	while ( m_packetChannel->m_socketWrapper->m_socket == INVALID_SOCKET )
	{
		std::string portAsString( GAME_PORT );
		uint16_t port = ( uint16_t ) stoi( portAsString ) + i;
		std::string intPortAsChar = std::to_string( port );
		m_packetChannel->Create( g_theNetworkingSystem->GetLocalHostName(), intPortAsChar.c_str(), &m_socketAddr );
		++i;
	}

	if ( m_packetChannel->m_socketWrapper->m_socket != INVALID_SOCKET )
	{
		const char* sockAddr = g_theNetworkingSystem->SockAddrToString( ( const sockaddr* ) &m_socketAddr );
		std::string sockAddrStr = std::string( sockAddr );
		g_theDeveloperConsole->ConsolePrint( "Session running on " + sockAddrStr );
		m_hasStarted = true;
		m_sessionState = SESSION_STATE_UNCONNECTED;

		// Initialize connections array by setting all pointers to nullptr
		for ( int index = 0; index < MAX_CONNECTIONS; ++index )
		{
			m_connections[ index ] = nullptr;
		}

	}
	else
	{
		g_theDeveloperConsole->ConsolePrint( "Unable to create UDP socket.", Rgba::RED );
	}
}


//-----------------------------------------------------------------------------------------------
void Session::HandleConnectionTick( Connection* connection, float deltaSeconds )
{
	UNUSED( deltaSeconds );

	// If we have some object associated with a connection, we can update it
	// Assume from connection is valid, if they're connected they have a valid index
	if ( m_myConnection->m_index == connection->m_index )
	{
		// Do nothing
		return;
	}
	else
	{
		uint8_t connectionIndex = connection->m_session->GetMyConnectionIndex();
		for ( int index = 0; index < MAX_NET_OBJECTS; ++index )
		{
			if ( g_theGame->m_netObjects[ index ] != nullptr )
			{
				if ( g_theGame->m_netObjects[ index ]->m_ownerConnectionIndex == connectionIndex )
				{
					// Get my synced object and update it
					NetObject* myObject = g_theGame->m_netObjects[ index ];
					Message updateMsg( GAMENETMSG_UPDATE );
					updateMsg.Write< uint8_t >( myObject->m_ownerConnectionIndex );
					updateMsg.Write< uint16_t >( myObject->m_netID );
					updateMsg.Write< Vector2 >( myObject->m_position );
					connection->AddMessage( updateMsg );

				}
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
Connection* Session::GetConnection( uint8_t index )
{
	if ( index < 0 || index > MAX_CONNECTIONS )
	{
		return nullptr;
	}
	else
	{
		return m_connections[ index ];
	}
}


//-----------------------------------------------------------------------------------------------
void Session::SetLag( float additionalLagMilliseconds )
{
	m_packetChannel->m_additionalLagMilliseconds = additionalLagMilliseconds;
}


//-----------------------------------------------------------------------------------------------
void Session::SetLoss( float dropRatePercentage )
{
	m_packetChannel->m_dropRatePercentage = dropRatePercentage;
}


//-----------------------------------------------------------------------------------------------
bool Session::MessageCanBeProcessed( const Sender& sender, const Message& message )
{
	if ( sender.connection == nullptr )
	{
		// if the message doesn't need a connection then we're good
		return ( !message.NeedsConnection() );
	}
	else
	{
		if ( message.IsReliable() )
		{
			return ( !sender.connection->HasReceivedReliable( message.m_reliableID ) );
		}
		else
		{
			return true;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Session::SendMessageToOthers( Message& message )
{
	for ( int index = 0; index < MAX_CONNECTIONS; ++index )
	{
		if ( m_connections[ index ] != nullptr )
		{
			if ( m_connections[ index ] != m_myConnection )
			{
				m_connections[ index ]->AddMessage( message );
			}
		}
	}
}


#endif