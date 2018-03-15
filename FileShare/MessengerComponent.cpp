#include "MessengerComponent.h"
#include "Logger.h"
#include <ctime>
#include <thread>
#include <chrono>

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
    Int result;

    #ifdef LOGGER
    {
        InRed("Receiver trying to recv");
    }
    #endif

    char buff[128]{};

    result = recv(sc, buff, sizeof(buff), NULL);
    
    #ifdef LOGGER
    {
        IF(result == SOCKET_ERROR)
            InRedWithError("Receive failed, error: ");
        ELIF(result == NULL)
            InRed("connection ??gracefully?? closed");
        ELSE
            InRed("received smth");
    }
    #endif LOGGGER

    return String(buff);
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
            InRedWithError("nothing sent, error: ");
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

#ifndef MESSENGER_COMPONENT_CONSTRUCTORS
MessengerComponent::MessengerComponent(USHORT port):
    listener(port)
{
    #ifdef LOGGER
    {
        InRed("MSG component constructing");
    }
    #endif
    listener.Bind();
    listener.Listen();
}
MessengerComponent::MessengerComponent() :
    MessengerComponent(messPort)
{}
#endif MESSENGER_COMPONENT_CONSTRUCTORS

Int SendMessageViaSender(const String msg, Sender&& sender) {

    Int result = SOCKET_ERROR;

    std::chrono::duration<LONG> timeOut = std::chrono::seconds(5);

    #ifdef LOGGER
    {
        InRed("gonna try to connect for 5s");
    }
    #endif LOGGER

    for (TimePoint then = Clock::now(); Clock::now() - then < timeOut;) {
        result = sender.ConnectToUser();
        if (result != SOCKET_ERROR)
            break;
    }

    if (result == SOCKET_ERROR) {
        #ifdef LOGGER
        {
            InRedWithError("MSG component Send failed, connect timeout, error: ");
        }
        #endif LOGGER

        return result;
    }
    else{

        #ifdef LOGGER
        {
            InRed("gonna try to send for 5s");
        }
        #endif LOGGER

        for (TimePoint then = Clock::now(); Clock::now() - then < timeOut;) {
            result = sender.SendMessageToUser(msg);
            if (result > 0)
                break;
        }

        #ifdef LOGGER
        {
            IF(result <= NULL)
                InRedWithError("MSG component Send failed to send, error: ");
            ELSE
                InRed("MSG component Send succeeded in sending");
        }
        #endif LOGGER
    }
    return result;
}

Int MessengerComponent::SendMessageTo(const String& msg, ULONG adr, USHORT port)
{
    Int result = SOCKET_ERROR;

    #ifdef LOGGER
    {
        InRed("entering SendMessage, creating sender");
    }
    #endif LOGGER

    Sender sender(adr, port);

    if (sender.InvalidSocket()) 
    {
        #ifdef LOGGER
        {
            InRed("sender wasnt created properly");
        }
        #endif LOGGER

        result = sender.sc;

    }
    else 
    {
        #ifdef LOGGER
        {
            InRed("sender created? trying to send");
        }
        #endif LOGGER

        result = SendMessageViaSender(msg, std::move(sender));
    }
    
    return result;
}
Int MessengerComponent::SendMessageTo(const String& msg, const String& adr, USHORT port)
{
    return SendMessageTo(msg, inet_addr(adr.c_str()), port);
}

Int MessengerComponent::SendMessageTo(const String& msg, const UserData& usr)
{
    String userAddr = usr.Address().to_str();
    USHORT port = usr.Address().port;

    return SendMessageTo(msg, userAddr, port);
}
Int MessengerComponent::SendMessageTo(const String& msg, const UserVector& users)
{
    int sum = 0;

    for (auto& user : users)
        sum+= SendMessageTo(msg, user);

    return sum + users.size(); 
    // since SendTo returns SOCKET_ERROR == -1 on fail
    // return is userCount - failCount == successCount
    // 0 is total fail
}
Int MessengerComponent::ReceiveMessage()
{
    #ifdef LOGGER
    {
        InRed("entering MSG_C RecMes");
    }
    #endif

    Int result;

    Receiver rc = listener.Accept();

    #ifdef LOGGER
    {
        InRed("listener accepter receiver");
    }
    #endif

    String buff = rc.ReceiveMessage();

    #ifdef LOGGER
    {
        InRed("Receiver tried hard");
    }
    #endif

    if (buff.empty()) {
        #ifdef LOGGER
        {
            InRed("but we got nothing");
        }
        #endif

        return 0;
    }
    else
    {
        #ifdef LOGGER
        {
            InRed("and we got smth");
        }
        #endif

        result = buff.length();

        TimePoint now = Clock::now();
        ULONG addr = rc.addr.sin_addr.S_un.S_addr;

        Message msg = std::make_tuple(now, addr, buff);

        yetUnread.push_back(msg);
    }
    
    return result;

    /*TCPSocketedEntity se = listener.Accept();

    if (se.InvalidSocket()) {
        result = se.sc;

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
    return result;*/
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