#include "Controller.h"
#include "Model.h"
#include "Logger.h"

#include <regex>
#include <conio.h>
#include <ctime>
#include <sstream>

#pragma warning(disable : 4996) 

using namespace FileShare;
using STR = std::string;
using Regex = std::regex;
using StrStream = std::stringstream;

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

#define _0_255 "(0[0-9]?[0-9]?|1[0-9][0-9]|2([0-4][0-9]|5[0-5]))"
#define _dot_ "[.]"

const Regex rxIp("(" _0_255 "[.]" "){3}" _0_255);
const Regex rxFileName(R"(^[^<>:;,?"*|/\\]+[.][A-Za-z0-9]+$)");
#endif // !rxConst

BasicController::~BasicController()
{
    Log::TextInRed(~BasicController() :);
}
ConsoleController::ConsoleController(Model& mdl, ConsoleView& vw):
    model(mdl),
    view(vw)
{
    Log::TextInRed(ConsoleController() :);
}
ConsoleController::~ConsoleController()
{
    Log::TextInRed(~ConsoleController() :);
}
Data ConsoleController::GetCommand()
{
    return view.GetDataFromInput();
}

#ifndef ON_LOAD
void ConsoleController::OnLoad()
{
    Log::TextInRed(ConsoleController::OnLoad()->);
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
    Log::TextInRed(< -ConsoleController::OnLoad());
}
void ConsoleController::Exit()
{
    Log::TextInRed(Exit()->);
    __Begin;
    {
        view.Render();
        model.stupidThreadsDie = true;
        getch();
    }
    __End;
    Log::TextInRed(<-Exit());

}
void ConsoleController::Inception()
{
    Log::TextInRed(Inception()->);
    __Begin;
    {
        view.Render();

        String userName;
        {
            UserData::UserStatus selfStatus("self");
            UserVector searchResult = model.udfNavigator.FindUsersInFile(selfStatus);
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
    Log::TextInRed(<-Inception());
}
void ConsoleController::HelloUser()
{
    Log::TextInRed(HelloUser()->);
    __Begin;
    {
        view.Render();

        getch();
        view.stage = Stage::MainMenu;
    }
    __End;
    Log::TextInRed(<-HelloUser());
}
void ConsoleController::HelloNoName()
{
    Log::TextInRed(HelloNoName()->);
    __Begin;
    {
        for (bool keepGoing = true; keepGoing;)
        {
            view.Render();

            String command = GetCommand();

            if (std::regex_match(command, rxHelp))
            {
                view.stage.provideHelp = true;
                continue;
            }

            Int bad = UserData::IsBadAlias(command);
            if (!bad)
            {
                String selfName = command;
                String selfAddress = model.presenceAura.GetHostIp();
                UserData::UserStatus selfStatus("self");

                UserData selfData(selfName, selfAddress, selfStatus);
                model.udfNavigator.AppendUser(selfData);

                view.stage = Stage::Inception;
                keepGoing = false;
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
    __End;
    Log::TextInRed(<-HelloNoName());
}
void ConsoleController::MainMenu()
{
    Log::TextInRed(ConsoleController::MainMenu()->);
    __Begin;
    {
        for (bool keepGoing = 1; keepGoing;)
        {
            view.Render();

            String command = GetCommand();
            {
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

                if (std::regex_match(command.c_str(), rxShared))
                {
                    view.stage = Stage::SharedFolder;
                    SharedFolder();
                    continue;
                }

                if (std::regex_match(command.c_str(), rxUdf))
                {
                    view.stage = Stage::UserDataFile;
                    UserDataFile();
                    continue;
                }
                /*
                .....
                */

                view.stage.comment += "no such command: " + command + ". Try help";
            }
        }
    }
    __End;
    Log::TextInRed(<-ConsoleController::OnLoad());
}

String UlongIpToDotIp(ULONG ul)
{
    IN_ADDR adr{};
    adr.S_un.S_addr = ul;
    return inet_ntoa(adr);
}

void ConsoleController::Messenger()
{
    Log::TextInRed(Messenger()->);
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
                        for (auto aura : model.presenceAura.activeLocalBroadcasters)
                        {
                            String ip = UlongIpToDotIp(aura.first);
                            model.chatMessenger.SendMessageTo(msg, ip, messPort);
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
                            UserData ud = model.udfNavigator.FindUsersInFile(adr);
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
                        UserData ud = model.udfNavigator.FindUsersInFile(str);

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
    Log::TextInRed(< -Messenger());

}
void ConsoleController::MessengerSendToActiveIf(const String& msg, const Condition& cd)
{
    for (auto aura : model.presenceAura.activeLocalBroadcasters)
    {
        String ip = UlongIpToDotIp(aura.first);
        if (cd(ip))
            model.chatMessenger.SendMessageTo(msg, ip, messPort);
    }
}

void ConsoleController::UserDataFile()
{
    Log::TextInRed(UserDataFile()->);
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
                        Log::TextInRed(userName - );
                        continue;
                    }
                    UserData ud = model.udfNavigator.FindUsersInFile(userName);
                    if (ud != UserData::BadData)
                    {
                        {
                            Log::TextInRed(FindUsersInFilee + );
                        }
                        Bool removeUserResult = model.udfNavigator.RemoveUser(ud);
                        {
                            if (removeUserResult)                            
                                Log::TextInRed(RemoveUser + );
                            else
                                Log::TextInRed(RemoveUser - );
                        }
                    }
                    else
                    {
                        Log::TextInRed(FindUsersInFilee - );
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
                        Log::TextInRed(userName - );
                        continue;
                    }

                    if (!std::regex_match(ip, rxIp))
                    {
                        Log::TextInRed(ip - );
                        continue;
                    }

                    if (UserData::UserStatus::BadString(status))
                    {
                        Log::TextInRed(status - );
                        continue;
                    }

                    UserData ud(userName, UserData::UserAddr(ip), UserData::UserStatus(status));

                    Bool appendUserResult = model.udfNavigator.AppendUser(ud);
                    {
                        if (appendUserResult)
                            Log::TextInRed(AppendUser + );
                        else
                            Log::TextInRed(AppendUser - );
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
                        Log::TextInRed(userName - );
                        continue;
                    }

                    if (UserData::IsBadAlias(newName))
                    {
                        Log::TextInRed(newName - );
                        continue;
                    }

                    if (!std::regex_match(ip, rxIp))
                    {
                        Log::TextInRed(ip - );
                        continue;
                    }

                    if (UserData::UserStatus::BadString(status))
                    {
                        Log::TextInRed(status - );
                        continue;
                    }

                    UserData udOld = model.udfNavigator.FindUsersInFile(userName);
                    if (udOld == UserData::BadData)
                    {
                        Log::TextInRed(FindUsersInFile - );
                        continue;
                    }

                    if (udOld.Status() == UserData::UserStatus::StatusValue::Self)
                        status = udOld.Status().to_str();

                    UserData udNew(newName, UserData::UserAddr(ip), UserData::UserStatus(status));

                    Bool modifyUserResult = model.udfNavigator.ModifyUser(udOld, udNew);
                    {
                        if (modifyUserResult)
                            Log::TextInRed(ModifyUser + );
                        else
                            Log::TextInRed(ModifyUser - );
                    }
                    
                    continue;
                }
            }
        }
    }
    __End;
    Log::TextInRed(<-UserDataFile());
}

void ConsoleController::SharedFolder() 
{
    Log::TextInRed(SharedFolder()->);
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

                if (std::regex_match(command, rxSelf))
                {
                    view.stage = Stage::SharedFolderSelf;
                    SharedFolderSelf();
                    continue;
                }

                if (std::regex_match(command, rxOther))
                {
                    //view.stage = Stage::SharedFolderOther;
                    //SharedFolderOther();
                    continue;
                }
            }
        }
    }
    __End;
    Log::TextInRed(<-SharedFolder());
}

void ConsoleController::SharedFolderSelf()
{
    Log::TextInRed(SharedFolderSelf()->);
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
                    model.sfNavigator.self.FileRename(fileName, newfName);
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
                    model.sfNavigator.self.FileCreate(fileName);
                    continue;
                }

                if (std::regex_match(str, rxOpen))
                {
                    model.sfNavigator.self.FileOpen(fileName);
                    continue;
                }

                if (std::regex_match(str, rxDelete))
                {
                    model.sfNavigator.self.FileDelete(fileName);
                    continue;
                }

                if (std::regex_match(str, rxUpload))
                {
                    view.stage.comment = "This finction is to be implemented after throurough Aura testing.";
                }
            }
        }
    }
    __End;
    Log::TextInRed(<-SharedFolderSelf());
}
#endif