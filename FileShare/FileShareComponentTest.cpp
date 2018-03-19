#include "Logger.h"
#include <iostream>
#include <exception>
#ifdef _DEBUG
#define TEST main

// tests
#define TEST_SHARED_FOLDER_NAVIGATOR_SELF   1
#define TEST_USER_DATA_FILE_COMPONENT       2
#define TEST_PRESENCE_COMPONENT             3
#define TEST_MESSENGER                      4
#define TEST_SHARED_FOLDER_NAVIGATOR_OTHER  5

#if !defined(CURRENT_TEST)
#define CURRENT_TEST 5
#endif // !CURRENT_TEST

#if defined TEST_SHARED_FOLDER_NAVIGATOR_OTHER
#if defined CURRENT_TEST 
#if CURRENT_TEST == TEST_SHARED_FOLDER_NAVIGATOR_OTHER

#include <thread>
#include <chrono>
#include "SharedFolderNavigatorOther.h"

using namespace FileShare;
using Clock = std::chrono::system_clock;


String GetHostIp();
void TrySendingFile();
void TryReceivingFile();

String hostIp = GetHostIp();
String folderPath = R"(C:\Users\oleh.kostiv\Documents\Visual Studio 2015\Projects\FileShare\Debug\)";
String folderFrom = R"(SharedFolder\)";
String folderTo = R"(OtherFolder\)";
String fileName = "writeme.txt";

USHORT filePort = 11111;

int TEST() {
    //*****************************************************
    Log::InRed(TO_STR(TEST_SHARED_FOLDER_NAVIGATOR_OTHER));
    //*****************************************************

    WSADATA wsaData;
    int error = WSAStartup(MAKEWORD(2, 2), &wsaData);


    int sendRecv;
    std::cin >> sendRecv;
    switch (sendRecv) {
        case 1:TrySendingFile(); break;
        case 2:TryReceivingFile(); break;
    }
    return system("pause");
}

String GetHostIp()
{
    WSADATA wsaData;
    int error = WSAStartup(MAKEWORD(2, 2), &wsaData);
    char hostName[1024]{};
    if (gethostname(hostName, sizeof(hostName)) == SOCKET_ERROR)
        return{};
    return inet_ntoa(*((IN_ADDR**)(gethostbyname(hostName))->h_addr_list)[0]);
}

void TrySendingFile()
try {
    ++Log::depth;
    Log::InRed("Try sending file");

    bool keepAtIt = true;
    while (true) {
        ++Log::depth;
        Log::InRed("Creating FS");

        FileSender fs(hostIp, filePort);

        if (fs.InvalidSocket()) {
            Log::InRedWithError("FS invalid");
            continue;
        }

        Int someResult = SOCKET_ERROR;

        for (auto then = Clock::now(); Clock::now() - then < std::chrono::seconds(4); std::this_thread::sleep_for(std::chrono::seconds(1))) {
            someResult = fs.ConnectToUser();
            if (someResult != SOCKET_ERROR)
                break;
        }

        if (someResult == SOCKET_ERROR) {
            Log::InRed("failed to connect");
        }
        else {
            for (auto then = Clock::now(); Clock::now() - then < std::chrono::seconds(4); std::this_thread::sleep_for(std::chrono::seconds(1))) {
                someResult = fs.SendFile(folderPath + folderFrom + fileName, 8);
                if (someResult != SOCKET_ERROR) {
                    break;
                    keepAtIt = false;
                }
            }
            if (someResult == SOCKET_ERROR) {
                Log::InRed("failed to send");
            }
        }
        --Log::depth;
    }
}
catch (std::exception& ex) {
    Log::InRed("tried sending, got: " + String(ex.what()));
}



void TryReceivingFile() {
    ++Log::depth;
    Log::InRed("Try recv file");
    
    std::thread th([&]() {
        Log::InRed("Creating FL");

        FileListener fl(filePort);
        fl.Bind();
        fl.Listen();

        while (true) {
            ++Log::depth;
            FileReceiver fr = fl.Accept();
            if (fr.ReceiveFile(folderPath + folderTo + fileName) != SOCKET_ERROR)
                break;
            --Log::depth;
        }
    });

    th.join();
    --Log::depth;
}

#endif // CURRENT_TEST == TEST_SHARED_FOLDER_NAVIGATOR_OTHER
#endif // CURRENT_TEST
#endif // TEST_SHARED_FOLDER_NAVIGATOR_OTHER


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
using namespace Log;

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
            InWhite(buff.c_str());
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
                InWhite(message.c_str());
            for (int i = 0; i < 100000; ++i);
        }
    });
    
    th_send.join();
}
#endif

volatile bool showMessages = false;

void TestMessengerSomehow() {
    USHORT listenerPort;
    InRed("Test: chose your port - ");
    std::cin >> listenerPort;

    MessengerComponent mc(listenerPort);

    std::thread th_receive([&]() {
        InRed("recv_thread started");

        while (true) {
            mc.ReceiveMessage();
        }
    });

    std::thread th_send([&]() {
        InRed("send_thread started");
        InRed("");

        while (true) {
            if (showMessages)
                std::this_thread::yield();

            String command;
            std::cin >> command;

            if (command == "send") {
                USHORT port;
                std::cin >> port;

                String msg;
                std::getline(std::cin, msg);

                InRed("Test: sending...");
                IF(mc.SendMessageTo(msg, GetHostIp(), port) > 0)
                    InRed("Test: ...msg sent");
                ELSE
                    InRed("Test: ...msg not sent");

            }
            else 
                if (command == "show") {
                    if (!mc.Messages().empty()) {
                        InRed("Test: some msg in");
                        for (Message& msg : mc.Messages()) {

                            time_t tt = Clock::to_time_t(std::get<0>(msg));
                            String time = String(std::ctime(&tt));

                            time = time.substr(0, time.length() - 1);

                            IN_ADDR addr{};
                            addr.S_un.S_addr = std::get<1>(msg);
                            String ip = inet_ntoa(addr);

                            String txt = std::get<2>(msg);

                            InWhite(time + "[" + ip + "]: " + txt);

                            for (int i = 0; i < 100000; ++i);
                        }
                    }
                    else
                        InRed("Test: msg none");
                    showMessages = false;
                }
        }
    });

    while (true);
}

int TEST() {
    WsaStartup();

    //TestSendingViaTcpSender();

    //TestListenerAndReceiver();

    TestListenerSenderAndReceiver();

    //TestMessengerSomehow();

    return system("pause");
}

#endif //CURRENT_TEST == TEST_MESSENGER
#endif //CURRENT_TEST
#endif //TEST_MESSENGER

#if defined TEST_PRESENCE_COMPONENT
#if defined CURRENT_TEST
#if CURRENT_TEST == TEST_PRESENCE_COMPONENT
#include <thread>
#include <chrono>
#include "PresenceComponent.h"
using namespace FileShare;
using namespace Log;
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
#endif //CURRENT_TEST == TEST_PRESENCE_COMPONENT
#endif //CURRENT_TEST
#endif //TEST_PRESENCE_COMPONENT

#if defined TEST_USER_DATA_FILE_COMPONENT
#if defined CURRENT_TEST
#if CURRENT_TEST == TEST_USER_DATA_FILE_COMPONENT

#include <Windows.h>
#include "UserDataFileComponent.h"
using namespace FileShare; 
using namespace Log;
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
#endif // TEST_USER_DATA_FILE_COMPONENT

#if defined TEST_SHARED_FOLDER_NAVIGATOR_SELF 
#if defined CURRENT_TEST
#if CURRENT_TEST == TEST_SHARED_FOLDER_NAVIGATOR_SELF
#include <Windows.h>
#include "SharedFolderNavigatorComponent.h"

using namespace FileShare;
using namespace Log;
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
#endif // TEST_SHARED_FOLDER_NAVIGATOR_SELF

#endif _DEBUG