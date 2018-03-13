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
    using Clock = std::chrono::steady_clock;
    using TimePoint = std::chrono::steady_clock::time_point;
    using Int = int;
    using Bool = int;

    using Message = std::tuple<TimePoint, UserData, String>;
    using MessageVector = std::vector<Message>;

    constexpr USHORT messPort = 54321u;

#ifndef TCP_SOCKETED_ENTITY
    class TCPSocketedEntity 
    {
    public:
        TCPSocketedEntity();
        TCPSocketedEntity(SOCKET sc, String addr, USHORT port);
        TCPSocketedEntity(SOCKET sc, ULONG addr, USHORT port);
        
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
        Sender(const UserData& usr);
        Int ConnectToUser();
        Int SendMessageToUser(const String& msg);
    protected:
        static TCPSocketedEntity UserDataToSocketedentity(const UserData&);
    };

#endif TCP_SENDER
#endif TCP_SOCKETED_ENTITY

#ifndef MESSENGER_COMPONENT
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
