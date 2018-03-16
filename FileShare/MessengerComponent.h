#pragma once
#include "TcpSocketedEntity.h"
#include "UserDataFileComponent.h"

namespace FileShare {

    using Clock = std::chrono::system_clock;
    using TimePoint = Clock::time_point;
    using Message = std::tuple<TimePoint, ULONG, String>;
    using MessageVector = std::vector<Message>;

#ifndef MESSENGER_COMPONENT
    
    class SenderInterface 
    {
    public:
        virtual Int SendMessageTo(const String&, const String&, USHORT) = 0;
        virtual Int SendMessageTo(const String&, ULONG, USHORT) = 0;

        virtual Int SendMessageTo(const String&, const UserData&)   = 0;
        virtual Int SendMessageTo(const String&, const UserVector&) = 0;
    };
    class ReceiverInterface {
    public:
        virtual Int ReceiveMessage() = 0;

        virtual MessageVector& Messages() = 0;

        virtual Int MessageReadCount()      = 0;
        virtual Int MessageUnreadCount()    = 0;
        virtual Int MarkAllAsRead()         = 0;
    };

    struct ReceiverData {
        int unreadCount = 0;
        MessageVector msgs;
    };

    class MessengerComponent:
        public SenderInterface,
        public ReceiverInterface,
        protected ReceiverData
    {
    public:
        MessengerComponent();
        MessengerComponent(USHORT port);

        virtual Int SendMessageTo(const String&, const String&, USHORT) override;
        virtual Int SendMessageTo(const String&, ULONG, USHORT) override;

        virtual Int SendMessageTo(const String&, const UserData&)   override;
        virtual Int SendMessageTo(const String&, const UserVector&) override;

        virtual Int ReceiveMessage() override;

        virtual MessageVector& Messages() override;

        virtual Int MessageReadCount()      override;
        virtual Int MessageUnreadCount()    override;

        virtual Int MarkAllAsRead()    override;

        Listener listener;
    };
}
#endif MESSENGER_COMPONENT
