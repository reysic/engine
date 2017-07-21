#include "Engine/Networking/UDPSocket.hpp"


//-----------------------------------------------------------------------------------------------
UDPSocket::UDPSocket()
{
}


//-----------------------------------------------------------------------------------------------
SOCKET UDPSocket::Create( char const *addr, char const *service, sockaddr_in *out_addr )
{
	addrinfo *info_list = g_theNetworkingSystem->AllocateAddressesForHost( addr, service, AF_INET,
		SOCK_DGRAM, AI_PASSIVE );

	if ( info_list == nullptr )
	{
		return false;
	}

	SOCKET my_socket = INVALID_SOCKET;
	addrinfo *iter = info_list;
	while ( ( iter != nullptr ) && ( my_socket == INVALID_SOCKET ) )
	{
		my_socket = socket( iter->ai_family, iter->ai_socktype, iter->ai_protocol );
		if ( my_socket != INVALID_SOCKET )
		{
			int result = bind( my_socket, iter->ai_addr, ( int ) ( iter->ai_addrlen ) );
			if ( SOCKET_ERROR != result )
			{
				u_long non_blocking = 1;
				ioctlsocket( my_socket, FIONBIO, &non_blocking );

				ASSERT_OR_DIE( iter->ai_addrlen == sizeof( sockaddr_in ), "addrlen != sockaddr_in" );
				if ( nullptr != out_addr )
				{
					memcpy( out_addr, iter->ai_addr, iter->ai_addrlen );
				}
			}
			else
			{
				// Cleanup on Fail.
				closesocket( my_socket );
				my_socket = INVALID_SOCKET;
			}
		}
		iter = iter->ai_next;
	}

	g_theNetworkingSystem->FreeAddresses( info_list );

	m_socket = my_socket;
	return my_socket;
}


//-----------------------------------------------------------------------------------------------
size_t UDPSocket::SendTo( sockaddr_in &to_addr, void const *data, size_t const data_size )
{
	if ( m_socket != INVALID_SOCKET )
	{
		int size = ::sendto( m_socket, ( char const* ) data, ( int ) data_size, 0,
			( sockaddr const* ) &to_addr, sizeof( sockaddr_in ) );

		if ( size > 0 )
		{
			return size;
		}
	}

	return 0;
}


//-----------------------------------------------------------------------------------------------
size_t UDPSocket::ReceiveFrom( sockaddr_in *from_addr, void *buffer, size_t const buffer_size )
{
	if ( m_socket != INVALID_SOCKET )
	{
		sockaddr_storage addr;
		int addrlen = sizeof( addr );

		int size = ::recvfrom( m_socket, ( char* ) buffer, buffer_size, 0, ( sockaddr* ) &addr,
			&addrlen );

		if ( size > 0 )
		{
			ASSERT_OR_DIE( addrlen == sizeof( sockaddr_in ), "addrlen != sizeof( sockaddr_in )" );

			memcpy( from_addr, &addr, addrlen );
			return size;
		}
	}

	return 0U;
}
