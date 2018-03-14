#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS 

//#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#include <vector>
#include <tuple>
#include <string>
#include <chrono>

#include "UserDataFileComponent.h"

namespace FileShare {

    using String = std::string;
    using Int = int;
    using Bool = int;

    constexpr USHORT messPort = 54321u;

#ifndef TCP_SOCKETED_ENTITY
    class TCPSocketedEntity 
    {
    public:
        TCPSocketedEntity();
        ~TCPSocketedEntity();

        TCPSocketedEntity(SOCKET sc, ULONG addr, USHORT port);
        TCPSocketedEntity(SOCKET sc, String addr, USHORT port);
        TCPSocketedEntity(Int scType, Int protocol, ULONG inetAddr, USHORT port);

        TCPSocketedEntity(TCPSocketedEntity&& other);
        TCPSocketedEntity& operator = (TCPSocketedEntity&& other);

        Bool CompareSockaddr(const TCPSocketedEntity& other) const;

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
    class Sender:
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
        //static TCPSocketedEntity UserDataToSocketedentity(const UserData&);
    };
#endif TCP_SENDER

#ifndef TCP_RECEIVER
    class Receiver :
        public TCPSocketedEntity
    {
    public:
        Receiver(TCPSocketedEntity&&);

        String ReceiveMessage();
    };
#endif TCP_RECEIVER

#endif TCP_SOCKETED_ENTITY

#ifndef MESSENGER_COMPONENT
    using Clock = std::chrono::system_clock;
    using TimePoint = Clock::time_point;
    using Message = std::tuple<TimePoint, UserData, String>;
    using MessageVector = std::vector<Message>;

    class SenderInterface 
    {
    public:
        virtual Int SendMessageTo(const String&, const UserData&)   = 0;
        virtual Int SendMessageTo(const String&, const UserVector&) = 0;
    };

    class ReceiverInterface {
    public:
        virtual Int ReceiveMessage() = 0;

        virtual MessageVector& MsgAlrdyRead() = 0;
        virtual MessageVector& MsgYetUnread() = 0;

        virtual Int MessageReadCount()      = 0;
        virtual Int MessageUnreadCount()    = 0;
        virtual Int MarkAllAsRead()         = 0;


    //protected:
    //    virtual void MsgSort() = 0;
    };
    struct ReceiverData {
        MessageVector alrdyRead{};
        MessageVector yetUnread{};
    };

    class MessengerComponent:
        public SenderInterface,
        public ReceiverInterface,
        protected ReceiverData
    {
    public:
        MessengerComponent();
        virtual Int SendMessageTo(const String&, const UserData&)   override;
        virtual Int SendMessageTo(const String&, const UserVector&) override;
        virtual Int ReceiveMessage()                 override;

        virtual MessageVector& MsgAlrdyRead() override;
        virtual MessageVector& MsgYetUnread() override;

        virtual Int MessageReadCount()      override;
        virtual Int MessageUnreadCount()    override;

        virtual Int MarkAllAsRead()    override;

    //protected:                                     
    //    virtual void MsgSort() override;

        Listener listener;
    };
}
#endif MESSENGER_COMPONENT
