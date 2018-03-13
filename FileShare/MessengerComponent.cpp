#include "MessengerComponent.h"

using namespace FileShare;

#ifndef TCP_SOCKETED_ENTITY
TCPSocketedEntity::TCPSocketedEntity(SOCKET s, ULONG inetAddr, USHORT port):
    sc(s),
    addr{}
{
    addr.sin_family = AF_INET;
    addr.sin_addr.S_un.S_addr = inetAddr;
    addr.sin_port = htons(port);

    addrPtr = reinterpret_cast<SOCKADDR*>(&addr);
    addrSize = sizeof(addr);
}
TCPSocketedEntity::TCPSocketedEntity(SOCKET sc, String addr, USHORT port):
    TCPSocketedEntity(sc, inet_addr(addr.c_str()), port)
{}
TCPSocketedEntity::TCPSocketedEntity():
    TCPSocketedEntity(INVALID_SOCKET, NULL, NULL)
{}
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
{}
Int Listener::Bind()
{
    return bind(sc, addrPtr, addrSize);
}
Int Listener::Listen()
{
    return listen(sc, 1);
}
TCPSocketedEntity Listener::Accept()
{
    TCPSocketedEntity se;
    se.sc = accept(sc, se.addrPtr, &se.addrSize);
    return se;
}
#endif !TCP_LISTENER

#ifndef TCP_SENDER
Sender::Sender(const UserData & usr):
    TCPSocketedEntity(UserDataToSocketedentity(usr))
{}

Int Sender::ConnectToUser()
{
    return connect(sc, addrPtr, addrSize);
}

Int Sender::SendMessageToUser(const String & buff)
{
    return send(sc, buff.c_str(), buff.length(), NULL);

}

TCPSocketedEntity Sender::UserDataToSocketedentity(const UserData& usr)
{
    SOCKET sc = socket(AF_INET, SOCK_STREAM, NULL);
    ULONG inetAddr = inet_addr(usr.Address().to_str().c_str());
    USHORT port = usr.Address().port;
    return TCPSocketedEntity(sc, inetAddr, port);
}
#endif TCP_SENDER

#endif !TCP_SOCKETED_ENTITY


#ifndef MESSENGER_COMPONENT
MessengerComponent::MessengerComponent():
    listener(messPort)
{}
Int MessengerComponent::SendMessageTo(const String& msg, const UserData& usr)
{
    Sender sender(usr);
    if (sender.ConnectToUser() != SOCKET_ERROR)
        return sender.SendMessageToUser(msg);
    else
        return SOCKET_ERROR;
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
    int result;
    if (se.sc != INVALID_SOCKET) {
        char buff[100]{};
        if ((result = recv(se.sc, buff, sizeof(buff), NULL)) == SOCKET_ERROR)
        {
        }
        else {
            TimePoint now = Clock::now();
            char str[100]{};
            inet_ntop(AF_INET, se.addrPtr, str, sizeof(str));

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
