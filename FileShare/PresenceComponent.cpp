#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "PresenceComponent.h"

#define DontLog
#include "Logger.h"

using namespace FileShare;

SockAddrIn::SockAddrIn():
    SockAddrIn(preComPort)
{}

SockAddrIn::SockAddrIn(USHORT port):
    addr{}
{
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addrPtr = reinterpret_cast<sockaddr*>(&addr);
    addrSize = sizeof(addr);
}

BasicSocketedEntity::BasicSocketedEntity(USHORT port) :
    inAddrAny(port),
    inAddrBro(port)
{}

PresenceComponent::PresenceComponent():
    PresenceComponent(preComPort)
{}
PresenceComponent::PresenceComponent(USHORT pcPort) :
    BasicSocketedEntity(pcPort)
{
    Log::InRed("PresenceComponent()->");
    __Begin;
    {
        WSAstartup();
        InitializeSockaddr();
        CreateSocket();
        SetSocketOptions();
        BindSocket();
    }
    __End;
    Log::InRed("<-PresenceComponent();");
}
PresenceComponent::~PresenceComponent()
{
    Log::InRed("~PresenceComponent()->");
    __Begin;
    {
        CloseSocket();
    }
    __End;
    Log::InRed("<~PresenceComponent();");
}

STR PresenceComponent::ReceiveBroadcastedMessage()
{   
    //STR buff(100, 0);
    CHAR buff[100]{};
    recvfrom(sc, buff,sizeof(buff), NULL, inAddrSdr.addrPtr, &inAddrSdr.addrSize);

    return buff;
}
Int PresenceComponent::SendMessageBroadcast(const STR& msg)
{
    return sendto(sc, msg.c_str(), msg.length(), NULL, inAddrBro.addrPtr, inAddrBro.addrSize);
}

STR PresenceComponent::GetHostIp()
{
    static WSADATA wsaData;
    int error = WSAStartup(MAKEWORD(2, 2), &wsaData);

    char hostName[1024]{};
    if (gethostname(hostName, sizeof(hostName)) == SOCKET_ERROR)
        return STR();

    return inet_ntoa(*((IN_ADDR**)(gethostbyname(hostName))->h_addr_list)[0]);
}

Int PresenceComponent::GetAuraCount()
{
    return activeLocalBroadcasters.size();
}

Int PresenceComponent::WSAstartup()
{
    Int wsaStartupResult;
    Log::InRed("WSAstartup()->");
    __Begin;
    {
        wsaStartupResult = WSAStartup(MAKEWORD(2, 2), &wsa);

    }
    __End;
    Log::InRed("<-WSAstartup();");
    return wsaStartupResult;
}
Int PresenceComponent::InitializeSockaddr()
{
    Log::InRed("InitializeSockaddr()->");
    __Begin;
    {
        inAddrAny.addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
        inAddrBro.addr.sin_addr.S_un.S_addr = htonl(INADDR_BROADCAST);
    }
    __End;
    Log::InRed("<-InitializeSockaddr()");
    return 0;
}
Int PresenceComponent::CreateSocket()
{
    Log::InRed("CreateSocket()->");
    __Begin;
    {
        sc = socket(AF_INET, SOCK_DGRAM, NULL);
    }
    __End;
    Log::InRed("<-CreateSocket();");
    return sc;
}
Int PresenceComponent::SetSocketOptions()
{
    Int result = 0;
    Log::InRed("SetSocketOptions()->");
    __Begin;
    {
        char* scOptPtr = reinterpret_cast<char*>(&scOptions);
        result += setsockopt(sc, SOL_SOCKET, SO_REUSEADDR, scOptPtr, sizeof(Int));
        result += setsockopt(sc, SOL_SOCKET, SO_BROADCAST, scOptPtr, sizeof(Int));
    }
    __End;
    Log::InRed("<-SetSocketOptions()");
    return result;
}
Int PresenceComponent::BindSocket()
{
    Int result;
    Log::InRed("BindSocket()->");
    __Begin;
    {
        result = bind(sc, inAddrAny.addrPtr, inAddrAny.addrSize);
    }
    __End;
    Log::InRed("<-BindSocket()");
    return result;
}
Int PresenceComponent::CloseSocket()
{
    Int result;
    Log::InRed("CloseSocket()->");
    __Begin;
    {
        result = closesocket(sc);
    }
    __End;
    Log::InRed("<-CloseSocket()");
    return result;
}
