#include "TcpSocketedEntity.h"

#include "Logger.h"

using namespace FileShare;


#ifndef TCP_SOCKETED_ENTITY
TCPSocketedEntity::TCPSocketedEntity(SOCKET s, ULONG inetAddr, USHORT port) :
    sc(s),
    addr{}
{
    Log::InRed("TCPSocketedEntity()->");
    __Begin;
    {
        if (sc == INVALID_SOCKET)
            Log::InRedWithError("Somehow socket invalid, error: ");
        else
            Log::InRed("socket created alright");

        addr.sin_family = AF_INET;
        addr.sin_addr.S_un.S_addr = inetAddr;
        addr.sin_port = htons(port);

        addrPtr = reinterpret_cast<SOCKADDR*>(&addr);
        addrSize = sizeof(addr);

        int scOp = 1;
        setsockopt(sc, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&scOp), sizeof(int));
    }
    __End;
    Log::InRed("TCPSocketedEntity();");

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
    __Begin;
    Log::InRed("SE move-constructed");
    __End;
}
TCPSocketedEntity& TCPSocketedEntity::operator=(TCPSocketedEntity&& other)
{
    sc = other.sc;
    addr = other.addr;
    addrPtr = (SOCKADDR*)&addr;
    addrSize = sizeof(addr);

    other.sc = INVALID_SOCKET;

    __Begin;
    Log::InRed("SE move-assigned");
    __End;

    return *this;
}
TCPSocketedEntity::TCPSocketedEntity() :
    TCPSocketedEntity(INVALID_SOCKET, NULL, NULL)
{}
TCPSocketedEntity::~TCPSocketedEntity()
{
    Int result;
    Log::InRed("~TCPSocketedEntity()->");
    __Begin;
    {
        if (sc != INVALID_SOCKET)
        {
            result = closesocket(sc);

            if (result == SOCKET_ERROR)
                Log::InRedWithError("closesocket() -");
            else
                Log::InRed("closesocket() +");
        }
        else 
        {
            Log::InRed("~SocketedEntity()/closesocket()");
        }
    }
    __End;
    Log::InRed("<~TCPSocketedEntity();");
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
    __Begin;
    Log::InRed("listener constructing");
    __End;
}

Int Listener::Bind()
{
    Int result = SOCKET_ERROR;
    Log::InRed("Bind()->");
    __Begin;
    {
        result = bind(sc, addrPtr, addrSize);
        if (result == SOCKET_ERROR)
            Log::InRedWithError("listener socket bind failed, error: ");
        else
            Log::InRed("listener socket bound");
    }
    __End;
    Log::InRed("Bind();");
    return result;
}
Int Listener::Listen()
{
    Int result = SOCKET_ERROR;
    Log::InRed("Listen()->");
    __Begin;
    {
        result = listen(sc, 1);
        if (result == SOCKET_ERROR)
            Log::InRedWithError("listener socket failed to start listening, error: ");
        else
            Log::InRed("listener socket is listening alright");
    }
    __End;
    Log::InRed("Listen();");
    return result;
}
TCPSocketedEntity Listener::Accept()
{
    Log::InRed("Accept()->");
    __Begin;
    {
        Log::InRed("creating invalid socket for acceptance");

        TCPSocketedEntity se;
        se.sc = accept(sc, se.addrPtr, &se.addrSize);
        {
            if (se.sc == INVALID_SOCKET)
                Log::InRedWithError("accepted socket invalid");
            else
                Log::InRed("socket accepted alright");

            Log::InRed("moving SocEnt");
        }
        __End;
        Log::InRed("<-Accept()");
        return std::move(se);
    }
}
#endif TCP_LISTENER

#ifndef TCP_RECEIVER

Receiver::Receiver(TCPSocketedEntity&& target) :
    TCPSocketedEntity(target.sc, target.addr.sin_addr.S_un.S_addr, ntohs(addr.sin_port))
{
    __Begin;
    {
        target.sc = INVALID_SOCKET;
        Log::InRed("receiver created");
    }
    __End;
}
String Receiver::ReceiveMessage()
{
    CHAR buff[128]{};
    Log::InRed("ReceiveMessage()->");
    __Begin;
    {
        Int result = recv(sc, buff, sizeof(buff), NULL);

        if (result == SOCKET_ERROR)
            Log::InRedWithError("Receive failed, error: ");
        else if (result == NULL)
            Log::InRed("connection ??gracefully?? closed");
        else
            Log::InRed("received smth");
    }
    __End;
    Log::InRed("ReceiveMessage();");

    return String(buff);
}
#endif TCP_RECEIVER

#ifndef TCP_SENDER

Sender::Sender(SOCKET sc, ULONG addr, USHORT port) :
    TCPSocketedEntity(sc, addr, port)
{
    Log::InRed("Created sender");
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

Sender::Sender(TCPSocketedEntity&& target):
    TCPSocketedEntity(target.sc, target.addr.sin_addr.S_un.S_addr, ntohs(addr.sin_port))
{
    target.sc = SOCKET_ERROR;
}

Int Sender::ConnectToUser()
{
    Int result = SOCKET_ERROR;
    Log::InRed("ConnectToUser()->");
    __Begin;
    {
        result = connect(sc, addrPtr, addrSize);

        if (result == SOCKET_ERROR)
            Log::InRedWithError("failed to connect, error: ");
        else
            Log::InRed("socket connected alright");
    }
    __End;
    Log::InRed("ConnectToUser();");
    return result;
}
Int Sender::SendMessageToUser(const String & buff)
{
    Int result = SOCKET_ERROR;
    Log::InRed("SendMessageToUser()->");
    __Begin;
    {
        Log::InRed(buff);
        result = send(sc, buff.c_str(), buff.length(), NULL);

        if (result < 0)
            Log::InRedWithError("send failed, error: ");
        else if (result == 0)
            Log::InRedWithError("nothing sent, error: ");
        else
            Log::InRed("message probably sent");
    }
    __End;
    Log::InRed("SendMessageToUser();");
    return result;
}

#endif TCP_SENDER

#endif !TCP_SOCKETED_ENTITY
