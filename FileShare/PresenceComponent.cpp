#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "PresenceComponent.h"

using namespace FileShare;

SockAddrIn::SockAddrIn():
    addr{}
{
    addr.sin_family = AF_INET;
    addr.sin_port = htons(preComPort);

    addrPtr = reinterpret_cast<sockaddr*>(&addr);
    addrSize = sizeof(addr);
}

PresenceComponent::PresenceComponent()
{
    WSAstartup();
    InitializeSockaddr();
    CreateSocket();
    SetSocketOptions();
    BindSocket();
}

PresenceComponent::~PresenceComponent()
{
    CloseSocket();
}

String PresenceComponent::ReceiveBroadcastedMessage()
{   
    String buff(100, 0);

    recvfrom(sc, &buff.front(), buff.capacity(), NULL, inAddrSdr.addrPtr, &inAddrSdr.addrSize);

    buff.shrink_to_fit();

    return buff;
}

Int PresenceComponent::SendMessageBroadcast(const String& msg)
{
    return sendto(sc, msg.c_str(), msg.length(), NULL, inAddrBro.addrPtr, inAddrBro.addrSize);
}

String FileShare::PresenceComponent::GetHostIp()
{
    static WSADATA wsaData;
    int error = WSAStartup(MAKEWORD(2, 2), &wsaData);

    char hostName[1024]{};
    if (gethostname(hostName, sizeof(hostName)) == SOCKET_ERROR)
        return String();

    //HOSTENT* hostEnt = gethostbyname(hostName);
    //IN_ADDR** addrList = (IN_ADDR**)hostEnt->h_addr_list;
    //char* hostAddr = inet_ntoa(*addrList[0]);
    //
    //return hostAddr;
    return inet_ntoa(*((IN_ADDR**)(gethostbyname(hostName))->h_addr_list)[0]);
}

Int PresenceComponent::WSAstartup()
{
    return WSAStartup(MAKEWORD(2, 2), &wsa);
}
Int PresenceComponent::InitializeSockaddr()
{
    inAddrAny.addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    inAddrBro.addr.sin_addr.S_un.S_addr = htonl(INADDR_BROADCAST); 

    return 0;
}
Int PresenceComponent::CreateSocket()
{
    sc = socket(AF_INET, SOCK_DGRAM, NULL);

    return sc;
}
Int PresenceComponent::SetSocketOptions()
{
    char* scOptPtr = reinterpret_cast<char*>(&scOptions);

    return{
        setsockopt(sc, SOL_SOCKET, SO_REUSEADDR, scOptPtr, sizeof(int))
        +
        setsockopt(sc, SOL_SOCKET, SO_BROADCAST, scOptPtr, sizeof(int))
    };
}
Int PresenceComponent::BindSocket()
{
    return bind(sc, inAddrAny.addrPtr, inAddrAny.addrSize);
}
Int PresenceComponent::CloseSocket()
{
    return closesocket(sc);
}
