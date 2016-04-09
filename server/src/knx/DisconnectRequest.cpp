/* 
 * File:   DisconnectRequest.cpp
 * Author: Joshua Johannson
 *
 * 
 * --------------------------------------
 * DisconnectRequest CLASS
 * connection partner should disconnect
 * --------------------------------------
 */

#include "DisconnectRequest.h"


// -- CREATE OBJECT ---------------------
DisconnectRequest::DisconnectRequest(KnxConnection* connection): KnxPacket::KnxPacket(connection)
{
    // default
    setType(KNX_PACKET_DISCONNECT_REQUEST);
    setTotalLength(16);

    conHeaderChanelID = &(connection->chanelId);
}

DisconnectRequest::DisconnectRequest(KnxConnection* connection, char bytes[]): DisconnectRequest::DisconnectRequest(connection)
{
    fromBytes(bytes);
}


void DisconnectRequest::fromBytes(char bytes[])
{
    // Header
    KnxPacket::fromBytes(bytes);

    // data
    *conHeaderChanelID = bytes[6];    // given channel id
}

void DisconnectRequest::toBytes(char bytes[])
{
    // Header
    KnxPacket::toBytes(bytes);

    // data
    bytes[6] = *conHeaderChanelID & 0xff;    // given channel id
    bytes[7] = 0x00;                        // 00 -
    bytes[8] = 0x08;                        // 08 - Host Protocol Address Information (HPAI) Lenght
    bytes[9] = 0x01;                        // 01 - Host Protocol Code 0x01 -> IPV4_UDP, 0x02 -> IPV6_TCP
    bytes[10] = 0x00; // ntohl(connection->clientAddr.sin_addr.s_addr) >> 24 & 0xff;     // c0 - IP address c0 = 192 */
    bytes[11] = 0x00; // ntohl(connection->clientAddr.sin_addr.s_addr) >> 16 & 0xff;     // a8 - IP address a8 = 168 */
    bytes[12] = 0x00; // ntohl(connection->clientAddr.sin_addr.s_addr) >> 8 & 0xff;      // 0a - IP address 0a = 10 */
    bytes[13] = 0x00; // ntohl(connection->clientAddr.sin_addr.s_addr) & 0xff;           // b3 - IP address 9F = 179 */
    bytes[14] = 0x00; // (ntohs(connection->clientAddr.sin_port) >> 8) & 0xff;           // 0e - hi-byte local port number for CONNECTION, CONNECTIONSTAT and DISCONNECT requests */
    bytes[15] = 0x00; // ntohs(connection->clientAddr.sin_port) & 0xff;                  // 57 - lo-byte local port number for CONNECTION, CONNECTIONSTAT and DISCONNECT requests */
}
