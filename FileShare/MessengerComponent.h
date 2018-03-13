#pragma once

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

    using Message = std::tuple<TimePoint, UserData, String>;
    using MessageVector = std::vector<Message>;

    constexpr Int messPort = 0xdeaf;

    class SenderInterface 
    {
    public:
        virtual Int SendMessageTo(const UserVector&) = 0;
    protected:
    };

    class ReceiverInterface {
    public:
        virtual Int ReceiveMessage() = 0;

        virtual MessageVector& MsgAlrdyRead() = 0;
        virtual MessageVector& MsgYetUnread() = 0;
        virtual MessageVector& MsgAllSorted() = 0;
    protected:
        virtual void MsgSort() = 0;
    };
    struct ReceiverData {
        MessageVector alrdyRead;
        MessageVector yetUnread;
    };

    class MessengerComponent:
        public SenderInterface,
        public ReceiverInterface,
        public ReceiverData
    {
        
    };
}
