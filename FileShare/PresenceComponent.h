#pragma once
#include <winsock2.h>
#include <string>
#include <tuple>
#include <set>
#include <utility>

/*
    Presence component is gonna spam local area with <notice-me> msgs
    <notice-me> msg contains
        usr name
        messenger sockaddr
        file-share-request sockaddr
*/

namespace FileShare {
    using String = std::string;
    using Int = int;
    constexpr int preComPort = 14088;

    struct SockAddrIn {
        SockAddrIn();

        sockaddr_in addr;
        sockaddr* addrPtr;
        Int addrSize;
    };

    class BasicPresenceComponentInterface {
    protected:
        virtual Int WSAstartup() = 0;
        virtual Int InitializeSockaddr() = 0;
        virtual Int CreateSocket() = 0;
        virtual Int CloseSocket() = 0;
        virtual Int SetSocketOptions() = 0;
    };

    class ReceiverPresenceComponentInterface:
        virtual protected BasicPresenceComponentInterface
    {
    protected:
        virtual Int BindSocket() = 0;
        virtual String ReceiveBroadcastedMessage() = 0;
    };

    class SenderBasicPresenceComponentInterface :
        virtual protected BasicPresenceComponentInterface
    {
    protected:
        virtual Int SendMessageBroadcast(const String&) = 0;
    };

    struct BasicSocketedEntity {
        WSADATA wsa;
        SOCKET sc;
        Int scOptions = 1;

        SockAddrIn inAddrAny{}; // inaddr_any - bind
        SockAddrIn inAddrBro{}; // inaddr_broadcast send
        SockAddrIn inAddrSdr{}; // inaddr_sender receive from
    };
    class PresenceComponent :
        protected BasicSocketedEntity,
        virtual protected ReceiverPresenceComponentInterface,
        virtual protected SenderBasicPresenceComponentInterface
    { 
    public:
        PresenceComponent();
        ~PresenceComponent();

        virtual String ReceiveBroadcastedMessage()         override;
        virtual Int   SendMessageBroadcast(const String&)  override;

    protected:
        virtual Int WSAstartup()           override;
        virtual Int InitializeSockaddr()   override;
        virtual Int CreateSocket()         override;
        virtual Int SetSocketOptions()     override;
        virtual Int BindSocket()           override;
        virtual Int CloseSocket()          override;
    };
}