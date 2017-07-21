#pragma once

#include "Engine/Networking/NetworkingSystem.hpp"
#include "Engine/Networking/Packet.hpp"
#include "Engine/Networking/UDPSocket.hpp"


//-----------------------------------------------------------------------------------------------
struct TimeStampedPacket
{
	Packet packet;
	double timeToProcess;
	sockaddr_in fromAddr;
};


//-----------------------------------------------------------------------------------------------
class PacketChannel
{
public:
	PacketChannel();
	SOCKET Create( char const *addr, char const *service, sockaddr_in *out_addr );
	size_t SendTo( sockaddr_in &to_addr, void const *data, size_t const data_size );
	size_t ReceiveFrom( sockaddr_in *from_addr, void *buffer, size_t const buffer_size );

public:
	UDPSocket* m_socketWrapper;
	std::map< double, TimeStampedPacket* > m_inboundPackets;
	float m_additionalLagMilliseconds;
	float m_dropRatePercentage;
};