#include "MessengerComponent.h"
#include "Logger.h"
#include <ctime>

using namespace FileShare;

#ifndef TCP_SOCKETED_ENTITY
TCPSocketedEntity::TCPSocketedEntity(SOCKET s, ULONG inetAddr, USHORT port) :
    sc(s),
    addr{}
{
    #ifdef LOGGER
    {
        InRed("Constructing SocketedEntity");

        IF(sc == INVALID_SOCKET)
            InRedWithError("Somehow socket invalid, error: ");
        ELSE
            InRed("socket created alright");
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
TCPSocketedEntity::TCPSocketedEntity(Int scType, Int ptc, ULONG inetAddr, USHORT port):
    TCPSocketedEntity(socket(AF_INET, scType, ptc), inetAddr, port)
{}
TCPSocketedEntity::TCPSocketedEntity(SOCKET sc, String addr, USHORT port):
    TCPSocketedEntity(sc, inet_addr(addr.c_str()), port)
{}
TCPSocketedEntity::TCPSocketedEntity(TCPSocketedEntity&& other):
    TCPSocketedEntity(other.sc, other.addr.sin_addr.S_un.S_addr, ntohs(addr.sin_port))
    /*sc(other.sc),
    addr(other.addr),
    addrPtr((SOCKADDR*)&addr),
    addrSize(sizeof(addr))*/
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
TCPSocketedEntity::TCPSocketedEntity():
    TCPSocketedEntity(INVALID_SOCKET, NULL, NULL)
{}
TCPSocketedEntity::~TCPSocketedEntity()
{
    Int result{};
    if (sc != INVALID_SOCKET) {
        result = closesocket(sc);
        #ifdef LOGGER
        {
            IF(result == SOCKET_ERROR)
                InRedWithError("failed to close socket, error: ");
            ELSE
                InRed("socket closed");
        }
        #endif LOGGER
    }
}

Bool TCPSocketedEntity::CompareSockaddr(const TCPSocketedEntity& s2) const{
    const TCPSocketedEntity& s1 = *this;
    return{
        s1.addr.sin_family              == s2.addr.sin_family           &&
        s1.addr.sin_addr.S_un.S_addr    == s2.addr.sin_addr.S_un.S_addr &&
        s1.addr.sin_port                == s2.addr.sin_port
    };;
}

#ifndef TCP_LISTENER
Listener::Listener(USHORT port) :
    TCPSocketedEntity(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP), INADDR_ANY, port)
{
    #ifdef LOGGER
    {
        InRed("listener constructing");
    }
    #endif
}
Int Listener::Bind()
{
    Int result = bind(sc, addrPtr, addrSize);

    #ifdef LOGGER
    {
        IF (result == SOCKET_ERROR)
            InRedWithError("listener socket bind failed, error: ");
        ELSE
            InRed("listener socket bound");
    }
    #endif LOGGER

    return result;
}
Int Listener::Listen()
{
    Int result = listen(sc, 1);

    #ifdef LOGGER
    {
        IF (result == SOCKET_ERROR)
            InRedWithError("listener socket failed to start listening, error: ");
        ELSE
            InRed("listener socket is listening alright");
    }
    #endif LOGGER

    return result;
}
TCPSocketedEntity Listener::Accept()
{
    #ifdef LOGGER
    {
        InRed("creating invalid socket for acceptance");
    }
    #endif LOGGER
    TCPSocketedEntity se;
    se.sc = accept(sc, se.addrPtr, &se.addrSize);

    #ifdef LOGGER
    {
        IF (se.sc == INVALID_SOCKET)
            InRedWithError("accepted socket invalid");
        ELSE
            InRed("socket accepted alright");
    }
    #endif LOGGER

    return std::move(se);
}
#endif TCP_LISTENER

#ifndef TCP_RECEIVER

Receiver::Receiver(TCPSocketedEntity&& se) :
    TCPSocketedEntity(std::move(se))
{
    #ifdef LOGGER
    {
        InRed("receiver created");
    }
    #endif
}
String Receiver::ReceiveMessage()
{
    char buff[100]{};
    Int result = recv(sc, buff, sizeof(buff), NULL);
    
    #ifdef LOGGER
    {
        IF(result == SOCKET_ERROR)
            InRedWithError("Receive failed, error: ");
        ELSE
            InRed("received smth");
    }
    #endif LOGGGER

    return buff;
}
#endif TCP_RECEIVER

#ifndef TCP_SENDER

Sender::Sender(SOCKET sc, ULONG addr, USHORT port) :
    TCPSocketedEntity(sc, addr, port)
{
    #ifdef LOGGER
    {
        InRed("Created sender");
    }
    #endif LOGGER
    
}
Sender::Sender(Int scType, Int ptc, ULONG addr, USHORT port):
    Sender(socket(AF_INET, scType, ptc), addr, port)
{}
Sender::Sender(ULONG addr, USHORT port):
    Sender(socket(AF_INET, SOCK_STREAM, 0), addr, port)
{}
Sender::Sender(String addr, USHORT port):
    Sender(inet_addr(addr.c_str()), port)
{}

Int Sender::ConnectToUser()
{
    Int result = connect(sc, addrPtr, addrSize);

    #ifdef LOGGER
    {
        IF (result == SOCKET_ERROR)
            InRedWithError("failed to connect, error: ");
        ELSE
            InRed("socket connected alright");
    }
    #endif LOGGER

    return result;
}
Int Sender::SendMessageToUser(const String & buff)
{
    Int result = send(sc, buff.c_str(), buff.length(), NULL);
    
    #ifdef LOGGER
    {
        IF (result < 0)
            InRedWithError("send failed somehow, error: ");
        ELIF(result == 0)
            InRedWithError("nothing set, error: ");
        ELSE
            InRed("message probably sent");
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


#ifndef MESSENGER_COMPONENT

MessengerComponent::MessengerComponent():
    listener(messPort)
{
    #ifdef LOGGER
    {
        InRed("MSG component constructing");
    }
    #endif
    listener.Bind();
    listener.Listen();
}
Int MessengerComponent::SendMessageTo(const String& msg, const UserData& usr)
{
    String userAddr = usr.Address().to_str();
    USHORT port = usr.Address().port;

    Sender sender(userAddr, port);

    Int result = sender.ConnectToUser();

    if (result != SOCKET_ERROR) {
        #ifdef LOGGER
        {
            InRedWithError("MSG component Send failed to connect, error: ");
        }
        #endif LOGGER

        result = sender.SendMessageToUser(msg);
        
        #ifdef LOGGER
        {
            IF(result == SOCKET_ERROR)
                InRedWithError("MSG component Send failed to send, error: ");
            ELSE
                InRed("MSG component Send succeeded in sending");
        }
        #endif LOGGER
    }
    return result;
}
Int MessengerComponent::SendMessageTo(const String& msg, const UserVector& users)
{
    int sum = 0;
    for (auto& user : users)
        sum+= SendMessageTo(msg, user);
    return sum;
}
Int MessengerComponent::ReceiveMessage()
{
    TCPSocketedEntity se = listener.Accept();
   
    Int result = se.sc;

    if (result == INVALID_SOCKET) {
        #ifdef LOGGER
        {
            InRedWithError("MSG component Receive: Accept fail, error: ");
        }
        #endif LOGGER
    }
    else{
        char buff[100]{};
        result = recv(se.sc, buff, sizeof(buff), NULL);
        if (result == SOCKET_ERROR){
            #ifdef LOGGER
            {
                InRedWithError("MSG component Receive: recv fail, error: ");
            }
            #endif LOGGER
        }
        else {
            TimePoint now = Clock::now();
            char str[100]{};
            inet_ntop(AF_INET, se.addrPtr, str, sizeof(str));

            #ifdef LOGGER
            {
                time_t tt = Clock::to_time_t(now);
                String time = String(std::ctime(&tt));

                String log = "Rcvd msg: " +  time + " " + str + " " + buff;
                InRed(log);
            }
            #endif LOGGER

            UserData ud = UDFComponent().FindUsersInFile(UserData::UserAddr::UserAddr(str));
            Message msg = std::make_tuple(now, ud, buff);
            yetUnread.push_back(msg);
        }
    }
    return result;
}
MessageVector& MessengerComponent::MsgAlrdyRead()
{
    return alrdyRead;  
}
MessageVector& MessengerComponent::MsgYetUnread()
{
    return yetUnread;
}

Int MessengerComponent::MessageReadCount()
{
    return alrdyRead.size();
}

Int MessengerComponent::MessageUnreadCount()
{
    return yetUnread.size();
}

Int MessengerComponent::MarkAllAsRead()
{
    Int elemCount = yetUnread.size();

    auto reEnd = std::end(alrdyRead);
    auto unBeg = std::begin(yetUnread);
    auto unEnd = std::end(yetUnread);

    alrdyRead.insert(reEnd, unBeg, unEnd);
    yetUnread.clear();

    return elemCount;
}
#endif !MESSENGER_COMPONENT