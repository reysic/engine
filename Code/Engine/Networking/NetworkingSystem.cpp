#ifdef NETWORKING_SYSTEM


#include "Engine/Networking/NetworkingSystem.hpp"
#include "Engine/Networking/Session.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/BuildConfig.hpp"


// Include from Game, how to avoid?
#include "Game/Core/TheGame.hpp"


//-----------------------------------------------------------------------------------------------
NetworkingSystem* g_theNetworkingSystem = nullptr;


//-----------------------------------------------------------------------------------------------
// A1 Globals
static addrinfo* g_allocatedAddress = nullptr;
static addrinfo* g_clientAddress = nullptr;
static bool g_isHosting = false;
static bool g_clientJoined = false;
static bool g_connectedToHost = false;
static SOCKET g_listenSocket;
static SOCKET g_mySocket;
static size_t g_sizeOfDataSent = 0;


//-----------------------------------------------------------------------------------------------
// A2 Globals
static Session* g_session = nullptr;


//-----------------------------------------------------------------------------------------------
NetworkingSystem::NetworkingSystem()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int error;

	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD( 2, 2 );

	error = WSAStartup( wVersionRequested, &wsaData );
	if ( error != 0 ) {
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		DebuggerPrintf( "WSAStartup failed with error: %d\n", error );
	}

	/* Confirm that the WinSock DLL supports 2.2.		 */
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */

	if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 ) 
	{
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		DebuggerPrintf( "Could not find a usable version of Winsock.dll\n" );
		WSACleanup();
	}
	else
	{
		DebuggerPrintf( "The Winsock 2.2 dll was found okay\n" );
	}
}


//-----------------------------------------------------------------------------------------------
NetworkingSystem::~NetworkingSystem()
{
	FreeAddresses( g_allocatedAddress );
	FreeAddresses( g_clientAddress );

	if ( g_session != nullptr )
	{
		delete g_session;
		g_session = nullptr;
	}

	WSACleanup();
}


//-----------------------------------------------------------------------------------------------
void NetworkingSystem::Update( float deltaSeconds )
{
	ProcessDataReceived();

	if ( g_session != nullptr )
	{
		g_session->Update( deltaSeconds );
	}

	if ( !g_isHosting ) { return; } // Return if not hosting, don't want client trying to accept connections
	if ( g_clientJoined ) { return; } // Return if a client is already connected, only support a single client for now

	// Currently hosting and client has not joined, so poll for connections to accept
	GetNewClient();

}


//-----------------------------------------------------------------------------------------------
void NetworkingSystem::ProcessDataReceived()
{
	bool shouldDisconnectReceive;
	char buffer[ 256 ];
	size_t dataReceived = g_theNetworkingSystem->SocketReceive( &shouldDisconnectReceive, g_mySocket, &buffer, 256 );
	if ( dataReceived != 0 )
	{
		std::string received = "";
		for ( unsigned int i = 0; i < dataReceived; i++ )
		{
			if ( buffer[ i ] != '\0' )
			{
				received.push_back( ( char ) buffer[ i ] );
			}
			else
			{
				break;
			}
		}
		g_theDeveloperConsole->ExecuteCommand( received );
	}
}


//-----------------------------------------------------------------------------------------------
void NetworkingSystem::GetNewClient()
{
	sockaddr_in outClientAddress;
	g_mySocket = AcceptConnection( g_listenSocket, &outClientAddress );

	if ( g_mySocket != INVALID_SOCKET )
	{
		// Valid connection accepted!
		g_clientJoined = true;
		g_theDeveloperConsole->ConsolePrint( "Client Joined: " +
			std::string( g_theNetworkingSystem->SockAddrToString( ( const sockaddr* ) &outClientAddress ) ) );
	}
}


//-----------------------------------------------------------------------------------------------
// Get local host name, usually your machine's name, stored in a global buffer so not thread-safe
const char* NetworkingSystem::GetLocalHostName()
{
	static char buffer[ 256 ];
	if ( ::gethostname( buffer, 256 ) == 0 ) 
	{
		return buffer;
	}
	else 
	{
		DebuggerPrintf( "Failed to grab local host name\n" ); // Check WSAGetLastError() to find
															  // why failed 
		return "localhost";
	}

}


//-----------------------------------------------------------------------------------------------
// Get All Addresses that match our criteria
addrinfo* NetworkingSystem::AllocateAddressesForHost( const char* host, // host, like google.com
	const char* service, // service, usually the port number as a string
	int family,			 // Connection Family, AF_INET (IPv4) for this assignment
	int socktype,		 // Socket Type, SOCK_STREAM or SOCK_DGRAM (TCP or UDP) for this class
	int flags )			 // Search flag hints, we use this for AI_PASSIVE (bindable addresses)
{
	// Also, family of AF_UNSPEC will return all address that support the 
	// sock type (so both IPv4 and IPv6 adddress).

	// Define the hints - this is what it will use
	// for determining what addresses to return
	addrinfo hints;
	memset( &hints, 0, sizeof( hints ) );

	hints.ai_family = family;
	hints.ai_socktype = socktype;
	hints.ai_flags = flags;
	// This will allocate all addresses into a single linked list
	// with the head put into result.
	addrinfo *result = nullptr;
	int status = getaddrinfo( host, service, &hints, &result );
	if ( status != 0 ) 
	{
		/* Warning( "net", "Failed to find addresses for [%s:%s].  Error[%s]",
		host, service, gai_strerror(status) ); */
		return nullptr;
	}

	return result;
}


//-----------------------------------------------------------------------------------------------
// Since we allocate the addresses, we must free them.
// Takes the addrinfo written to by getaddrinfo( ... )
void NetworkingSystem::FreeAddresses( addrinfo* addresses )
{
	if ( nullptr != addresses )
	{
		freeaddrinfo( addresses );
	}
}


//-----------------------------------------------------------------------------------------------
// Binding a TCP Socket for Listening Purposes
SOCKET NetworkingSystem::CreateListenSocket( const char* addr, const char* service, sockaddr_in *out_addr )
{
	// First, try to get network addresses for this
	addrinfo *info_list = AllocateAddressesForHost( addr, // an address for this machine
		service, // service, which for TCP/IP is the port as a string (ex: "80")
		AF_INET, // We're doing IPv4 in class
		SOCK_STREAM, // TCP for now
		AI_PASSIVE );  // And something we can bind (and therefore listen on)


	if ( info_list == nullptr ) {
		// no addresses match - FAIL
		return false;
	}

	// Alright, try to create a SOCKET from this addr info
	SOCKET my_socket = INVALID_SOCKET;
	addrinfo *iter = info_list;
	while ( ( iter != nullptr ) && ( my_socket == INVALID_SOCKET ) ) {

		// First, create a socket for this address.
		// family, socktype, and protocol are provided by the addrinfo
		// if you wanted to be manual, for an TCP/IPv4 socket you'd use
		// AF_INET, SOCK_STREAM, IPPROTO_TCP
		my_socket = socket( iter->ai_family, iter->ai_socktype, iter->ai_protocol );
		if ( my_socket != INVALID_SOCKET ) {
			// Okay, we were able to create it,
			// Now try to bind it (associates the address (ex: 192.168.1.52:4325) to this 
			// socket so it will receive information for it.
			int result = bind( my_socket, iter->ai_addr, ( int ) ( iter->ai_addrlen ) );
			if ( SOCKET_ERROR != result ) {

				// Set it to non-block - since we'll be working with this on our main thread
				u_long non_blocking = 1;
				ioctlsocket( my_socket, FIONBIO, &non_blocking );

				// Set it to listen - this will allow people to connect to us
				result = listen( my_socket, 2 );
				ASSERT_OR_DIE( result != SOCKET_ERROR, "SOCKET_ERROR" ); // sanity check

												  // Save off the address if available.
				ASSERT_OR_DIE( iter->ai_addrlen == sizeof( sockaddr_in ), "addrlen != sockaddr_in" );
				if ( nullptr != out_addr ) {
					memcpy( out_addr, iter->ai_addr, iter->ai_addrlen );
				}
			}
			else {
				// Cleanup on Fail.
				closesocket( my_socket );
				my_socket = INVALID_SOCKET;
			}
		}
		iter = iter->ai_next;
	}

	// If we allocted, we must free eventually
	FreeAddresses( info_list );

	// Return the socket we created.
	return my_socket;
}


//-----------------------------------------------------------------------------------------------
// Accepting a Connection on a Listening Socket
// (Called on the host - this will Okay a socket
// trying to connect with ::connect in the code above.
SOCKET NetworkingSystem::AcceptConnection( SOCKET host_socket, sockaddr_in *out_their_addr )
{
	sockaddr_storage their_addr;
	int their_addr_len = sizeof( their_addr );

	SOCKET their_socket = ::accept( host_socket, ( sockaddr* ) &their_addr, &their_addr_len );
	if ( their_socket != INVALID_SOCKET ) {

		// If you want to support IPv6, this is no longer a valid check
		if ( out_their_addr != nullptr ) {
			ASSERT_OR_DIE( their_addr_len == sizeof( sockaddr_in ), "addrlen != sockaddr_in" );
			memcpy( out_their_addr, &their_addr, their_addr_len );
		}

		return their_socket;
	}
	else {
		// if we fail to accept, it might be we lost
		// connection - you can check the same we we'll do it
		// for send and recv below, and potentially return 
		// that error code somehow (if you move this code into a method
		// you could disonnect directly)
		/*
		int err = WSAGetLastError();
		if (SocketErrorShouldDisconnect(err)) {
		disconnect();
		}
		*/
	}

	return INVALID_SOCKET;
}


//-----------------------------------------------------------------------------------------------
// Similar to Hosting - with a few changes internally
// Comments are only around places that change.
SOCKET NetworkingSystem::SocketJoin( const char* addr, const char* service, sockaddr_in *out_addr )
{
	// First, we don't look for AI_PASSIVE connections - we don't bind this one
	// We'll be sending to it.

	addrinfo *info_list = AllocateAddressesForHost( addr, service, AF_INET, SOCK_STREAM );
	if ( info_list == nullptr ) {
		return false;
	}

	SOCKET my_socket = INVALID_SOCKET;
	addrinfo *iter = info_list;
	while ( ( iter != nullptr ) && ( my_socket == INVALID_SOCKET ) ) {
		my_socket = socket( iter->ai_family, iter->ai_socktype, iter->ai_protocol );
		if ( my_socket != INVALID_SOCKET ) {

			// Instead of binding - we call connect, which will do the TCP/IP
			// handshake.  Be aware this can block and cause a frame hickup, which
			// is fine for now.
			int result = ::connect( my_socket, iter->ai_addr, ( int ) ( iter->ai_addrlen ) );
			if ( SOCKET_ERROR != result ) {
				u_long non_blocking = 1;
				ioctlsocket( my_socket, FIONBIO, &non_blocking );

				// We do not listen on on this socket - we are not
				// accepting new connections.

				ASSERT_OR_DIE( iter->ai_addrlen == sizeof( sockaddr_in ), "addrlen != sockaddr_in" );
				if ( nullptr != out_addr ) {
					memcpy( out_addr, iter->ai_addr, iter->ai_addrlen );
				}
			}
			else {
				closesocket( my_socket );
				my_socket = INVALID_SOCKET;
			}
		}
		iter = iter->ai_next;
	}

	FreeAddresses( info_list );

	return my_socket;
}


//-----------------------------------------------------------------------------------------------
size_t NetworkingSystem::SocketSend( bool *out_should_disconnect, SOCKET my_socket, void const *data, size_t const data_size )
{
	*out_should_disconnect = false;
	if ( my_socket != INVALID_SOCKET ) {
		// send will return the amount of data actually sent.
		// It SHOULD match, or be an error.  
		int size = ::send( my_socket, ( char const* ) data, ( int ) data_size, 0 );
		if ( size < 0 ) {
			int32_t error = WSAGetLastError();
			if ( SocketErrorShouldDisconnect( error ) ) {
				// If the error is critical - disconnect this socket
				*out_should_disconnect = true;
			}
		}
		else {
			ASSERT_OR_DIE( ( size_t ) size == data_size, "size != data_size" );
		}

		return ( size_t ) size;
	}

	return ( size_t ) 0;
}


//-----------------------------------------------------------------------------------------------
size_t NetworkingSystem::SocketReceive( bool *out_should_disconnect, SOCKET my_socket, void *buffer, size_t const buffer_size )
{
	*out_should_disconnect = false;
	if ( my_socket != INVALID_SOCKET ) {
		// recv will return amount of data read, should always be <= buffer_size
		// Also, if you send, say, 3 KB with send, recv may actually
		// end up returning multiple times (say, 1KB, 512B, and 1.5KB) because 
		// the message got broken up - so be sure you application watches for it
		int size = ::recv( my_socket, ( char* ) buffer, buffer_size, 0 );
		if ( size < 0 ) {
			int32_t error = WSAGetLastError();
			if ( SocketErrorShouldDisconnect( error ) ) {
				*out_should_disconnect = true;
			}
			return 0U;
		}
		else {
			return ( size_t ) size;
		}
	}
	else {
		return 0U;
	}
}


//-----------------------------------------------------------------------------------------------
// Converting a sockaddr_in to a String
// Again, doing this in a global buffer so it is not thread safe
char const* NetworkingSystem::SockAddrToString( sockaddr const *addr )
{
	static char buffer[ 256 ];

	// Hard coding this for sockaddr_in for brevity
	// You can make this work for IPv6 as well
	sockaddr_in *addr_in = ( sockaddr_in* ) addr;

	// inet_ntop converts an address type to a human readable string,
	// ie 0x7f000001 => "127.0.0.1"
	// GetInAddr (defined below) gets the pointer to the address part of the sockaddr
	char hostname[ 256 ];
	inet_ntop( addr_in->sin_family, GetInAddr( addr ), hostname, 256 );

	// Combine the above with the port.  
	// Port is stored in network order, so convert it to host order
	// using ntohs (Network TO Host Short)
	sprintf_s( buffer, 256, "%s:%u", hostname, ntohs( addr_in->sin_port ) );

	// buffer is static - so will not go out of scope, but that means this is not thread safe.
	return buffer;
}


//-----------------------------------------------------------------------------------------------
// Converting a sockaddr_in to a String
// Again, doing this in a global buffer so it is not thread safe
char const* NetworkingSystem::SockAddrToStringWithoutPort( sockaddr const *addr )
{
	static char buffer[ 256 ];

	// Hard coding this for sockaddr_in for brevity
	// You can make this work for IPv6 as well
	sockaddr_in *addr_in = ( sockaddr_in* ) addr;

	// inet_ntop converts an address type to a human readable string,
	// ie 0x7f000001 => "127.0.0.1"
	// GetInAddr (defined below) gets the pointer to the address part of the sockaddr
	char hostname[ 256 ];
	inet_ntop( addr_in->sin_family, GetInAddr( addr ), hostname, 256 );

	// buffer is static - so will not go out of scope, but that means this is not thread safe.
	return buffer;
}


//-----------------------------------------------------------------------------------------------
bool NetworkingSystem::SockAddrFromString( const char* ip, const uint16_t port, 
	sockaddr_in* outAddress )
{
	sockaddr_in addr;
	memset( &addr, 0, sizeof( sockaddr_in ) );
	addr.sin_addr.S_un.S_addr = inet_addr( ip );
	if ( addr.sin_addr.S_un.S_addr != INADDR_NONE )
	{
		addr.sin_port = htons( port );
		addr.sin_family = AF_INET;
		*outAddress = addr;
		return true;
	}
	else
	{
		addr.sin_port = htons( port );
		addr.sin_family = AF_INET;
		*outAddress = addr;
		return false;
	}
}


//-----------------------------------------------------------------------------------------------
// get address part of a sockaddr, IPv4 or IPv6:
void* NetworkingSystem::GetInAddr( sockaddr const *sa )
{
	if ( sa->sa_family == AF_INET ) {
		return &( ( ( sockaddr_in* ) sa )->sin_addr );
	}
	else {
		return &( ( ( sockaddr_in6* ) sa )->sin6_addr );
	}

}


//-----------------------------------------------------------------------------------------------
// Ignoring Non-Critical Errors
// These errors are non-fatal and are more or less ignorable.
bool NetworkingSystem::SocketErrorShouldDisconnect( int32_t const error )
{
	switch ( error ) {
	case WSAEWOULDBLOCK: // nothing to do - would've blocked if set to blocking
	case WSAEMSGSIZE:    // UDP message too large - ignore that packet.
	case WSAECONNRESET:  // Other side reset their connection.
		return false;

	default:
		return true;
	}
}


//-----------------------------------------------------------------------------------------------
Session* NetworkingSystem::GetLocalSession()
{
	ASSERT_OR_DIE( g_session != nullptr, "g_session was nullptr!" );
	return g_session;
}


//-----------------------------------------------------------------------------------------------
// Console Commands
CONSOLE_COMMAND( address )
{
	UNUSED( args );
 	g_allocatedAddress = g_theNetworkingSystem->AllocateAddressesForHost(
 		g_theNetworkingSystem->GetLocalHostName(), COMMAND_SERVICE, AF_INET, SOCK_STREAM );
	g_theDeveloperConsole->ConsolePrint( g_theNetworkingSystem->SockAddrToString( g_allocatedAddress->ai_addr ) );
}


//-----------------------------------------------------------------------------------------------
CONSOLE_COMMAND( host )
{
	UNUSED( args );
	if ( g_allocatedAddress == nullptr )
	{
		g_allocatedAddress = g_theNetworkingSystem->AllocateAddressesForHost(
			g_theNetworkingSystem->GetLocalHostName(), COMMAND_SERVICE, AF_INET, SOCK_STREAM );
	}

	if ( g_isHosting )
	{
		g_theDeveloperConsole->ConsolePrint( "Already hosting." );
		return;
	}
	else if ( g_connectedToHost )
	{
		g_theDeveloperConsole->ConsolePrint( "Cannot host when connected as client.", Rgba::RED );
		return;
	}

	const char* allocatedAddrAsStr = g_theNetworkingSystem->SockAddrToStringWithoutPort( g_allocatedAddress->ai_addr );
	sockaddr_in out_addr; // address we actually connected to
	g_listenSocket = g_theNetworkingSystem->CreateListenSocket( allocatedAddrAsStr, COMMAND_SERVICE, &out_addr );

	const char* sockAddr = g_theNetworkingSystem->SockAddrToString( ( const sockaddr* ) &out_addr );
	std::string sockAddrStr = std::string( sockAddr );
	g_theDeveloperConsole->ConsolePrint( "Listening on " + sockAddrStr );

	g_isHosting = true;
}


//-----------------------------------------------------------------------------------------------
CONSOLE_COMMAND( stop )
{
	UNUSED( args );
	if ( !g_isHosting )
	{
		g_theDeveloperConsole->ConsolePrint( "Not currently hosting." );
		return;
	}

	g_theNetworkingSystem->CloseSocket( g_listenSocket );
	g_theDeveloperConsole->ConsolePrint( "Stopped command server." );
}


//-----------------------------------------------------------------------------------------------
CONSOLE_COMMAND( join )
{
	if ( args.m_argList.size() == 0 )
	{
		g_theDeveloperConsole->ConsolePrint( "Must provide host address following command.", Rgba::RED );
		return;
	}
	else if ( g_isHosting )
	{
		g_theDeveloperConsole->ConsolePrint( "Cannot join as client when hosting.", Rgba::RED );
		return;
	}

	sockaddr_in out_addr;
	g_mySocket = g_theNetworkingSystem->SocketJoin( ( args.m_argList[ 0 ] ).c_str(), COMMAND_SERVICE, &out_addr );

 	if ( g_mySocket != INVALID_SOCKET )
	{
		const char* sockAddr = g_theNetworkingSystem->SockAddrToString( ( const sockaddr* ) &out_addr );
		std::string sockAddrStr = std::string( sockAddr );
		g_theDeveloperConsole->ConsolePrint( "Joined command server " + sockAddrStr );
		g_connectedToHost = true;
	}
	else
	{
		g_theDeveloperConsole->ConsolePrint( "Failed to join server.", Rgba::RED );
	}
}


//-----------------------------------------------------------------------------------------------
CONSOLE_COMMAND( leave )
{
	UNUSED( args );
	if ( g_connectedToHost )
	{
		g_connectedToHost = false;
		g_theNetworkingSystem->CloseSocket( g_mySocket );
		g_theDeveloperConsole->ConsolePrint( "Disconnected from host." );
	}
	else
	{
		g_theDeveloperConsole->ConsolePrint( "Not connected to host.", Rgba::RED );
	}
}


//-----------------------------------------------------------------------------------------------
CONSOLE_COMMAND( send )
{
	if ( args.m_argList.size() == 0 )
	{
		g_theDeveloperConsole->ConsolePrint( "Must provide a command to send.", Rgba::RED );
		return;
	}

	bool shouldDisconnectSend;
	char buffer[ 256 ];
	strcpy_s( buffer, ( args.m_argList[ 0 ] ).c_str() );
	g_sizeOfDataSent = g_theNetworkingSystem->SocketSend( &shouldDisconnectSend, g_mySocket, buffer,
		256 );
	if ( g_sizeOfDataSent != sizeof( args.m_argList[ 0 ].c_str() + 1 ) )
	{
		g_theDeveloperConsole->ConsolePrint( "Mismatch in size of data sent. ", Rgba::RED );
	}
}


//-----------------------------------------------------------------------------------------------
void OnPingReceived( const Sender& sender, const Message& msg )
{
	msg;
	msg.ResetOffset();
// msg.m_offset = msg.GetHeaderSize();
// const char *str = msg.ReadString();
// msg.ResetOffset();
// 
// 	std::string endStr = ( str != nullptr ? str : "null" );
// 
// 	std::string addrStr = g_theNetworkingSystem->SockAddrToString( ( sockaddr* ) ( &sender.msgSrcAddr ) );
// 
// 	g_theDeveloperConsole->ConsolePrint( "Received ping from: " + addrStr + ". [" + endStr + "]" );
// 
 	Message pong( NETMSG_PONG );

 	g_session->SendMessageDirect( sender.msgSrcAddr, pong );
}


//-----------------------------------------------------------------------------------------------
void OnPongReceived( const Sender& sender, const Message& msg )
{
	msg;
	std::string addrStr = g_theNetworkingSystem->SockAddrToString( ( sockaddr* ) ( &sender.msgSrcAddr ) );
	g_theDeveloperConsole->ConsolePrint( "Received pong from: " + addrStr );
}


//-----------------------------------------------------------------------------------------------
void OnUpdateReceived( const Sender& sender, const Message& msg )
{
	// Don't run if connection is nullptr
	if ( sender.connection == nullptr )
	{
		return;
	}

	msg.ResetOffset();
	uint8_t ownerConnectionIndex;
	msg.Read< uint8_t >( &ownerConnectionIndex );
	uint16_t netID;
	msg.Read< uint16_t >( &netID );
	Vector2 position = Vector2::ZERO;
	msg.Read< Vector2 >( &position );

	NetObject* theirObject = g_theGame->FindNetObjectByID( ownerConnectionIndex, netID );
	if ( theirObject != nullptr )
	{
		theirObject->m_position = position;
	}
}


//-----------------------------------------------------------------------------------------------
void OnSpawnBulletReceived( const Sender& sender, const Message& msg )
{
	// Don't run if connection is nullptr
	if ( sender.connection == nullptr )
	{
		return;
	}

	// Read in bullet properties
	uint16_t netID;
	msg.Read< uint16_t >( &netID );
	Vector2 position = Vector2::ZERO;
	msg.Read< Vector2 >( &position );
	float xboxRightStickPositionPolarThetaRadians;
	msg.Read< float >( &xboxRightStickPositionPolarThetaRadians );

	// Make bullet
	g_theGame->SpawnPlayerBulletIDoNotOwn( sender.connection->m_index, netID, position, 
		xboxRightStickPositionPolarThetaRadians );
}


//-----------------------------------------------------------------------------------------------
void OnDestroyBulletReceived( const Sender& sender, const Message& msg )
{
	// Don't run if connection is nullptr
	if ( sender.connection == nullptr )
	{
		return;
	}

	// Read in bullet properties
	uint16_t netID;
	msg.Read< uint16_t >( &netID );

	// Destroy bullet
	g_theGame->DeleteNetObjectByID( sender.connection->m_index, netID );
}


//-----------------------------------------------------------------------------------------------
void OnIncrementRedScoreReceived( const Sender& sender, const Message& msg )
{
	// Don't run if connection is nullptr
	if ( sender.connection == nullptr )
	{
		return;
	}

	g_theGame->IncrementRedScore();
}


//-----------------------------------------------------------------------------------------------
void OnIncrementGreenScoreReceived( const Sender& sender, const Message& msg )
{
	// Don't run if connection is nullptr
	if ( sender.connection == nullptr )
	{
		return;
	}

	g_theGame->IncrementGreenScore();
}

//-----------------------------------------------------------------------------------------------
void OnIncrementBlueScoreReceived( const Sender& sender, const Message& msg )
{
	// Don't run if connection is nullptr
	if ( sender.connection == nullptr )
	{
		return;
	}

	g_theGame->IncrementBlueScore();
}


//-----------------------------------------------------------------------------------------------
CONSOLE_COMMAND( net_session_start )
{
	UNUSED( args );
	if ( g_session != nullptr )
	{
		g_theDeveloperConsole->ConsolePrint( "Session already exists.", Rgba::RED );
		return;
	}

	g_session = new Session();

	// Registration of core session messages
	g_session->RegisterMessage( NETMSG_PING, "ping", OnPingReceived, 0, 2 );
	g_session->RegisterMessage( NETMSG_PONG, "pong", OnPongReceived, 0, 2 );

	// Registration of game-specific session messages
	g_session->RegisterMessage( GAMENETMSG_UPDATE, "gameupdate", OnUpdateReceived, 1, 1 );
	g_session->RegisterMessage( GAMENETMSG_SPAWNBULLET, "spawnbullet", OnSpawnBulletReceived, 1, 1 );
	g_session->RegisterMessage( GAMENETMSG_DESTROYBULLET, "destroybullet", OnDestroyBulletReceived, 1, 1 );
	g_session->RegisterMessage( GAMENETMSG_INCREMENTREDSCORE, "incrementredscore", OnIncrementRedScoreReceived, 1, 1 );
	g_session->RegisterMessage( GAMENETMSG_INCREMENTGREENSCORE, "incrementgreenscore", OnIncrementGreenScoreReceived, 1, 1 );
	g_session->RegisterMessage( GAMENETMSG_INCREMENTBLUESCORE, "incrementbluescore", OnIncrementBlueScoreReceived, 1, 1 );

	g_session->Start();
}


//-----------------------------------------------------------------------------------------------
CONSOLE_COMMAND( net_session_stop )
{
	UNUSED( args );
	if ( g_session != nullptr )
	{
		delete g_session;
		g_session = nullptr;
		g_theDeveloperConsole->ConsolePrint( "Session closed." );
	}
	else
	{
		g_theDeveloperConsole->ConsolePrint( "No session running.", Rgba::RED );
	}
}


//-----------------------------------------------------------------------------------------------
CONSOLE_COMMAND( net_ping )
{
	if ( g_session != nullptr )
	{
		// Ensure we have a session before trying to ping

		if ( args.m_argList.size() == 0 )
		{
			g_theDeveloperConsole->ConsolePrint( "Must provide an IP address to ping.", Rgba::RED );
 		}
		else if ( args.m_argList.size() == 1 )
		{
			std::string colon = ":";
			std::size_t found = args.m_argList[ 0 ].find( colon );
			if ( found != std::string::npos )
			{
				std::string ip = args.m_argList[ 0 ].substr( 0, found );
				std::string port = args.m_argList[ 0 ].substr( found + 1, std::string::npos );
				
				sockaddr_in to;
				if ( g_theNetworkingSystem->SockAddrFromString( ip.c_str(), ( uint16_t ) atoi( port.c_str() ), &to ) )
				{
					Message msg( NETMSG_PING );
					g_session->SendMessageDirect( to, msg );
				}
				else
				{
					g_theDeveloperConsole->ConsolePrint( "Invalid IP address provided.", Rgba::RED );
				}

			}
			else
			{
				g_theDeveloperConsole->ConsolePrint( "Invalid IP address provided.", Rgba::RED );
			}
		}
		else if ( args.m_argList.size() == 2 )
		{
			std::string colon = ":";
			std::size_t found = args.m_argList[ 0 ].find( colon );
			if ( found != std::string::npos )
			{
				std::string ip = args.m_argList[ 0 ].substr( 0, found );
				std::string port = args.m_argList[ 0 ].substr( found + 1, std::string::npos );

				sockaddr_in to;
				if ( g_theNetworkingSystem->SockAddrFromString( ip.c_str(), ( uint16_t ) atoi( port.c_str() ), &to ) )
				{
					Message msg( NETMSG_PING );
					g_session->SendMessageDirect( to, msg );
				}
				else
				{
					g_theDeveloperConsole->ConsolePrint( "Invalid IP address provided.", Rgba::RED );
				}

			}
			else
			{
				g_theDeveloperConsole->ConsolePrint( "Invalid IP address provided.", Rgba::RED );
			}

		}
		else
		{
			g_theDeveloperConsole->ConsolePrint( "Too many arguments provided.", Rgba::RED );
		}
	}
	else
	{
		g_theDeveloperConsole->ConsolePrint( "Cannot send with session.", Rgba::RED );
	}
}


//-----------------------------------------------------------------------------------------------
// Usage: net_session_create_connection <index> <guid> <address>
// Adds a connection to the session with the given information. If the index, guid, or address is
// taken, the command should fail.
CONSOLE_COMMAND( net_session_create_connection )
{
	if ( g_session == nullptr )
	{
		g_theDeveloperConsole->ConsolePrint( "Cannot create connection without session.", Rgba::RED );
		return;
	}

	if ( args.m_argList.size() == 0 )
	{
		// No arguments provided, so execute with some default values
		
		// Get first connection address as string
		std::string allocatedAddressAsString = g_theNetworkingSystem->SockAddrToString( g_allocatedAddress->ai_addr );
		std::string colon = ":";
		std::size_t found = allocatedAddressAsString.find( colon );
		allocatedAddressAsString.replace( found + 1, 4, GAME_PORT );
		std::string firstConnectionAddress = allocatedAddressAsString;

		// Get second connection address as string
		allocatedAddressAsString = g_theNetworkingSystem->SockAddrToString( g_allocatedAddress->ai_addr );
		found = allocatedAddressAsString.find( colon );
		std::string portAsString( GAME_PORT );
		uint16_t port = ( uint16_t ) stoi( portAsString ) + ( uint16_t ) 1; // Increment port by 1 for second connection
		std::string intPortAsChar = std::to_string( port );
		allocatedAddressAsString.replace( found + 1, 4, intPortAsChar );
		std::string secondConnectionAddress = allocatedAddressAsString;

		// Get first connection IP and port
		found = firstConnectionAddress.find( colon );
		std::string firstConnectionIP = firstConnectionAddress.substr( 0, found );
		std::string firstConnectionPort = firstConnectionAddress.substr( found + 1, std::string::npos );

		// Get second cconnection IP and port
		found = secondConnectionAddress.find( colon );
		std::string secondConnectionIP = secondConnectionAddress.substr( 0, found );
		std::string secondConnectionPort = secondConnectionAddress.substr( found + 1, std::string::npos );


		sockaddr_in firstConnectionAddr;
		g_theNetworkingSystem->SockAddrFromString( firstConnectionIP.c_str(), 
			( uint16_t ) atoi( firstConnectionPort.c_str() ), &firstConnectionAddr );
		sockaddr_in secondConnectionAddr;
		g_theNetworkingSystem->SockAddrFromString( secondConnectionIP.c_str(),
			( uint16_t ) atoi( secondConnectionPort.c_str() ), &secondConnectionAddr );

		g_session->CreateConnection( 0, "PLAYER0", firstConnectionAddr );
 		g_session->CreateConnection( 1, "PLAYER1", secondConnectionAddr );
		return;
	}

	// Fix this to handle bad input
	std::string colon = ":";
	std::size_t found = args.m_argList[ 2 ].find( colon );
	if ( found != std::string::npos )
	{
		std::string ip = args.m_argList[ 2 ].substr( 0, found );
		std::string port = args.m_argList[ 2 ].substr( found + 1, std::string::npos );

		sockaddr_in addr;
		g_theNetworkingSystem->SockAddrFromString( ip.c_str(), ( uint16_t ) atoi( port.c_str() ), &addr );

		g_session->CreateConnection( ( uint8_t ) atoi( args.m_argList[ 0 ].c_str() ), args.m_argList[ 1 ].c_str(), addr );
	}
	else
	{
		g_theDeveloperConsole->ConsolePrint( "Invalid IP address.", Rgba::RED );
	}
}


//-----------------------------------------------------------------------------------------------
// Just for debugging, quick setup of connection PLAYER0, then PLAYER1, then PLAYER2
CONSOLE_COMMAND( 0 )
{
	UNUSED( args );

	// Get first connection address as string
	std::string allocatedAddressAsString = g_theNetworkingSystem->SockAddrToString( g_allocatedAddress->ai_addr );
	std::string colon = ":";
	std::size_t found = allocatedAddressAsString.find( colon );
	allocatedAddressAsString.replace( found + 1, 4, GAME_PORT );
	std::string firstConnectionAddress = allocatedAddressAsString;

	// Get second connection address as string
	allocatedAddressAsString = g_theNetworkingSystem->SockAddrToString( g_allocatedAddress->ai_addr );
	found = allocatedAddressAsString.find( colon );
	std::string portAsString( GAME_PORT );
	uint16_t port = ( uint16_t ) stoi( portAsString ) + ( uint16_t ) 1; // Increment port by 1 for second connection
	std::string intPortAsChar = std::to_string( port );
	allocatedAddressAsString.replace( found + 1, 4, intPortAsChar );
	std::string secondConnectionAddress = allocatedAddressAsString;

	// Get third connection address as string
	allocatedAddressAsString = g_theNetworkingSystem->SockAddrToString( g_allocatedAddress->ai_addr );
	found = allocatedAddressAsString.find( colon );
	portAsString = std::string( GAME_PORT );
	port = ( uint16_t ) stoi( portAsString ) + ( uint16_t ) 2; // Increment port by 2 for third connection
	intPortAsChar = std::to_string( port );
	allocatedAddressAsString.replace( found + 1, 4, intPortAsChar );
	std::string thirdConnectionAddress = allocatedAddressAsString;

	// Get first connection IP and port
	found = firstConnectionAddress.find( colon );
	std::string firstConnectionIP = firstConnectionAddress.substr( 0, found );
	std::string firstConnectionPort = firstConnectionAddress.substr( found + 1, std::string::npos );

	// Get second connection IP and port
	found = secondConnectionAddress.find( colon );
	std::string secondConnectionIP = secondConnectionAddress.substr( 0, found );
	std::string secondConnectionPort = secondConnectionAddress.substr( found + 1, std::string::npos );

	// Get third connection IP and port
	found = thirdConnectionAddress.find( colon );
	std::string thirdConnectionIP = thirdConnectionAddress.substr( 0, found );
	std::string thirdConnectionPort = thirdConnectionAddress.substr( found + 1, std::string::npos );

	sockaddr_in firstConnectionAddr;
	g_theNetworkingSystem->SockAddrFromString( firstConnectionIP.c_str(),
		( uint16_t ) atoi( firstConnectionPort.c_str() ), &firstConnectionAddr );
	sockaddr_in secondConnectionAddr;
	g_theNetworkingSystem->SockAddrFromString( secondConnectionIP.c_str(),
		( uint16_t ) atoi( secondConnectionPort.c_str() ), &secondConnectionAddr );
	sockaddr_in thirdConnectionAddr;
	g_theNetworkingSystem->SockAddrFromString( thirdConnectionIP.c_str(),
		( uint16_t ) atoi( thirdConnectionPort.c_str() ), &thirdConnectionAddr );

	g_session->CreateConnection( 0, "PLAYER0", firstConnectionAddr );
	g_session->CreateConnection( 1, "PLAYER1", secondConnectionAddr );
	g_session->CreateConnection( 2, "PLAYER2", thirdConnectionAddr );
}


//-----------------------------------------------------------------------------------------------
// Just for debugging, quick setup of connection PLAYER1, then PLAYER0, then PLAYER2
CONSOLE_COMMAND( 1 )
{
	UNUSED( args );

	// Get first connection address as string
	std::string allocatedAddressAsString = g_theNetworkingSystem->SockAddrToString( g_allocatedAddress->ai_addr );
	std::string colon = ":";
	std::size_t found = allocatedAddressAsString.find( colon );
	allocatedAddressAsString.replace( found + 1, 4, GAME_PORT );
	std::string firstConnectionAddress = allocatedAddressAsString;

	// Get second connection address as string
	allocatedAddressAsString = g_theNetworkingSystem->SockAddrToString( g_allocatedAddress->ai_addr );
	found = allocatedAddressAsString.find( colon );
	std::string portAsString( GAME_PORT );
	uint16_t port = ( uint16_t ) stoi( portAsString ) + ( uint16_t ) 1; // Increment port by 1 for second connection
	std::string intPortAsChar = std::to_string( port );
	allocatedAddressAsString.replace( found + 1, 4, intPortAsChar );
	std::string secondConnectionAddress = allocatedAddressAsString;

	// Get third connection address as string
	allocatedAddressAsString = g_theNetworkingSystem->SockAddrToString( g_allocatedAddress->ai_addr );
	found = allocatedAddressAsString.find( colon );
	portAsString = std::string( GAME_PORT );
	port = ( uint16_t ) stoi( portAsString ) + ( uint16_t ) 2; // Increment port by 2 for third connection
	intPortAsChar = std::to_string( port );
	allocatedAddressAsString.replace( found + 1, 4, intPortAsChar );
	std::string thirdConnectionAddress = allocatedAddressAsString;

	// Get first connection IP and port
	found = firstConnectionAddress.find( colon );
	std::string firstConnectionIP = firstConnectionAddress.substr( 0, found );
	std::string firstConnectionPort = firstConnectionAddress.substr( found + 1, std::string::npos );

	// Get second cconnection IP and port
	found = secondConnectionAddress.find( colon );
	std::string secondConnectionIP = secondConnectionAddress.substr( 0, found );
	std::string secondConnectionPort = secondConnectionAddress.substr( found + 1, std::string::npos );

	// Get third connection IP and port
	found = thirdConnectionAddress.find( colon );
	std::string thirdConnectionIP = thirdConnectionAddress.substr( 0, found );
	std::string thirdConnectionPort = thirdConnectionAddress.substr( found + 1, std::string::npos );

	sockaddr_in firstConnectionAddr;
	g_theNetworkingSystem->SockAddrFromString( firstConnectionIP.c_str(),
		( uint16_t ) atoi( firstConnectionPort.c_str() ), &firstConnectionAddr );
	sockaddr_in secondConnectionAddr;
	g_theNetworkingSystem->SockAddrFromString( secondConnectionIP.c_str(),
		( uint16_t ) atoi( secondConnectionPort.c_str() ), &secondConnectionAddr );
	sockaddr_in thirdConnectionAddr;
	g_theNetworkingSystem->SockAddrFromString( thirdConnectionIP.c_str(),
		( uint16_t ) atoi( thirdConnectionPort.c_str() ), &thirdConnectionAddr );

	g_session->CreateConnection( 1, "PLAYER1", secondConnectionAddr );
	g_session->CreateConnection( 0, "PLAYER0", firstConnectionAddr );
	g_session->CreateConnection( 2, "PLAYER2", thirdConnectionAddr );
}


//-----------------------------------------------------------------------------------------------
// Just for debugging, quick setup of connection PLAYER2, then PLAYER0, then PLAYER1
CONSOLE_COMMAND( 2 )
{
	UNUSED( args );

	// Get first connection address as string
	std::string allocatedAddressAsString = g_theNetworkingSystem->SockAddrToString( g_allocatedAddress->ai_addr );
	std::string colon = ":";
	std::size_t found = allocatedAddressAsString.find( colon );
	allocatedAddressAsString.replace( found + 1, 4, GAME_PORT );
	std::string firstConnectionAddress = allocatedAddressAsString;

	// Get second connection address as string
	allocatedAddressAsString = g_theNetworkingSystem->SockAddrToString( g_allocatedAddress->ai_addr );
	found = allocatedAddressAsString.find( colon );
	std::string portAsString( GAME_PORT );
	uint16_t port = ( uint16_t ) stoi( portAsString ) + ( uint16_t ) 1; // Increment port by 1 for second connection
	std::string intPortAsChar = std::to_string( port );
	allocatedAddressAsString.replace( found + 1, 4, intPortAsChar );
	std::string secondConnectionAddress = allocatedAddressAsString;

	// Get third connection address as string
	allocatedAddressAsString = g_theNetworkingSystem->SockAddrToString( g_allocatedAddress->ai_addr );
	found = allocatedAddressAsString.find( colon );
	portAsString = std::string( GAME_PORT );
	port = ( uint16_t ) stoi( portAsString ) + ( uint16_t ) 2; // Increment port by 2 for third connection
	intPortAsChar = std::to_string( port );
	allocatedAddressAsString.replace( found + 1, 4, intPortAsChar );
	std::string thirdConnectionAddress = allocatedAddressAsString;

	// Get first connection IP and port
	found = firstConnectionAddress.find( colon );
	std::string firstConnectionIP = firstConnectionAddress.substr( 0, found );
	std::string firstConnectionPort = firstConnectionAddress.substr( found + 1, std::string::npos );

	// Get second cconnection IP and port
	found = secondConnectionAddress.find( colon );
	std::string secondConnectionIP = secondConnectionAddress.substr( 0, found );
	std::string secondConnectionPort = secondConnectionAddress.substr( found + 1, std::string::npos );

	// Get third connection IP and port
	found = thirdConnectionAddress.find( colon );
	std::string thirdConnectionIP = thirdConnectionAddress.substr( 0, found );
	std::string thirdConnectionPort = thirdConnectionAddress.substr( found + 1, std::string::npos );

	sockaddr_in firstConnectionAddr;
	g_theNetworkingSystem->SockAddrFromString( firstConnectionIP.c_str(),
		( uint16_t ) atoi( firstConnectionPort.c_str() ), &firstConnectionAddr );
	sockaddr_in secondConnectionAddr;
	g_theNetworkingSystem->SockAddrFromString( secondConnectionIP.c_str(),
		( uint16_t ) atoi( secondConnectionPort.c_str() ), &secondConnectionAddr );
	sockaddr_in thirdConnectionAddr;
	g_theNetworkingSystem->SockAddrFromString( thirdConnectionIP.c_str(),
		( uint16_t ) atoi( thirdConnectionPort.c_str() ), &thirdConnectionAddr );

	g_session->CreateConnection( 2, "PLAYER2", thirdConnectionAddr );
	g_session->CreateConnection( 0, "PLAYER0", firstConnectionAddr );
	g_session->CreateConnection( 1, "PLAYER1", secondConnectionAddr );
}


//-----------------------------------------------------------------------------------------------
// Usage: net_session_destroy_connection <index>
// If the index is in-use in your session, destroy the connection.
CONSOLE_COMMAND( net_session_destroy_connection )
{
	// Fix this to handle bad input
	g_session->DestroyConnection( ( uint8_t ) atoi( args.m_argList[ 0 ].c_str() ) );
}


//-----------------------------------------------------------------------------------------------
CONSOLE_COMMAND( net_sim_lag )
{
	g_session->SetLag( std::stof( args.m_argList[ 0 ] ) );
	g_session->m_simLagMilliseconds = std::stof( args.m_argList[ 0 ] );
}


//-----------------------------------------------------------------------------------------------
CONSOLE_COMMAND( net_sim_loss )
{
	g_session->SetLoss( std::stof( args.m_argList[ 0 ] ) );
	g_session->m_simLossPercent = std::stof( args.m_argList[ 0 ] ) * 100.0f;
}


#endif