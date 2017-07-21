#include "Engine/Networking/PacketChannel.hpp"
#include "Engine/Core/Time.hpp"


//-----------------------------------------------------------------------------------------------
PacketChannel::PacketChannel()
	: m_additionalLagMilliseconds( 0.0f )
	, m_dropRatePercentage( 0.0f )
{
	m_socketWrapper = new UDPSocket();
}


//-----------------------------------------------------------------------------------------------
SOCKET PacketChannel::Create( char const *addr, char const *service, sockaddr_in *out_addr )
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

	m_socketWrapper->m_socket = my_socket;
	return my_socket;
}


//-----------------------------------------------------------------------------------------------
size_t PacketChannel::SendTo( sockaddr_in &to_addr, void const *data, size_t const data_size )
{
	if ( m_socketWrapper->m_socket != INVALID_SOCKET )
	{
		int size = ::sendto( m_socketWrapper->m_socket, ( char const* ) data, ( int ) data_size, 0,
			( sockaddr const* ) &to_addr, sizeof( sockaddr_in ) );

		if ( size > 0 )
		{
			return size;
		}
	}

	return 0;
}


//-----------------------------------------------------------------------------------------------
size_t PacketChannel::ReceiveFrom( sockaddr_in *out_from_addr, void *buffer, size_t const buffer_size )
{
// 	if ( m_socketWrapper->m_socket != INVALID_SOCKET )
// 	{
// 		sockaddr_storage addr;
// 		int addrlen = sizeof( addr );
// 
// 		int size = ::recvfrom( m_socketWrapper->m_socket, ( char* ) buffer, buffer_size, 0, ( sockaddr* ) &addr,
// 			&addrlen );
// 
// 		if ( size > 0 )
// 		{
// 			ASSERT_OR_DIE( addrlen == sizeof( sockaddr_in ), "addrlen != sizeof( sockaddr_in )" );
// 
// 			memcpy( out_from_addr, &addr, addrlen );
// 			return size;
// 		}
// 	}
// 
// 	return 0U;
	TimeStampedPacket* timeStampedPacket = new TimeStampedPacket();
	sockaddr_in fromAddr;
	size_t read = m_socketWrapper->ReceiveFrom( &fromAddr, timeStampedPacket->packet.m_buffer, buffer_size );
	if ( read > 0 )
	{
		if ( GetRandomChance( m_dropRatePercentage ) )
		{
			delete timeStampedPacket;
			timeStampedPacket = nullptr;
		}
		else
		{
			double delayMilliseconds = m_additionalLagMilliseconds;
			timeStampedPacket->packet.m_currentContentSize = read;
			timeStampedPacket->timeToProcess = GetCurrentTimeSeconds() * 1000.0 + delayMilliseconds;
			timeStampedPacket->fromAddr = fromAddr;
			m_inboundPackets.insert( std::pair< double, TimeStampedPacket* >( timeStampedPacket->timeToProcess,
				timeStampedPacket ) );
		}
	}
	else
	{
		delete timeStampedPacket;
		timeStampedPacket = nullptr;
	}

	if ( m_inboundPackets.size() > 0 )
	{
		double currentTimeMilliseconds = GetCurrentTimeSeconds() * 1000.0;
		TimeStampedPacket* inboundPacket = m_inboundPackets.begin()->second;
		if ( currentTimeMilliseconds >= inboundPacket->timeToProcess )
		{
			int addrlen = sizeof( inboundPacket->fromAddr );
			memcpy( out_from_addr, &inboundPacket->fromAddr, addrlen );
			size_t size = inboundPacket->packet.m_currentContentSize;
			memcpy( ( char* ) buffer, inboundPacket->packet.m_buffer, size );
			m_inboundPackets.erase( m_inboundPackets.begin() );
			return size;
		}
	}
	return 0;
}