#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS 

//#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#include <vector>
#include <tuple>
#include <string>
#include <chrono>

namespace FileShare {

    using String = std::string;
    using Int = int;
    using Bool = int;

    #define messPort (USHORT)0x1488


#ifndef TCP_SOCKETED_ENTITY
    class TCPSocketedEntity
    {
    public:
        TCPSocketedEntity();
        ~TCPSocketedEntity();
        TCPSocketedEntity(TCPSocketedEntity&& other);

        TCPSocketedEntity(SOCKET sc, ULONG addr, USHORT port);
        TCPSocketedEntity(SOCKET sc, String addr, USHORT port);
        TCPSocketedEntity(Int scType, Int protocol, ULONG inetAddr, USHORT port);


        TCPSocketedEntity& operator = (TCPSocketedEntity&& other);

        Bool CompareSockaddr(const TCPSocketedEntity& other) const;

        Bool InvalidSocket() const;

        SOCKET sc;
        SOCKADDR_IN addr;
        SOCKADDR* addrPtr;
        INT addrSize;
    };

#ifndef TCP_LISTENER
    class Listener :
        public TCPSocketedEntity
    {
    public:
        Listener(USHORT port);
        Int Bind();
        Int Listen();
        TCPSocketedEntity Accept();
    };
#endif

#ifndef TCP_SENDER
    class Sender :
        public TCPSocketedEntity
    {
    public:
        Sender(SOCKET sc, ULONG addr, USHORT port);
        Sender(Int scType, Int protocol, ULONG addr, USHORT port);
        Sender(ULONG addr, USHORT port);
        Sender(String addr, USHORT port);

        Int ConnectToUser();
        Int SendMessageToUser(String const& msg);
    protected:
        Sender() = delete;
    };
#endif TCP_SENDER

#ifndef TCP_RECEIVER
    class Receiver :
        public TCPSocketedEntity
    {
    public:
        Receiver(TCPSocketedEntity&&);
        //Receiver& operator = (TCPSocketedEntity&&);
        String ReceiveMessage();
    };
#endif TCP_RECEIVER

#endif TCP_SOCKETED_ENTITY
}