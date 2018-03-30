#pragma once
#include <winsock2.h>
#include <string>
#include <vector>
#include <utility>

namespace FileShare 
{
    using STR = std::string;
    using Aura = std::pair<ULONG, USHORT>;
    using AuraSet = std::vector<Aura>;
    using Int = int;

    #define preComPort (USHORT)14088

    struct SockAddrIn 
    {
        SockAddrIn();
        SockAddrIn(USHORT port);

        sockaddr_in addr;
        sockaddr* addrPtr;
        Int addrSize;
    };

    class BasicPresenceComponentInterface 
    {
    protected:
        virtual Int WSAstartup()            = 0;
        virtual Int InitializeSockaddr()    = 0;
        virtual Int CreateSocket()          = 0;
        virtual Int CloseSocket()           = 0;
        virtual Int SetSocketOptions()      = 0;
    };

    class ReceiverPresenceComponentInterface:
        virtual protected BasicPresenceComponentInterface
    {
    protected:
        virtual Int BindSocket() = 0;
        virtual STR ReceiveBroadcastedMessage() = 0;
    };

    class SenderBasicPresenceComponentInterface :
        virtual protected BasicPresenceComponentInterface
    {
    protected:
        virtual Int SendMessageBroadcast(const STR&) = 0;
    };

    struct BasicSocketedEntity {
        BasicSocketedEntity() = default;
        BasicSocketedEntity(USHORT port);

        WSADATA wsa;
        SOCKET sc;
        Int scOptions = 1;

        SockAddrIn inAddrAny; // inaddr_any - bind
        SockAddrIn inAddrBro; // inaddr_broadcast send
        SockAddrIn inAddrSdr{}; // inaddr_sender receive from
    };

    class PresenceComponent :
        public BasicSocketedEntity,
        virtual protected ReceiverPresenceComponentInterface,
        virtual protected SenderBasicPresenceComponentInterface
    { 
    public:
        PresenceComponent();
        PresenceComponent(USHORT presenceComponentPort);
        ~PresenceComponent();

        virtual STR ReceiveBroadcastedMessage()      override;
        virtual Int SendMessageBroadcast(const STR&) override;

        virtual STR GetHostIp();

        Int GetAuraCount();
        AuraSet activeLocalBroadcasters;
    protected:
        virtual Int WSAstartup()           override;
        virtual Int InitializeSockaddr()   override;
        virtual Int CreateSocket()         override;
        virtual Int SetSocketOptions()     override;
        virtual Int BindSocket()           override;
        virtual Int CloseSocket()          override;
    };
}