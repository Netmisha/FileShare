#include "Controller.h"
#include "Model.h"
#include "Logger.h"

#include <regex>
#include <conio.h>
#include <ctime>
#include <sstream>
#include <thread>

#pragma warning(disable : 4996) 

using namespace FileShare;
using STR = std::string;
using Regex = std::regex;
using StrStream = std::stringstream;
using Thread = std::thread;

#ifndef rxConst
const Regex rxExit("e(xit)?|q(uit)?");
const Regex rxBack("b(ack)?");
const Regex rxHelp("(h(elp)?|[?])");

const Regex rxMessenger("m(sg|essenger)");
const Regex rxShared("s(hared|f)");
const Regex rxUdf("u(ser[ _]data|df)?");
const Regex rxAura("p(resence)?|aura");

const Regex rxSend("s(end)?");
const Regex rxAll("a(ll)?");
const Regex rxAllStatus("a(ll)?-(g(ood)?|b(ad)?|u(gly)?)");
const Regex rxAllGood("a(ll)?-g(ood)?");
const Regex rxAllBad("a(ll)?-b(ad)?");
const Regex rxAllUgly("a(ll)?-u(gly)?");
const Regex rxSelf("s(elf)?");
const Regex rxOther("o(ther)?");

const Regex rxAppend("a(ppend)?");
const Regex rxModify("m(odify)?");
const Regex rxRemove("r(emove)?");

const Regex rxCreate("c(reate)?");
const Regex rxOpen  ("o(pen)?");
const Regex rxRename("r(ename)?");
const Regex rxDelete("d(elete)?");
const Regex rxUpload("u(pload)?");

const Regex rxRequest("r(q|equest)");
const Regex rxRecSend("s(end)?");
const Regex rxRecRecv("r(ecv)?");
const Regex rxRecList("f(l|list|ile_list)");

#define _0_255 "(0?[0-9]?[0-9]?|1[0-9][0-9]|2([0-4][0-9]|5[0-5]))"
#define _dot_ "[.]"

const Regex rxIp("(" _0_255 "[.]" "){3}" _0_255);
const Regex rxFileName(R"(^[^<>:;,?"*|/\\]+[.][A-Za-z0-9]+$)");
const Regex rxStatus("g(ood)?|b(ad)?|u(gly)?");

namespace Commands
{
    const String msgStandard = "LOLKEK";
    const String recRecvMyFile = "RECVMYFILE";
    const String fileListPls = "FILELISTPLS";
    const String recSendMeFile = "SENDMEFILE";
    const String recNoSuchFile = "NOSUCHFILE";
};
#endif // !rxConst

BasicController::~BasicController()
{
    Log::InRed("~BasicController();");
}
ConsoleController::ConsoleController(Model& mdl, ConsoleView& vw):
    model(mdl),
    view(vw)
{
    Log::InRed("ConsoleController()->");
}
ConsoleController::~ConsoleController()
{
    Log::InRed("~ConsoleController();");
}
Data ConsoleController::GetCommand()
{
    return view.GetDataFromInput();
}

#ifndef ON_LOAD
void ConsoleController::OnLoad()
{
    Log::InRed("ConsoleController::OnLoad()->");
    __Begin;
    for (bool keepGoing = 1; keepGoing; )
    {
        switch (view.stage.value)
        {
            case Stage::ViewStage::Value::Inception:    Inception();    break; // break->render
            case Stage::ViewStage::Value::HelloNoName:  HelloNoName();  break; // break->render
            case Stage::ViewStage::Value::HelloUser:    HelloUser();    break; // break->render

            case Stage::ViewStage::Value::MainMenu:     MainMenu();     break; // norender
            case Stage::ViewStage::Value::Exit:         keepGoing = 0;  break; //
        }
    }
    __End;
    Log::InRed("ConsoleController::OnLoad();");
}
void ConsoleController::Exit()
{
    Log::InRed("Exit()->");
    __Begin;
    {
        view.Render();
        model.stupidThreadsDie = true;
        getch();
    }
    __End;
    Log::InRed("Exit();");

}
void ConsoleController::Inception()
{
    Log::InRed("Inception()->");
    __Begin;
    {
        view.Render();

        String userName;
        {
            UserData::UserStatus selfStatus("self");
            UserVector searchResult = model.cudf.FindUsersInFile(selfStatus);
            if (!searchResult.empty())
                userName = searchResult.at(0).Alias();
        }
        if (userName.empty())
        {
            view.stage = Stage::HelloNoName;
        }
        else
        {
            view.stage = Stage::HelloUser;
        }
    }
    __End;
    Log::InRed("Inception();");
}
void ConsoleController::HelloUser()
{
    Log::InRed("HelloUser()->");
    __Begin;
    {
        view.Render();

        getch();
        view.stage = Stage::MainMenu;
    }
    __End;
    Log::InRed("HelloUser();");
}
void ConsoleController::HelloNoName()
{
    Log::InRed("HelloNoName()->");
    __Begin;
    {
        for (bool keepGoing = true; keepGoing;)
        {
            view.Render();

            String command = GetCommand();
            {
                if (command.empty())
                {
                    continue;
                }

                if (std::regex_match(command, rxHelp))
                {
                    view.stage.provideHelp = true;
                    continue;
                }

                Int bad = UserData::IsBadAlias(command);
                if (!bad)
                {
                    String selfName = command;
                    String selfAddress = model.cpca.GetHostIp();
                    UserData::UserStatus selfStatus("self");

                    UserData selfData(selfName, selfAddress, selfStatus);
                    model.cudf.AppendUser(selfData);

                    view.stage = Stage::Inception;
                    keepGoing = false;
                    continue;
                }
                else
                {
                    switch (bad) {
                        case 1: view.stage.comment += "error: empty";               break;
                        case 2: view.stage.comment += "error: contain whitespace";  break;
                        case 3: view.stage.comment += "error: weird characters";    break;
                        case 4: view.stage.comment += "non-word prefix";            break;
                    }
                }
            }
        }
    }
    __End;
    Log::InRed("HelloNoName();");
}
void ConsoleController::MainMenu()
{
    Log::InRed("MainMenu()->");
    __Begin;
    {
        for (bool keepGoing = 1; keepGoing;)
        {
            view.Render();

            String command = GetCommand();
            {
                if (command.empty())
                {
                    continue;
                }

                if (std::regex_match(command, rxHelp))
                {
                    view.stage.provideHelp = true;
                    continue;
                }

                if (std::regex_match(command, rxExit))
                {
                    view.stage = Stage::Exit;
                    Exit();
                    keepGoing = false;
                    continue;
                }

                if (std::regex_match(command, rxMessenger))
                {
                    view.stage = Stage::Messenger;
                    Messenger();
                    continue;
                }

                if (std::regex_match(command, rxShared))
                {
                    view.stage = Stage::SharedFolder;
                    SharedFolder();
                    continue;
                }

                if (std::regex_match(command, rxUdf))
                {
                    view.stage = Stage::UserDataFile;
                    UserDataFile();
                    continue;
                }
                
                if (std::regex_match(command, rxAura))
                {
                    view.stage = Stage::CheckPresence;
                    CheckPresence();
                    continue;
                }

                view.stage.comment += "no such command: " + command + ". Try help";
            }
        }
    }
    __End;
    Log::InRed("MainMenu();");
}
String UlongIpToDotIp(ULONG ul)
{
    IN_ADDR adr{};
    adr.S_un.S_addr = ul;
    return inet_ntoa(adr);
}
void ConsoleController::Messenger()
{
    Log::InRed("Messenger()->");
    __Begin;
    {
        for (bool keepGoing = true; keepGoing;)
        {
            view.Render();

            String command = GetCommand();
            {
                if (command.empty())
                {
                    continue;
                }
                if (std::regex_match(command, rxBack))
                {
                    view.stage = Stage::MainMenu;
                    keepGoing = false;
                    continue;
                }

                if (std::regex_match(command, rxHelp))
                {
                    view.stage.provideHelp = true;
                    continue;
                }

                StrStream ss(command);
                String str;
                {
                    ss >> str;
                }

                if (std::regex_match(str, rxSend))
                {
                    ss >> str;
                    ss.ignore();
                    String msg;
                    {
                        std::getline(ss, msg);
                    }

                    //send all active
                    if (std::regex_match(str, rxAll))
                    {
                        for (auto aura : model.cpca.activeLocalBroadcasters)
                        {
                            String ip = UlongIpToDotIp(aura.first);
                            model.cmsg.SendMessageTo(msg, ip, messPort);
                        }
                    }

                    //send all active with status
                    if (std::regex_match(str, rxAllStatus))
                    {
                        UserData::UserStatus sts;
                        {
                            if (std::regex_match(str, rxAllGood))
                                sts = UserData::UserStatus::StatusValue::Good;
                            if (std::regex_match(str, rxAllBad))
                                sts = UserData::UserStatus::StatusValue::Bad;
                            if (std::regex_match(str, rxAllUgly))
                                UserData::UserStatus::StatusValue::Ugly;
                        }

                        MessengerSendToActiveIf(msg, [&](const String ip)->bool
                        {
                            UserData::UserAddr adr(ip);
                            UserData ud = model.cudf.FindUsersInFile(adr);
                            return (ud != UserData::BadData && ud.Status() == sts);
                        });
                    }

                    //send to ip if active
                    if (std::regex_match(str, rxIp))
                    {
                        MessengerSendToActiveIf(msg, [&](const String ip)->bool
                        {
                            return str == ip;
                        });
                    }

                    //send to userName if in UDF and active
                    if (!UserData::IsBadAlias(str))
                    {
                        UserData ud = model.cudf.FindUsersInFile(str);

                        if (ud != UserData::BadData) {
                            MessengerSendToActiveIf(msg, [&](const String ip)->bool
                            {
                                return (ud.Address() == ip);
                            });
                        }
                    }
                }
            }
        }
    }
    __End;
    Log::InRed("Messenger();");

}
void ConsoleController::MessengerSendToActiveIf(const String& msg, const Condition& cd)
{
    for (auto aura : model.cpca.activeLocalBroadcasters)
    {
        String ip = UlongIpToDotIp(aura.first);
        if (cd(ip))
            model.cmsg.SendMessageTo(msg, ip, messPort);
    }
}

void ConsoleController::UserDataFile()
{
    Log::InRed("UserDataFile()->");
    __Begin;
    {
        for (bool keepGoing = true; keepGoing;)
        {
            view.Render();

            String command = GetCommand();
            {
                if (std::regex_match(command, rxBack))
                {
                    view.stage = Stage::MainMenu;
                    keepGoing = false;
                    continue;
                }

                if (std::regex_match(command, rxHelp))
                {
                    view.stage.provideHelp = true;
                    continue;
                }

                StrStream ss(command);
                String str;
                {
                    ss >> str;
                }

                if (std::regex_match(str, rxRemove))
                {
                    String userName;
                    {
                        ss >> userName;
                    }
                    if (UserData::IsBadAlias(userName))
                    {
                        Log::InRed("userName - ");
                        continue;
                    }
                    UserData ud = model.cudf.FindUsersInFile(userName);
                    if (ud != UserData::BadData)
                    {
                        {
                            Log::InRed("FindUsersInFile +");
                        }
                        Bool removeUserResult = model.cudf.RemoveUser(ud);
                        {
                            if (removeUserResult)                            
                                Log::InRed("RemoveUser +");
                            else
                                Log::InRed("RemoveUser -");
                        }
                    }
                    else
                    {
                        Log::InRed("FindUsersInFile -");
                    }
                    continue;
                }

                if (std::regex_match(str, rxAppend))
                {
                    String userName;
                    String ip;
                    String status;
                    {
                        ss >> userName >> ip >> status;
                    }

                    if (UserData::IsBadAlias(userName))
                    {
                        Log::InRed("userName -");
                        continue;
                    }

                    if (!std::regex_match(ip, rxIp))
                    {
                        Log::InRed("ip -");
                        continue;
                    }

                    if (UserData::UserStatus::BadString(status))
                    {
                        Log::InRed("status -");
                        continue;
                    }

                    UserData ud(userName, UserData::UserAddr(ip), UserData::UserStatus(status));

                    Bool appendUserResult = model.cudf.AppendUser(ud);
                    {
                        if (appendUserResult)
                            Log::InRed("AppendUser +");
                        else
                            Log::InRed("AppendUser -");
                    }
                    
                    continue;
                }
                if (std::regex_match(str, rxModify))
                {
                    String userName;
                    String newName;
                    String ip;
                    String status;
                    {
                        ss >> userName >> newName >> ip >> status;
                    }

                    if (UserData::IsBadAlias(userName))
                    {
                        Log::InRed("userName -");
                        continue;
                    }

                    if (UserData::IsBadAlias(newName))
                    {
                        Log::InRed("newName -");
                        continue;
                    }

                    if (!std::regex_match(ip, rxIp))
                    {
                        Log::InRed("ip -");
                        continue;
                    }

                    if (UserData::UserStatus::BadString(status))
                    {
                        Log::InRed("status -");
                        continue;
                    }

                    UserData udOld = model.cudf.FindUsersInFile(userName);
                    if (udOld == UserData::BadData)
                    {
                        Log::InRed("FindUsersInFile -");
                        continue;
                    }

                    if (udOld.Status() == UserData::UserStatus::StatusValue::Self)
                        status = udOld.Status().to_str();

                    UserData udNew(newName, UserData::UserAddr(ip), UserData::UserStatus(status));

                    Bool modifyUserResult = model.cudf.ModifyUser(udOld, udNew);
                    {
                        if (modifyUserResult)
                            Log::InRed("ModifyUser +");
                        else
                            Log::InRed("ModifyUser -");
                    }
                    
                    continue;
                }
            }
        }
    }
    __End;
    Log::InRed("UserDataFile();");
}
void ConsoleController::SharedFolder() 
{
    Log::InRed("SharedFolder()->");
    __Begin;
    {
        for (bool keepGoing = true; keepGoing;)
        {
            view.Render();

            String command = GetCommand();
            {
                if (command.empty())
                {
                    continue;
                }

                if (std::regex_match(command, rxBack))
                {
                    view.stage = Stage::MainMenu;
                    keepGoing = false;
                    continue;
                }

                if (std::regex_match(command, rxHelp))
                {
                    view.stage.provideHelp = true;
                    continue;
                }

                if (std::regex_match(command, rxSelf))
                {
                    view.stage = Stage::SharedFolderSelf;
                    SharedFolderSelf();
                    continue;
                }

                if (std::regex_match(command, rxOther))
                {
                    view.stage = Stage::SharedFolderOther;
                    SharedFolderOther();
                    continue;
                }
            }
        }
    }
    __End;
    Log::InRed("SharedFolder();");
}
void ConsoleController::SharedFolderSelf()
{
    Log::InRed("SharedFolderSelf()->");
    __Begin;
    {
        for (bool keepGoing = true; keepGoing;)
        {
            view.Render();

            String command = GetCommand();
            {
                if (command.empty())
                {
                    continue;
                }

                if (std::regex_match(command, rxBack))
                {
                    view.stage = Stage::MainMenu;
                    keepGoing = false;
                    continue;
                }

                if (std::regex_match(command, rxHelp))
                {
                    view.stage.provideHelp = true;
                    continue;
                }

                StrStream ss(command);

                String str;
                {
                    ss >> str;
                    ss.ignore();
                }

                if (std::regex_match(str, rxRename))
                {
                    String fileName = GetCommand();
                    {
                        if (!std::regex_match(fileName, rxFileName))
                        {
                            view.stage.comment += "Bad file name.";
                            continue;
                        }
                    }
                    String newfName = GetCommand();
                    {
                        if (!std::regex_match(newfName, rxFileName))
                        {
                            view.stage.comment += "Bad file name.";
                            continue;
                        }
                    }
                    model.csfn.self.FileRename(fileName, newfName);
                    continue;
                }

                String fileName;
                {
                    std::getline(ss, fileName);
                    if (!std::regex_match(fileName, rxFileName))
                    {
                        view.stage.comment = "Bad file name.";
                        continue;
                    }
                }

                if (std::regex_match(str, rxCreate))
                {
                    model.csfn.self.FileCreate(fileName);
                    continue;
                }

                if (std::regex_match(str, rxOpen))
                {
                    model.csfn.self.FileOpen(fileName);
                    continue;
                }

                if (std::regex_match(str, rxDelete))
                {
                    model.csfn.self.FileDelete(fileName);
                    continue;
                }

                if (std::regex_match(str, rxUpload))
                {
                    view.stage.comment = "This function is to be implemented after throurough Aura testing.";
                }
            }
        }
    }
    __End;
    Log::InRed("SharedFolderSelf();");
}
void ConsoleController::SharedFolderOther()
{
    Log::InRed("SharedFolderOther()->");
    __Begin;
    {
        for (bool keepGoing = true; keepGoing;)
        {
            view.Render();
            String command = GetCommand();
            {
                if (command.empty())
                {
                    continue;
                }

                if (std::regex_match(command, rxBack))
                {
                    view.stage = Stage::MainMenu;
                    keepGoing = false;
                    continue;
                }
                    
                if (std::regex_match(command, rxHelp))
                {
                    view.stage.provideHelp = true;
                    continue;
                }

                StrStream ss(command);
                String request;
                {
                    ss >> request;
                }
                if (std::regex_match(request, rxRequest))
                {
                    String target;
                    String rawTarget;
                    {
                        ss >> rawTarget;
                    }

                    if (std::regex_match(rawTarget, rxIp))
                    {
                        target = rawTarget;
                    }
                    else /* rawTarget == userName?*/
                    {
                        UserData ud = model.cudf.FindUsersInFile(rawTarget);
                        if (ud.IsBadData())
                        {
                            view.stage.comment = "error: bad user data";
                            continue;
                        }
                        else
                        {
                            target = ud.Address().to_str();
                        }
                    }

                    Sender requestSender(target, requestPort);
                    requestSender.ConnectToUser();

                    String specificRequest;
                    {
                        ss >> specificRequest;
                    }

                    if (std::regex_match(specificRequest, rxRecList)) 
                    {
                        String fileList = model.csfn.other20.RequestAndReceiveFileList(requestSender);

                        if (fileList.empty())
                        {
                            view.stage.comment = "failed to recv fileList";
                        }
                        else
                        {
                            view.stage.comment = fileList;
                        }
                    }

                    if (std::regex_match(specificRequest, rxRecSend))
                    {
                        String fileName;
                        {
                            ss.ignore();
                            std::getline(ss, fileName);

                            if (!std::regex_match(fileName, rxFileName)) 
                            {
                                view.stage.comment = "bad fileName";
                                continue;
                            }
                        }

                        Int requestResult = model.csfn.other20.RequestAndReceiveFile(requestSender, fileName);

                        if (requestResult)
                        {
                            view.stage.comment = "request was sent";
                        }
                        else
                        {
                            view.stage.comment = "request probably failed";
                        }
                    }

                    if (std::regex_match(specificRequest, rxRecRecv))
                    {
                        String fileName;
                        {
                            ss.ignore();
                            std::getline(ss, fileName);

                            if (!std::regex_match(fileName, rxFileName))
                            {
                                view.stage.comment = "bad fileName";
                                continue;
                            }

                            Bool fileExists = model.csfn.self.FileExists(fileName);
                            if (fileExists)
                            {
                                String folderPath = model.csfn.self.SharedFolderPath();
                                fileName = folderPath + fileName;
                            }
                            else
                            {
                                view.stage.comment = "no such file in SharedFolder";
                            }
                        }

                        Int requestResult = model.csfn.other20.RequestReceivingAndSendFile(requestSender, fileName);

                        if (requestResult)
                        {
                            view.stage.comment = "request was sent";
                        }
                        else
                        {
                            view.stage.comment = "request probably failed";
                        }
                    }
                }
                else
                {
                    continue;
                }
            }
        }
    }
    __End;
    Log::InRed("SharedFolderOther();");
}
//{
//    Log::InRed("SharedFolderOther()->");
//    __Begin;
//    {
//        for (bool keepGoing = true; keepGoing;)
//        {
//            view.Render();

//            String command = GetCommand(); 
//            {
//                if (std::regex_match(command, rxBack))
//                {
//                    view.stage = Stage::MainMenu;
//                    keepGoing = false;
//                    continue;
//                }

//                if (std::regex_match(command, rxHelp))
//                {
//                    view.stage.provideHelp = true;
//                    continue;
//                }

//                StrStream ss(command);
//                String str;
//                {
//                    ss >> str;
//                }

//                if (std::regex_match(str, rxRequest))
//                {
//                    String userSmth; {
//                        ss >> userSmth;
//                    }
//                    String userAddr;
//                    //usrSmth is ip
//                    if (std::regex_match(userSmth, rxIp))
//                    {
//                        userAddr = userSmth;
//                    }
//                    //usrSmth is name
//                    else
//                    {
//                        UserData ud = model.cudf.FindUsersInFile(userSmth);
//                        if (ud.IsBadData())
//                        {
//                            view.stage.comment = "error: bad user data";
//                            continue;
//                        }
//                        userAddr = ud.Address().to_str();
//                    }

//                    RequestSender rs(std::move(Sender(userAddr, requestPort)));
//
//                    String request;
//                    {
//                        ss >> request;
//                    }
//
//                    if (std::regex_match(request, rxRecList))
//                    {
//                        rs.SendRequest(Commands::fileListPls);
//                        RequestReceiver rr(std::move(rs));
//
//                        String fileList = rr.ReceiveRequest();
//
//                        view.stage.comment = fileList;
//                        continue;
//                    }

//                    if (std::regex_match(request, rxRecSend))
//                    {
//                        String fileName;
//                        {
//                            ss.ignore();
//                            getline(ss, fileName);
//                        }
//
//                        rs.SendRequest(Commands::recSendMeFile + fileName);
//
//                        view.stage.comment = "request sent";
//                        continue;
//                    }

//                    if (std::regex_match(request, rxRecRecv))
//                    {
//                        view.stage = Stage::SharedFolderSelf;
//                        view.Render();
//
//                        String fileName = GetCommand();
//                        String stageComment;
//                        if (!std::regex_match(fileName, rxFileName))
//                        {
//                            stageComment = "error: bad file name";
//                        }
//                        else if (!model.sfNavigator.self.FileExists(fileName))
//                        {
//                            stageComment = "error: file not found";
//                        }
//                        else
//                        {
//                            rs.SendRequest(Commands::recRecvMyFile + fileName);
//                            FileSender fs(userAddr);
//                            String path = model.sfNavigator.self.SharedFolderPath();
//                            Thread th{ [&]() {
//                                fs.SendFile(path + fileName);
//                                }
//                            };
//                            th.detach();
//                            view.stage = Stage::SharedFolderOther;
//                            continue;
//                        }
//                    }
//                }
//            }
//        }
//    }
//    __End;
//    Log::InRed("SharedFolderOther();");
//}
void ConsoleController::CheckPresence()
{
    Log::InRed("CheckPresence()->");
    __Begin;
    {
        for (bool keepGoing = true; keepGoing;)
        {
            view.Render();

            String command = GetCommand();
            {
                if (std::regex_match(command, rxBack))
                {
                    view.stage = Stage::MainMenu;
                    keepGoing = false;
                    continue;
                }

                if (std::regex_match(command, rxHelp))
                {
                    view.stage.provideHelp = true;
                    continue;
                }

                StrStream ss(command);
                String str;
                {
                    ss >> str;
                }

                if (std::regex_match(str, rxAppend))
                {
                    String ip;
                    String userName;
                    String status;
                    {
                        ss >> ip >> userName >> status;
                    }

                    if (!std::regex_match(ip, rxIp))
                    {
                        view.stage.comment = "error: bad ip";
                        continue;
                    }
                    else
                    {
                        UserData udSameAddr = model.cudf.FindUsersInFile(UserData::UserAddr(ip));
                        if (udSameAddr != UserData::BadData)
                        {
                            view.stage.comment = "error: somehow user already in UDF";
                            continue;
                        }

                    }

                    if (UserData::IsBadAlias(userName))
                    {
                        view.stage.comment = "error: bad userName";
                        continue;
                    }
                    else
                    {
                        UserData udSameName = model.cudf.FindUsersInFile(userName);
                        if (udSameName != UserData::BadData)
                        {
                            view.stage.comment = "error: userName already exists";
                            continue;
                        }
                    }

                    if (!std::regex_match(status, rxStatus))
                    {
                        view.stage.comment = "error: bad status";
                        continue;
                    }

                    UserData ud(userName, ip, status);

                    Bool appendResult = model.cudf.AppendUser(ud);
                    if (!appendResult)
                    {
                        view.stage.comment = "error: append failed somehow";
                        continue;
                    }              
                }

            }
        }
    }
    __End;
    Log::InRed("CheckPresence();");
}
#endif