#include "MessengerComponent.h"

#include <ctime>
#include <chrono>

#define DontLog
#include "Logger.h"

using namespace FileShare;

#ifndef MESSENGER_COMPONENT

#ifndef MESSENGER_COMPONENT_CONSTRUCTORS
MessengerComponent::MessengerComponent(USHORT port) :
    listener(port)
{
    Log::InRed("MSG component constructing");

    listener.Bind();
    listener.Listen();
}
MessengerComponent::MessengerComponent() :
    MessengerComponent(messPort)
{}
#endif MESSENGER_COMPONENT_CONSTRUCTORS
Int SendMessageViaSender(const String msg, Sender&& sender) 
{
    Int result = SOCKET_ERROR;
    std::chrono::duration<LONG> timeOut = std::chrono::seconds(5);
    Log::InRed("gonna try to connect for 5s");

    for (TimePoint then = Clock::now(); Clock::now() - then < timeOut;) 
    {
        result = sender.ConnectToUser();
        if (result != SOCKET_ERROR)
            break;
    }

    if (result == SOCKET_ERROR) 
    {
        Log::InRedWithError("MSG component Send failed, connect timeout, error: ");
        return result;
    }
    else
    {
        Log::InRed("gonna try to send for 5s");

        for (TimePoint then = Clock::now(); Clock::now() - then < timeOut;) {
            result = sender.SendMessageToUser(msg);
            if (result > 0)
                break;
        }

        if (result <= NULL)
            Log::InRedWithError("MSG component Send failed to send, error: ");
        else
            Log::InRed("MSG component Send succeeded in sending");
    }
    return result;
}

Int MessengerComponent::SendMessageTo(const String& msg, ULONG adr, USHORT port)
{
    Int result = SOCKET_ERROR;
    Log::InRed("SendMessageTo()->");
    __Begin;
    {
        Sender sender(adr, port);

        if (sender.InvalidSocket())
        {
            Log::InRed("sender wasnt created properly");
            result = sender.sc;
        }
        else
        {
            Log::InRed("sender created? trying to send");
            result = SendMessageViaSender(msg, std::move(sender));

            if (result != SOCKET_ERROR)
            {
                msgs.emplace_back(Clock::now(), adr^(~0), msg);
            }

        }
    }
    __End;
    Log::InRed("SendMessageTo();");
    return result;
}
Int MessengerComponent::SendMessageTo(const String& msg, const String& adr, USHORT port)
{
    return SendMessageTo(msg, inet_addr(adr.c_str()), port);
}

Int MessengerComponent::SendMessageTo(const String& msg, const UserData& usr)
{
    if (usr == UserData::BadData)
        return 0;

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
    Int result = 0;
    Log::InRed("ReceiveMessage()->");
    __Begin;
    {
        Receiver rc = listener.Accept();
        Log::InRed("Listener.accept() +");

        String buff = rc.ReceiveMessage();
        Log::InRed("Receiver tried hard");

        if (buff.empty())
        {
            Log::InRed("but we got nothing");
        }
        else
        {
            Log::InRed("and we got smth");
            result = buff.length();

            TimePoint now = Clock::now();
            ULONG addr = rc.addr.sin_addr.S_un.S_addr;
            Message msg = std::make_tuple(now, addr, buff);

            msgs.push_back(msg);
            ++unreadCount;
        }
    }
    __End;
    Log::InRed("ReceiveMessage();");
    return result;
}

MessageVector& MessengerComponent::Messages()
{
    return msgs;  
}

Int MessengerComponent::MessageReadCount()
{
    return msgs.size() - unreadCount;
}

Int MessengerComponent::MessageUnreadCount()
{
    return unreadCount;
}

Int MessengerComponent::MarkAllAsRead()
{
    Int result = unreadCount;

    unreadCount = 0;

    return result;
}
#endif !MESSENGER_COMPONENT