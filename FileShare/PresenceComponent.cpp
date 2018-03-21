#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "PresenceComponent.h"
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
    {
        #ifdef LOGGER
        Log::TextInRed(PresenceComponent()->);
        __Begin;
        #endif
    }
    WSAstartup();
    InitializeSockaddr();
    CreateSocket();
    SetSocketOptions();
    BindSocket();
    {
        #ifdef LOGGER
        __End;
        Log::TextInRed(<-PresenceComponent());
        #endif
    }
}

PresenceComponent::~PresenceComponent()
{
    CloseSocket();
}

STR PresenceComponent::ReceiveBroadcastedMessage()
{   
    STR buff(100, 0);

    recvfrom(sc, &buff.front(), buff.capacity(), NULL, inAddrSdr.addrPtr, &inAddrSdr.addrSize);

    buff.shrink_to_fit();

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

    //HOSTENT* hostEnt = gethostbyname(hostName);
    //IN_ADDR** addrList = (IN_ADDR**)hostEnt->h_addr_list;
    //char* hostAddr = inet_ntoa(*addrList[0]);
    //
    //return hostAddr;
    return inet_ntoa(*((IN_ADDR**)(gethostbyname(hostName))->h_addr_list)[0]);
}

Int PresenceComponent::GetAuraCount()
{
    return activeLocalBroadcasters.size();
}

Int PresenceComponent::WSAstartup()
{
    Int wsaStartupResult;
    {
        #ifdef LOGGER
        Log::TextInRed(WSAstartup()->);
        __Begin;
        #endif
    }
    wsaStartupResult = WSAStartup(MAKEWORD(2, 2), &wsa);
    {
        #ifdef LOGGER
        __End;
        Log::TextInRed(<-WSAstartup());
        #endif
    }
    return wsaStartupResult;
}
Int PresenceComponent::InitializeSockaddr()
{
    {
        #ifdef LOGGER
        Log::TextInRed(InitializeSockaddr()->);
        __Begin;
        #endif
    }
    inAddrAny.addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    inAddrBro.addr.sin_addr.S_un.S_addr = htonl(INADDR_BROADCAST); 
    {
        #ifdef LOGGER
        __End;
        Log::TextInRed(<-InitializeSockaddr());
        #endif
    }
    return 0;
}
Int PresenceComponent::CreateSocket()
{
    {
        #ifdef LOGGER
        Log::TextInRed(->CreateSocket());
        __Begin;
        #endif
    }
    sc = socket(AF_INET, SOCK_DGRAM, NULL);
    {
        #ifdef LOGGER
        __End;
        Log::TextInRed(<-CreateSocket());
        #endif
    }
    return sc;
}
Int PresenceComponent::SetSocketOptions()
{
    Int result = 0;
    {
        char* scOptPtr = reinterpret_cast<char*>(&scOptions);
        {
            #ifdef LOGGER
            Log::TextInRed(SetSocketOptions()->);
            __Begin;
            #endif
        }
        result += setsockopt(sc, SOL_SOCKET, SO_REUSEADDR, scOptPtr, sizeof(Int));
        result += setsockopt(sc, SOL_SOCKET, SO_BROADCAST, scOptPtr, sizeof(Int));
        {
            #ifdef LOGGER
            __End;
            Log::TextInRed(<-SetSocketOptions());
            #endif
        }
    }
    return result;
}
Int PresenceComponent::BindSocket()
{
    Int result;
    {
        {
            #ifdef LOGGER
            Log::TextInRed(BindSocket()->);
            __Begin;
            #endif
        }
        result = bind(sc, inAddrAny.addrPtr, inAddrAny.addrSize);
        {
            #ifdef LOGGER
            __End;
            Log::TextInRed(<-BindSocket());
            #endif
        }
    }
    return result;
}
Int PresenceComponent::CloseSocket()
{
    Int result;
    {    
        {
            #ifdef LOGGER
            Log::TextInRed(CloseSocket()->);
            __Begin;
            #endif
        }
        result = closesocket(sc);
        {
            #ifdef LOGGER
            __End;
            Log::TextInRed(<-CloseSocket());
            #endif
        }
    }
    return result;
}
