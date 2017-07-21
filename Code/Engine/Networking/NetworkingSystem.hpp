#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <winsock2.h>

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/DeveloperConsole.hpp"

#pragma comment( lib, "ws2_32.lib" ) // Need to link with Ws2_32.lib

#define COMMAND_SERVICE "4325"
#define GAME_PORT "4334"


//-----------------------------------------------------------------------------------------------
class NetworkingSystem;
class Session;
struct addrinfo;


//-----------------------------------------------------------------------------------------------
extern NetworkingSystem* g_theNetworkingSystem;


//-----------------------------------------------------------------------------------------------
class NetworkingSystem
{
public:
	NetworkingSystem();
	~NetworkingSystem();
	void Update( float deltaSeconds );
	void ProcessDataReceived();
	void GetNewClient();
	const char* GetLocalHostName();
	addrinfo* AllocateAddressesForHost( const char* host, // host, like google.com
		const char* service, // service, usually the port number as a string
		int family,			 // Connection Family, AF_INET (IPv4) for this assignment
		int socktype,		 // Socket Type, SOCK_STREAM or SOCK_DGRAM (TCP or UDP) for this class
		int flags = 0 );	 // Search flag hints, we use this for AI_PASSIVE (bindable addresses)
	void FreeAddresses( addrinfo* addresses );
	SOCKET CreateListenSocket( const char* addr, const char* service, // who we're trying to connect to
		sockaddr_in *out_addr ); // address we actually connected to
	SOCKET AcceptConnection( SOCKET host_socket, sockaddr_in *out_their_addr );
	SOCKET SocketJoin( const char* addr, const char* service, sockaddr_in *out_addr );
	void CloseSocket( SOCKET sock ) { closesocket( sock ); }
	size_t SocketSend( bool *out_should_disconnect,
		SOCKET my_socket,
		void const *data,
		size_t const data_size );
	size_t SocketReceive( bool *out_should_disconnect,
		SOCKET my_socket,
		void *buffer,
		size_t const buffer_size );
	char const* SockAddrToString( sockaddr const *addr );
	char const* SockAddrToStringWithoutPort( sockaddr const *addr );
	bool SockAddrFromString( const char* ip, const uint16_t port, sockaddr_in* outAddress );
	void* GetInAddr( sockaddr const *sa );
	bool SocketErrorShouldDisconnect( int32_t const error );

	// New for A4
	static Session* GetLocalSession();
};