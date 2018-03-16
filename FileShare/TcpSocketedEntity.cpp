#include "TcpSocketedEntity.h"
#include "Logger.h"

using namespace FileShare;


#ifndef TCP_SOCKETED_ENTITY
TCPSocketedEntity::TCPSocketedEntity(SOCKET s, ULONG inetAddr, USHORT port) :
    sc(s),
    addr{}
{
    #ifdef LOGGER
    {
        ++Log::depth;

        Log::InRed("Constructing SocketedEntity");

        IF(sc == INVALID_SOCKET)
            Log::InRedWithError("Somehow socket invalid, error: ");
        ELSE
            Log::InRed("socket created alright");

        --Log::depth;
    }
    #endif LOGGER

    addr.sin_family = AF_INET;
    addr.sin_addr.S_un.S_addr = inetAddr;
    addr.sin_port = htons(port);

    addrPtr = reinterpret_cast<SOCKADDR*>(&addr);
    addrSize = sizeof(addr);

    int scOp = 1;
    setsockopt(sc, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&scOp), sizeof(int));
}
TCPSocketedEntity::TCPSocketedEntity(Int scType, Int ptc, ULONG inetAddr, USHORT port) :
    TCPSocketedEntity(socket(AF_INET, scType, ptc), inetAddr, port)
{}
TCPSocketedEntity::TCPSocketedEntity(SOCKET sc, String addr, USHORT port) :
    TCPSocketedEntity(sc, inet_addr(addr.c_str()), port)
{}
TCPSocketedEntity::TCPSocketedEntity(TCPSocketedEntity&& other) :
    TCPSocketedEntity(other.sc, other.addr.sin_addr.S_un.S_addr, ntohs(addr.sin_port))
{
    other.sc = INVALID_SOCKET;
}
TCPSocketedEntity& TCPSocketedEntity::operator=(TCPSocketedEntity&& other)
{
    sc = other.sc;
    addr = other.addr;
    addrPtr = (SOCKADDR*)&addr;
    addrSize = sizeof(addr);

    other.sc = INVALID_SOCKET;

    return *this;
}
TCPSocketedEntity::TCPSocketedEntity() :
    TCPSocketedEntity(INVALID_SOCKET, NULL, NULL)
{}
TCPSocketedEntity::~TCPSocketedEntity()
{
    Int result{};
    if (sc != INVALID_SOCKET) {
        result = closesocket(sc);
        #ifdef LOGGER
        {
            ++Log::depth;
            IF(result == SOCKET_ERROR)
                Log::InRedWithError("~SocketedEntity failed to close socket, error: ");
            ELSE
                Log::InRed("~SocketedEntity socket closed");
            --Log::depth;
        }
        #endif LOGGER
    }
    else {
        {
            #ifdef LOGGER
            Log::InRed("~SocketedEntity without closing sc");
            #endif
        }
    }
}

Bool TCPSocketedEntity::CompareSockaddr(const TCPSocketedEntity& s2) const {
    const TCPSocketedEntity& s1 = *this;
    return{
        s1.addr.sin_family == s2.addr.sin_family           &&
        s1.addr.sin_addr.S_un.S_addr == s2.addr.sin_addr.S_un.S_addr &&
        s1.addr.sin_port == s2.addr.sin_port
    };;
}

Bool FileShare::TCPSocketedEntity::InvalidSocket() const
{
    return sc == INVALID_SOCKET;
}

#ifndef TCP_LISTENER
Listener::Listener(USHORT port) :
    TCPSocketedEntity(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP), INADDR_ANY, port)
{
    #ifdef LOGGER
    {
        ++Log::depth;
        Log::InRed("listener constructing");
        --Log::depth;
    }
    #endif
}
Listener::Listener(TCPSocketedEntity&& target):
    TCPSocketedEntity(std::move(target))
{
    #ifdef LOGGER
    {
        ++Log::depth;
        Log::InRed("listener constructing");
        --Log::depth;
    }
    #endif
}
Int Listener::Bind()
{
    Int result = SOCKET_ERROR;

    #ifdef LOGGER
    {
        ++Log::depth;
        Log::InRed("listener trying to bind");
    }
    #endif LOGGER

    result = bind(sc, addrPtr, addrSize);

    #ifdef LOGGER
    {
        IF(result == SOCKET_ERROR)
            Log::InRedWithError("listener socket bind failed, error: ");
        ELSE
            Log::InRed("listener socket bound");
        --Log::depth;
    }
    #endif LOGGER

    return result;
}
Int Listener::Listen()
{
    Int result = SOCKET_ERROR;

    #ifdef LOGGER
    {
        ++Log::depth;
        Log::InRed("listener starts to listen");
    }
    #endif

    result = listen(sc, 1);

    #ifdef LOGGER
    {
        IF(result == SOCKET_ERROR)
            Log::InRedWithError("listener socket failed to start listening, error: ");
        ELSE
            Log::InRed("listener socket is listening alright");
        --Log::depth;
    }
    #endif LOGGER

    return result;
}
TCPSocketedEntity Listener::Accept()
{
    #ifdef LOGGER
    {
        ++Log::depth;
        Log::InRed("listener trying to accept");
        Log::InRed("creating invalid socket for acceptance");
    }
    #endif LOGGER

    TCPSocketedEntity se;
    se.sc = accept(sc, se.addrPtr, &se.addrSize);

    #ifdef LOGGER
    {
        IF(se.sc == INVALID_SOCKET)
            Log::InRedWithError("accepted socket invalid");
        ELSE
            Log::InRed("socket accepted alright");

        Log::InRed("moving SocEnt");
        --Log::depth;
    }
    #endif LOGGER

    return std::move(se);
}
#endif TCP_LISTENER

#ifndef TCP_RECEIVER

Receiver::Receiver(TCPSocketedEntity&& target) :
    TCPSocketedEntity(std::move(target))
{
    #ifdef LOGGER
    {
        ++Log::depth;
        Log::InRed("receiver created");
        --Log::depth;
    }
    #endif
}
String Receiver::ReceiveMessage()
{
    Int result;

    #ifdef LOGGER
    {
        ++Log::depth;
        Log::InRed("Receiver trying to recv");
    }
    #endif

    char buff[128]{};

    result = recv(sc, buff, sizeof(buff), NULL);

    #ifdef LOGGER
    {
        IF(result == SOCKET_ERROR)
            Log::InRedWithError("Receive failed, error: ");
        ELIF(result == NULL)
            Log::InRed("connection ??gracefully?? closed");
        ELSE
            Log::InRed("received smth");
    }
    #endif LOGGGER

    return String(buff);
}
#endif TCP_RECEIVER

#ifndef TCP_SENDER

Sender::Sender(TCPSocketedEntity&& target):
    TCPSocketedEntity(std::move(target))
{
    {
        #ifdef LOGGER
        ++Log::depth;
        Log::InRed("Created sender");
        --Log::depth;
        #endif LOGGER
    }
}

Sender::Sender(SOCKET sc, ULONG addr, USHORT port) :
    TCPSocketedEntity(sc, addr, port)
{
    #ifdef LOGGER
    {
        ++Log::depth;
        Log::InRed("Created sender");
        --Log::depth;
    }
    #endif LOGGER
}
Sender::Sender(Int scType, Int ptc, ULONG addr, USHORT port) :
    Sender(socket(AF_INET, scType, ptc), addr, port)
{}
Sender::Sender(ULONG addr, USHORT port) :
    Sender(socket(AF_INET, SOCK_STREAM, 0), addr, port)
{}
Sender::Sender(String addr, USHORT port) :
    Sender(inet_addr(addr.c_str()), port)
{}

Int Sender::ConnectToUser()
{
    Int result = SOCKET_ERROR;

    #ifdef LOGGER
    {
        ++Log::depth;
        Log::InRed("sender trying to connect");
    }
    #endif
    
    result = connect(sc, addrPtr, addrSize);

    #ifdef LOGGER
    {
        ++Log::depth;
        IF(result == SOCKET_ERROR)
            Log::InRedWithError("failed to connect, error: ");
        ELSE
            Log::InRed("socket connected alright");
        --Log::depth;
    }
    #endif LOGGER

    return result;
}
Int Sender::SendMessageToUser(const String & buff)
{
    Int result = SOCKET_ERROR;

    #ifdef LOGGER
    {
        ++Log::depth;
        Log::InRed("sender trying to send");
    }
    #endif

    result = send(sc, buff.c_str(), buff.length(), NULL);

    #ifdef LOGGER
    {
        IF(result < 0)
            Log::InRedWithError("send failed, error: ");
        ELIF(result == 0)
            Log::InRedWithError("nothing sent, error: ");
        ELSE
            Log::InRed("message probably sent");

        --Log::depth;
    }
    #endif LOGGER

    return result;
}
//
//TCPSocketedEntity Sender::UserDataToSocketedentity(const UserData& usr)
//{
//    SOCKET sc = socket(AF_INET, SOCK_STREAM, NULL);
//
//    #ifdef LOGGER
//    {
//        IF(sc == INVALID_SOCKET)
//            InRedWithError("UDtoSE failed to create socket, error: ");
//        ELSE
//            InRed("UDtoSE created socket");
//    }
//    #endif LOGGER
//
//    ULONG inetAddr = inet_addr(usr.Address().to_str().c_str());
//    USHORT port = usr.Address().port;
//    return TCPSocketedEntity(sc, inetAddr, port);
//}
#endif TCP_SENDER

#endif !TCP_SOCKETED_ENTITY
