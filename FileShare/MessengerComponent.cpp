#include "MessengerComponent.h"
#include "Logger.h"
#include <ctime>
#include <chrono>

#include "Logger.h"

using namespace FileShare;
using namespace Log;

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

    {
        #ifdef LOGGER
        {
            InRed("gonna try to connect for 5s");
        }
        #endif LOGGER
    }

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
    #ifdef LOGGER
    {
        InRed("entering MSG_C RecMes");
    }
    #endif

    Int result = 0;

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
        //ULONG addr = rc.addr.sin_addr.S_un.S_addr; 
        // bad addr
        // need to use sockname smth

        SOCKADDR_IN sin{};
        int sinSize = sizeof(sin);
        getsockname(rc.sc, (SOCKADDR*)&sin, &sinSize);
        //std::cout << (int)ntohs(sin.sin_port) << " ";
        ULONG addr = sin.sin_addr.S_un.S_addr;

        Message msg = std::make_tuple(now, addr, buff);

        msgs.push_back(msg);
        ++unreadCount;
    }
    
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