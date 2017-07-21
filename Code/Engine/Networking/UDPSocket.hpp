#pragma once

#include "Engine/Networking/NetworkingSystem.hpp"


//-----------------------------------------------------------------------------------------------
class UDPSocket
{
public:
	UDPSocket();
	SOCKET Create( char const *addr, char const *service, sockaddr_in *out_addr );
	size_t SendTo( sockaddr_in &to_addr, void const *data, size_t const data_size );
	size_t ReceiveFrom( sockaddr_in *from_addr, void *buffer, size_t const buffer_size );

public:
	SOCKET m_socket;
};