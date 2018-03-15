#include "Logger.h"

#ifdef _DEBUG
#define TEST main

// tests
#define TEST_SHARED_FOLDER_NAVIGATOR_SELF   1
#define TEST_USER_DATA_FILE_COMPONENT       2
#define TEST_PRESENCE_COMPONENT             3
#define TEST_MESSENGER                      4

#if !defined(CURRENT_TEST)
#define CURRENT_TEST TEST_MESSENGER
#endif !defined(CURRENT_TEST)

#if defined TEST_MESSENGER
#if defined CURRENT_TEST
#if CURRENT_TEST == TEST_MESSENGER

#include <thread>
#include <ctime>
#include <mutex>
#include <sstream>
#include <regex>
#include "MessengerComponent.h"

using namespace FileShare;

void WsaStartup() {
    static WSADATA wsaData;
    int error = WSAStartup(MAKEWORD(2, 2), &wsaData);

    #ifdef LOGGER
    {
        IF(error)
            InRedWithError("wsastartup failed with error: ");
        ELSE
            InRed("wsastartup");
    }
    #endif
}

String GetHostIp() {
    char hostName[1024]{};
    IF(gethostname(hostName, sizeof(hostName)) == SOCKET_ERROR)
        InRedWithError("Your host is nameless somehow");
    //ELSE
    //    InRed("Host name: " + String(hostName));

    HOSTENT* hostEnt = gethostbyname(hostName);
    IN_ADDR** addrList = (IN_ADDR**)hostEnt->h_addr_list;
    char* hostAddr = inet_ntoa(*addrList[0]);
    //InRed("Host addr: " + String(hostAddr));
    
    return hostAddr;
}

#ifndef TEST_SOCKETED_ENTITIES
void TestSendingViaTcpSender() {

    USHORT port;
    std::cin >> port;

    for (int i = 0; i < 3; ++i) {
        Sender sender(inet_addr(GetHostIp().c_str()), port);

        while (sender.ConnectToUser() == SOCKET_ERROR)
            for (int i = 0; i < 100000; ++i);


        while (sender.SendMessageToUser("hello") == SOCKET_ERROR)
            for (int i = 0; i < 100000; ++i);
    }
}
void TestListenerAndReceiver() {
    USHORT port;
    std::cin >> port;
    Listener listener(port);

    listener.Bind();
    listener.Listen();

    while (true) {
        InRed("About to enter accepting loop");
        Receiver se = listener.Accept();

        String buff = se.ReceiveMessage();

        if (buff.empty())
            continue;
        else
            InWhite(buff);
        for (int i = 0; i < 100000; ++i);
    }
}
void TestListenerSenderAndReceiver() {
    USHORT listenerPort;
    std::cin >> listenerPort;

    Listener listener(listenerPort);
    listener.Bind();
    listener.Listen();

    std::thread th_send([&]() {
        while (true) {

            USHORT port;
            String msg;
            std::cin >> port;
            std::cin.ignore();
            std::getline(std::cin, msg);

            Sender sender(GetHostIp(), port);
            while (sender.ConnectToUser() == SOCKET_ERROR);
            while (sender.SendMessageToUser(msg) == SOCKET_ERROR);
        }
    });

    std::thread th_accept_and_receive([&]() {
        while (true) {
            Receiver receiver = listener.Accept();
            String message = receiver.ReceiveMessage();
            if (message.empty())
                continue;
            else
                InWhite(message);
            for (int i = 0; i < 100000; ++i);
        }
    });
    
    th_send.join();
}
#endif

void TestMessengerSomehow() {
    USHORT listenerPort;
    InRed("Test: chose your port - ");
    std::cin >> listenerPort;

    MessengerComponent mc(listenerPort);

    std::mutex mx;
 
    std::thread th_receive([&]() {
        InRed("");
        InRed("recv_thread started");

        while (true) {

            InRed("Test: ready to recv");
            mc.ReceiveMessage();
           
            mx.lock();

            if (!mc.MsgYetUnread().empty()) {
                InRed("Test: some msg in");
                for (Message& msg : mc.MsgYetUnread()) {

                    time_t tt = Clock::to_time_t(std::get<0>(msg));
                    String time = String(std::ctime(&tt));

                    /*[](String& s) {
                        String result;
                        std::regex rx[] = {
                            std::regex(R"(\d+:.+:\d+)"),
                            std::regex(R"(\b[^ ][A-Za-z]+\b)"),
                            std::regex(R"((?<!:)\d\d(?= )\b)")
                        };

                        for (int i = 0; i < 3; ++i) {
                            std::smatch match;
                            std::regex_search(s, match, rx[i]);
                            result += String(match[i % 2]) + String(i < 2 ? ":" : "");
                        }
                        s = result;
                    }(time);*/

                    IN_ADDR addr{};
                    addr.S_un.S_addr = std::get<1>(msg);
                    String ip = inet_ntoa(addr);

                    String txt = std::get<2>(msg);

                    InWhite(time + "[" + ip + "]: " + txt);
                }
            }
            mx.unlock();
        }
    });
    std::thread th_send([&]() {
        InRed("");
        InRed("send_thread started");

        while (true) {
            mx.lock();

            InRed("");
            InRed("Test: chose port to send to - ");
            USHORT port;
            std::cin >> port;
            
            InRed("Test: msg - ");
            String msg;
            std::cin >> msg;
            //std::getline(std::cin, msg);
            //std::cin.ignore();

            InRed("Test: sending...");
            IF (mc.SendMessageTo(msg, GetHostIp(), port) > 0)
                InRed("Test: ...msg sent");
            ELSE
                InRed("Test: ...msg not sent");

            mx.unlock();
            for (int i = 0; i < 100000; ++i);
        }
    });

    while (true);
}

int TEST() {
    WsaStartup();

    //TestSendingViaTcpSender();

    //TestListenerAndReceiver();

    //TestListenerSenderAndReceiver();

    TestMessengerSomehow();

    return system("pause");
}

#endif CURRENT_TEST == TEST_MESSENGER
#endif CURRENT_TEST
#endif TEST_MESSENGER

#if defined TEST_PRESENCE_COMPONENT
#if defined CURRENT_TEST
#if CURRENT_TEST == TEST_PRESENCE_COMPONENT
#include <thread>
#include <chrono>
#include "PresenceComponent.h"
using namespace FileShare;
using Thread = std::thread;

volatile bool mutex = false;

int TEST() {
    InRed(TO_STR(TEST_PRESENCE_COMPONENT));

    PresenceComponent preCom;

    Thread th_in([&](){
        for (int i = 0; i < 10; ++i) {
            String msg = preCom.ReceiveBroadcastedMessage();
            while (mutex);
            InWhite(msg);
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    });

    Thread th_out([&]() {
        String str;
        std::cin >> str;
        for (int i = 0; i < 10; ++i) {
            mutex = true;
            preCom.SendMessageBroadcast(str);
            mutex = false;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
    
    th_in.join();
    th_out.join();

    return system("pause");
}
#endif CURRENT_TEST == TEST_PRESENCE_COMPONENT
#endif CURRENT_TEST
#endif TEST_PRESENCE_COMPONENT

#if defined TEST_USER_DATA_FILE_COMPONENT
#if defined CURRENT_TEST
#if CURRENT_TEST == TEST_USER_DATA_FILE_COMPONENT

#include <Windows.h>
#include "UserDataFileComponent.h"
using namespace FileShare;   
int TEST() {
    InRed(TO_STR(TEST_USER_DATA_FILE_COMPONENT));

    UDFComponent udf;

    for (UserData& ud : UserVector{
        UserData{"Boi",                 UserData::UserAddr("Next Door"),        UserData::UserStatus::StatusValue::Good},
        UserData{"Billy",               UserData::UserAddr("Ass We Can"),       UserData::UserStatus::StatusValue::Bad},
        UserData{"LeatherMan",          UserData::UserAddr("Leather Men Club"), UserData::UserStatus::StatusValue::Good},
        UserData{"PerformanceArtist",   UserData::UserAddr("Dungeon"),          UserData::UserStatus::StatusValue::Ugly}
    })
        udf.AppendUser(ud);

    InRed("UDF with some users: ");
    for (auto& ud : udf.FindUsersInFile())
        std::cout << ud.Alias() << " " << ud.Address().to_str() << " " << ud.Status().to_str() << std::endl;

    InRed("try to fing good users");
    for (auto& ud : udf.FindUsersInFile(UserData::UserStatus::StatusValue::Good))
        std::cout << ud.Alias() << " " << ud.Address().to_str() << " " << ud.Status().to_str() << std::endl;

    UserData ud = udf.FindUsersInFile("Boi");
    udf.ModifyUser(ud, { "ULIKE", UserData::UserAddr("THE POWAH, HUH"), UserData::UserStatus::StatusValue::Ugly});

    InRed("Tried modifying user");
    for (auto& ud : udf.FindUsersInFile())
        std::cout << ud.Alias() << " " << ud.Address().to_str() << " " << ud.Status().to_str() << std::endl;

    system("pause");

    for (auto& ud : udf.FindUsersInFile())
        udf.RemoveUser(ud);
    return 0;
}
#endif // CURRENT_TEST == TEST_USER_DATA_FILE_COMPONENT
#endif // CURRENT_TEST
#endif TEST_USER_DATA_FILE_COMPONENT

#if defined TEST_SHARED_FOLDER_NAVIGATOR_SELF 
#if defined CURRENT_TEST
#if CURRENT_TEST == TEST_SHARED_FOLDER_NAVIGATOR_SELF
#include <Windows.h>
#include "SharedFolderNavigatorComponent.h"

using namespace FileShare;
int TEST() {
    std::cout << TO_STR(EST_SHARED_FOLDER_NAVIGATOR_SELF) << std::endl;

    //for (int i = 0; i < 256; ++i) {
    //    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), i);
    //    std::cout << "i = " << i << std::endl;
    //}


    std::cout << "hello, ";
    InRed("world\n");
    
    SharedFolderNavigatorSelf self;
    InRed("instance created");

    String paintmeName = self.FileCreate("paintme.bmp");
    if (paintmeName != "paintme.bmp") {
        InRed("paintme.bmp already existed.");
        InRed("new paintme is created as " + paintmeName);
        InRed("gonna rename " + paintmeName + " somehow");
        self.FileRename(paintmeName, "new_paintme_name_with_blackjack_and_hookers.bmp");
    }
    else
        InRed("paintme.bmp created");
    {
        FileVector fv = self.GetFileList();
        if (fv.empty())
            InRed("vector empty, smth wrong");

        for (const String& fileName : fv)
            std::cout << fileName << std::endl;
    }

    String fileName;
    std::cout << "file name pls" << std::endl;
    std::cin >> fileName;

    InRed("opening " + fileName);
    self.FileOpen(fileName);

    InRed("opened file s gonna be deleted afterwards");
    system("pause");

    self.FileDelete(fileName);

    {
        FileVector fv = self.GetFileList();
        for (const String& fileName : fv)
            std::cout << fileName << std::endl;
    }   
    return system("pause");
}
#endif // CURRENT_TEST == TEST_SHARED_FOLDER_NAVIGATOR_SELF
#endif // defined CURRENT_TEST
#endif defined TEST_SHARED_FOLDER_NAVIGATOR_SELF
#endif _DEBUG