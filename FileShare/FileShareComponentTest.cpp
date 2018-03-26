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
#define TEST_MABITCH_MODEL_OUT              6
#define TEST_TRYING_TO_VIEW                 7
#define TEST_TRYING_TO_CONTROL              8

#if !defined(CURRENT_TEST)
#define CURRENT_TEST 8
#endif // !CURRENT_TEST

#ifdef TEST_TRYING_TO_CONTROL
#ifdef CURRENT_TEST
#if CURRENT_TEST==TEST_TRYING_TO_CONTROL

#include "Model.h"
#include "View.h"
#include "Controller.h"

using namespace FileShare;
void WsaStartup() {
    static WSADATA wsaData;
    int error = WSAStartup(MAKEWORD(2, 2), &wsaData);
}
int TEST() {
    WsaStartup();

    {
        #ifdef LOGGER
        Log::TextInRed(Model:);
        ++Log::depth;
        #endif // LOGGER
    }

    USHORT msgPort;
    std::cin >> msgPort;
    std::cin.ignore();

    Model model(msgPort, requestPort, sharePort, preComPort);
    //Model model;
    {
        #ifdef LOGGER
        --Log::depth;
        Log::TextInRed(:Model);
        #endif // LOGGER
    }

    {
        #ifdef LOGGER
        Log::TextInRed(ConsoleView:);
        ++Log::depth;
        #endif // LOGGER
    }
    ConsoleView view(model, std::cin, std::cout);
    {
        #ifdef LOGGER
        --Log::depth;
        Log::TextInRed(:ConsoleView);
        #endif // LOGGER
    }

    {
        #ifdef LOGGER
        Log::TextInRed(ConsoleController:);
        ++Log::depth;
        #endif // LOGGER
    }
    ConsoleController controller(model, view);
    {
        #ifdef LOGGER        
        --Log::depth;
        Log::TextInRed(:ConsoleController);
        #endif // LOGGER
    }

    controller.OnLoad();
    return 0;
}


#endif // CURRENT_TEST==TEST_TRYING_TO_CONTROL
#endif // CURRENT_TEST
#endif // TEST_TRYING_TO_CONTROL

#ifdef TEST_TRYING_TO_VIEW
#ifdef CURRENT_TEST
#if CURRENT_TEST == TEST_TRYING_TO_VIEW

#include "View.h"
using namespace FileShare;

int TEST() {

    Model mdl;
    ConsoleView cv(mdl, std::cin, std::cout);

    cv.SetDataToPrint("lol kek" "\n");

    while (true) {
        cv.Render();

        for (int i = (int)Stage::Experimental; i < 10; ++i) {
            Log::InRed(cv.StageToData((Stage)i));
        }

        String str = cv.GetDataFromInput();

        cv.SetDataToPrint("++" + str + "++" + "\n"); 
    }

    system("pause");
}

#endif // 
#endif // CURRENT_TEST
#endif // TEST_TRYING_TO_VIEW

#ifdef TEST_MABITCH_MODEL_OUT
#ifdef CURRENT_TEST
#if CURRENT_TEST == TEST_MABITCH_MODEL_OUT

#include "Model.h"
#include <ctime>
#include <sstream>
#include <thread>
using namespace FileShare;

void WsaStartup() {
    static WSADATA wsaData;
    int error = WSAStartup(MAKEWORD(2, 2), &wsaData);
}

VOID TestUdfComponent(Model&);
VOID TestMessenger(Model&);

using namespace FileShare;
int TEST() {
    Log::InRed(TO_STR(TEST_MABITCH_MODEL_OUT));
    {
        #ifdef LOGGER
        Log::InRed("creating Model");
        #endif // LOGGER
    }

    WsaStartup();
    USHORT msgPort;
    std::cin >> msgPort;
    std::cin.ignore();

    Model mdl(msgPort, requestPort, sharePort, preComPort);

    //TestUdfComponent(mdl);

    TestMessenger(mdl);




    return system("pause");
}

VOID TestMessenger(Model& mdl)
{
    while (true) {
        String str;
        std::getline(std::cin, str);

        if(str == "show")
            for (Message& msg : mdl.chatMessenger.Messages()) {

                time_t tt = Clock::to_time_t(std::get<0>(msg));
                String time = String(std::ctime(&tt));

                time = time.substr(0, time.length() - 1);

                IN_ADDR addr{};
                addr.S_un.S_addr = std::get<1>(msg);
                String ip = inet_ntoa(addr);

                String txt = std::get<2>(msg);

                Log::InWhite(time + "[" + ip + "]: " + txt);

                for (int i = 0; i < 100000; ++i);
            }
        else
        {
            std::stringstream ss(str);

            USHORT port;
            ss >> port;
            ss.ignore();
            String msg;
            std::getline(ss, msg);

            mdl.chatMessenger.SendMessageTo(msg, mdl.presenceAura.GetHostIp(), port);

        }
    }

}

VOID TestUdfComponent(Model& mdl) {
    Log::TextInRed(TestUdfComponent{ );
    __Begin;
    for (bool repeatThisTest = TRUE; repeatThisTest;)
    {
        Log::InRed("Get good userName");
        String userName;
        {
            __Begin;
            for (bool keepGoing = true; keepGoing;) {
                std::getline(std::cin, userName);

                if (userName == "quit")
                    repeatThisTest = FALSE;

                switch (Bool bad = UserData::IsBadAlias(userName)) {
                    case 0:
                        Log::InRed("Good name");
                        keepGoing = false;
                        break;
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                        Log::InRed("Bad name: " + std::to_string(bad));
                }
            }
            __End;
        }

        Log::InRed("Checking if user exists");
        BOOL userExists;
        {
            __Begin;
            UserData userFound = mdl.udfNavigator.FindUsersInFile(userName);
            userExists = (userFound != UserData::BadData);
            __End;
        }

        if (userExists) {
            Log::InRed("userName exists");
        }
        else
        {
            Log::InRed("userName doesnt exist. adding");
            __Begin;
            String ip = mdl.presenceAura.GetHostIp();
            UserData::UserStatus sts = UserData::UserStatus::StatusValue::Good;
            UserData newUser(userName, ip, sts);

            mdl.udfNavigator.AppendUser(newUser);
            __End;
        }
    }
    __End;
    Log::TextInRed(}TestUdfComponent);
}

#endif // CURRENT_TEST == TEST_MABITCH_MODEL_OUT
#endif // CURRENT_TEST
#endif // TEST_MABITCH_MODEL_OUT


#if defined TEST_SHARED_FOLDER_NAVIGATOR_OTHER
#if defined CURRENT_TEST 
#if CURRENT_TEST == TEST_SHARED_FOLDER_NAVIGATOR_OTHER

#include <thread>
#include <chrono>
#include "SharedFolderNavigatorOther.h"

using namespace FileShare;
using Clock = std::chrono::system_clock;
using Duration = std::chrono::duration<INT>;
using TimePoint = std::chrono::system_clock::time_point;
using Seconds = std::chrono::seconds;

String GetHostIp();

void TestFileListenerSenderReceiver(); //works ok
void TestRequestListenerSenderReceiver_TryMakeItAsItIsSupposedToBe(); // kinda works

String hostIp = GetHostIp();
String folderPath = R"(C:\Users\oleh.kostiv\Documents\Visual Studio 2015\Projects\FileShare\Debug\)";
String folderFrom = R"(SharedFolder\)";
String folderTo = R"(OtherFolder\)";
String fileName = "paintme.bmp";

USHORT filePort = 0x2b67;
USHORT requPort = 0x56ce;

int TEST() {
    //*****************************************************
    Log::InRed(TO_STR(TEST_SHARED_FOLDER_NAVIGATOR_OTHER));
    //*****************************************************

    WSADATA wsaData;
    int error = WSAStartup(MAKEWORD(2, 2), &wsaData);

    //TestFileListenerSenderReceiver(); 

    // the plan is to turn request sender 
    // into request receiver 
    // after SendRequest is complete
    TestRequestListenerSenderReceiver_TryMakeItAsItIsSupposedToBe();

    return system("pause");
}

String GetHostIp()
{
    WSADATA wsaData;
    int error = WSAStartup(MAKEWORD(2, 2), &wsaData);
    char hostName[1024]{};
    IF (gethostname(hostName, sizeof(hostName)) == SOCKET_ERROR)
        return{};
    return inet_ntoa(*((IN_ADDR**)(gethostbyname(hostName))->h_addr_list)[0]);
}
void Wait(const Duration dur) {
    std::this_thread::sleep_for(dur);
}
void TrySendingFile(){
    ++Log::depth;
    Log::InRed("Try sending file");

    bool keepAtIt = true;
    while (keepAtIt) {
        ++Log::depth;
        Log::InRed("Creating FS");

        FileSender fs(hostIp, filePort);

        IF (fs.InvalidSocket()) {
            Log::InRedWithError("FS invalid");
            continue;
        }

        Int someResult = SOCKET_ERROR;

        for (auto then = Clock::now(); Clock::now() - then < Seconds(4); Wait(Seconds(1))) 
        {
            someResult = fs.ConnectToUser();
            IF (someResult != SOCKET_ERROR)
                break;
        }

        IF (someResult == SOCKET_ERROR) {
            Log::InRed("failed to connect");
        }
        ELSE {
            for (auto then = Clock::now(); Clock::now() - then < Seconds(4); Wait(Seconds(1))) 
            {
                someResult = fs.SendFile(folderPath + folderFrom + fileName, 1024);
                IF (someResult != SOCKET_ERROR) {
                    keepAtIt = false;
                    break;
                }
            }
            IF (someResult == SOCKET_ERROR) {
                Log::InRed("failed to send");
            }
        }
        --Log::depth;
    }
}
void TryReceivingFile() {
    ++Log::depth;
    Log::InRed("Try recv file");

    Log::InRed("Creating FL");

    FileListener fl(filePort);
    fl.Bind();
    fl.Listen();

    while (true) {
        ++Log::depth;
        FileReceiver fr = fl.Accept();
        IF(fr.ReceiveFile(folderPath + folderTo + fileName) != SOCKET_ERROR)
            break;
        --Log::depth;
    }

    --Log::depth;
}
void TestFileListenerSenderReceiver()
{
    int sendRecv;
    std::cin >> sendRecv;
    switch (sendRecv) {
        case 1:TrySendingFile(); break;
        case 2:TryReceivingFile(); break;
    }
}

bool TimeOut(const TimePoint& then, const Duration& timeOut = Seconds(5)) {
    auto now = Clock::now();
    IF (now - then < timeOut)
        return false;
    ELSE
        return true;
}

INT RequestSenderTryConnect(RequestSender& rs) {
    INT result = SOCKET_ERROR;
    for (TimePoint now = Clock::now();;)
    __Begin
        result = rs.ConnectToUser();
        IF(result != SOCKET_ERROR) {
            Log::InRed("Connected alright");
            break;
        }
        ELIF(TimeOut(now)) {
            Log::InRed("Connect timeout");
            break;
        }
        Wait(std::chrono::seconds(1));
    __End
    return result;
}
INT RequestSenderTrySend(RequestSender& rs, const String& request) {
    INT result = SOCKET_ERROR;
    for (TimePoint now = Clock::now();;)
    __Begin
        result = rs.SendRequest(request);
        IF(result != SOCKET_ERROR) {
            Log::InRed("Request sent");
            break;
        }
        ELIF(TimeOut(now)) {
            Log::InRed("Send timeout");
            break;
        }
        Wait(std::chrono::seconds(1));
    __End
    return result;
}
INT RequestReceiverTryReceive(RequestReceiver& rr) {
    for (int i = 0; i < 10; ++i)
    __Begin
        String response = rr.ReceiveRequest();
        if (!response.empty())
            Log::InWhite(response);

        Wait(std::chrono::seconds(1));
    __End
    return 0;
}
void TrySendingRequest() {
    Log::InRed("Trying to send request");
    ++Log::depth;

    for (bool keepGoing = true; keepGoing; Log::InRed("Cycle complete"))
    {
        ++Log::depth;
        Log::InRed("Creqte request sender");
        RequestSender rs(hostIp, requPort);
        {
            IF (rs.InvalidSocket()) 
            {
                Log::InRed("rs socket fail, try again");
                continue;
            }
            ELSE{
                INT result = RequestSenderTryConnect(rs);

                IF (result != SOCKET_ERROR) 
                {
                    String request = "This Is My Request";

                    result = RequestSenderTrySend(rs, request);

                    IF(result != SOCKET_ERROR) 
                    {
                        Log::InRed("Trying RS->RR");
                        RequestReceiver rr = std::move(dynamic_cast<TCPSocketedEntity&>(rs));
                        RequestReceiverTryReceive(rr);

                        keepGoing = false;
                    }
                }
            }
        }
        --Log::depth;
    }
    --Log::depth;
}
void TryReceiveRequestAndRespondSomehow() {
    Log::InRed("Try receive Request and respond to it");
    __Begin
        RequestListener rl(requPort);
        rl.Bind();
        rl.Listen();

        while (true)
        __Begin
        {
            TCPSocketedEntity se = rl.Accept();

            if (!se.InvalidSocket()) {

                auto threadFun = [](TCPSocketedEntity se) {
                    __Begin
                    {
                        RequestReceiver rr(std::move(Receiver(std::move(se))));

                        String request;
                        while(request.empty())
                            request = rr.ReceiveRequest();
                        Log::InWhite(request);

                        RequestSender rs(std::move(rr));
                        for(int i=0; i< 10; ++i) 
                        {
                            rs.SendRequest("response to " + request);
                            Wait(Seconds(2));
                        }
                    }
                    __End
                };

                std::thread th(threadFun, std::move(se));
                th.detach();
            }
        }
        __End
    __End
}
void TestRequestListenerSenderReceiver_TryMakeItAsItIsSupposedToBe()
{
    for (bool badChoice = true; badChoice; badChoice = [&]()
    {   
        bool result = false;
        CHAR sendRecv = std::cin.get();
        switch (sendRecv) 
        {
            case '1':
                TrySendingRequest();
                break;
            case '2':
                TryReceiveRequestAndRespondSomehow();
                break;
            default:
                result = true;
                break;
        }
        return result;
    }());
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

        IF (buff.empty())
            continue;
        ELSE
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
            IF (message.empty())
                continue;
            ELSE
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
            IF (showMessages)
                std::this_thread::yield();

            String command;
            std::cin >> command;

            IF (command == "send") {
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
            ELSE 
                IF (command == "show") {
                    IF (!mc.Messages().empty()) {
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
                    ELSE
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

    InRed("Tried modIFying user");
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
    IF (paintmeName != "paintme.bmp") {
        InRed("paintme.bmp already existed.");
        InRed("new paintme is created as " + paintmeName);
        InRed("gonna rename " + paintmeName + " somehow");
        self.FileRename(paintmeName, "new_paintme_name_with_blackjack_and_hookers.bmp");
    }
    ELSE
        InRed("paintme.bmp created");
    {
        FileVector fv = self.GetFileList();
        IF (fv.empty())
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