#include "MessengerComponent.h"
#include "Logger.h"
#include <ctime>
#include <chrono>

#include "Logger.h"
#undef LOGGER

using namespace FileShare;

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