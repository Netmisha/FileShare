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
const Regex rxMessenger(R"(m(!|sg|essenger))");

const Regex rxHelp(R"((h(!|elp)|[?]))");
const Regex rxExit(R"(e(!|xit)|q(!|uit))");
const Regex rxBack("b(!|ack)");

const Regex rxRefr(R"(r(efresh|efr|!))");
const Regex rxShared(R"(s(hared|f|!))");
const Regex rxUdf(R"(u(ser[ _]data|df|!))");
const Regex rxUser("USER");

const Regex rxUsers("USRS");
const Regex rxUnread("URMSG");
const Regex rxReadMs("RMSG");

const Regex rxMsgs("MSGS");

const Regex rxSend("s(end)?");
const Regex rxAll("a(ll)?");
const Regex rxAllStatus("a(ll)?-(g(ood)?|b(ad)?|u(gly)?)");
const Regex rxAllGood("a(ll)?-g(ood)?");
const Regex rxAllBad("a(ll)?-b(ad)?");
const Regex rxAllUgly("a(ll)?-u(gly)?");

const Regex rxAppend("a(!|ppend)");
const Regex rxModify("m(!|odify)");
const Regex rxRemove("r(!|emove)");

#define _0_255 "(0[0-9]?[0-9]?|1[0-9][0-9]|2([0-4][0-9]|5[0-5]))"
#define _dot_ "[.]"

const Regex rxIp("(" _0_255 "[.]" "){3}" _0_255);
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
        view.Render();

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
        model.stupidThreadsDie = true;
        getch();
    }
    __End;
    Log::TextInRed(<-Exit());

}
void ConsoleController::Inception()
{
    __Begin;
    Log::TextInRed(Inception()->);
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
        String format = Stage::HelloUser.format;
        format = std::regex_replace(format, rxUser, userName);

        view.stage = Stage::HelloUser;
        view.stage.format = format;
    }
    Log::TextInRed(<-Inception());
    __End;
}
void ConsoleController::HelloUser()
{
    Log::TextInRed(HelloUser()->);
    __Begin;
    {
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
                //switch (bad) {
                //    case 1: break;// empty
                //    case 2: break;// whitespace
                //    case 3: break;// bad characters
                //    case 4: break;// bad prefix
                //}
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
            if (view.stage.value == Stage::ViewStage::Value::MainMenu) 
            {
                String format = Stage::MainMenu.format;
                {
                    Int auraCount = model.presenceAura.activeLocalBroadcasters.size();
                    format = std::regex_replace(format, rxUsers, std::to_string(auraCount));

                    Int urMsgCount = model.chatMessenger.MessageUnreadCount();
                    format = std::regex_replace(format, rxUnread, std::to_string(urMsgCount));

                    Int rMsgCount = urMsgCount + model.chatMessenger.MessageReadCount();
                    format = std::regex_replace(format, rxReadMs, std::to_string(rMsgCount));
                }
                view.stage.format = format;
            }

            view.Render();

            switch (view.stage.value)
            {
                case Stage::ViewStage::Value::SharedFolder: break; // not ready
                case Stage::ViewStage::Value::UserData:     UserDataFile(); continue;
                case Stage::ViewStage::Value::Aura:         break;
                case Stage::ViewStage::Value::Messenger:    Messenger();    continue;

                case Stage::ViewStage::Value::Exit:         Exit(); keepGoing = false; continue;
            }

            String command = GetCommand();
            {
                if (std::regex_match(command, rxHelp))
                {
                    view.stage.provideHelp = true;
                    continue;
                }

                if (std::regex_match(command.c_str(), rxExit))
                {
                    String format = Stage::Exit.format;
                    {
                        String userName;
                        {
                            UserData::UserStatus selfStatus("self");
                            UserVector searchResult = model.udfNavigator.FindUsersInFile(selfStatus);
                            userName = searchResult.at(0).Alias();
                        }
                        format = std::regex_replace(format, rxUser, userName);

                    }
                    view.stage = Stage::Exit;
                    view.stage.format = format;
                    continue;
                }

                if (std::regex_match(command.c_str(), rxMessenger))
                {
                    view.stage = Stage::Messenger;
                }

                if (std::regex_match(command.c_str(), rxShared))
                {
                    //view.stage = Stage::SharedFolder;
                }
                if (std::regex_match(command.c_str(), rxUdf))
                {
                    view.stage = Stage::UserDataFile;
                }
                /*
                .....
                */
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
            String format = Stage::Messenger.format;
            {
                String activeUsersList;
                {
                    for (auto aura : model.presenceAura.activeLocalBroadcasters)
                    {
                        String ip = UlongIpToDotIp(aura.first);
                        UserData::UserAddr adr(ip);
                        UserData ud = model.udfNavigator.FindUsersInFile(adr);

                        if (ud != UserData::BadData)
                        {
                            activeUsersList += ud.Alias();
                        }
                        else
                        {
                            activeUsersList += ip;
                        }

                        activeUsersList += ";\t";
                    }

                    if (activeUsersList.empty())
                        activeUsersList = ">> no auras acound \n";
                }
                format = std::regex_replace(format, rxUsers, activeUsersList);

                String messageHistory;
                {
                    for (auto msg : model.chatMessenger.Messages())
                    {
                        String sMsg;
                        {
                            String time;
                            {
                                time_t tt = Clock::to_time_t(std::get<0>(msg));
                                time = String(std::ctime(&tt));
                                time = time.substr(0, time.length() - 1);
                            }
                            String user;
                            {
                                String ip = UlongIpToDotIp(std::get<1>(msg));
                                UserData::UserAddr adr(ip);
                                auto udata = model.udfNavigator.FindUsersInFile(adr);
                                if (udata != UserData::BadData)
                                {
                                    user += udata.Alias();
                                }
                                else
                                {
                                    user += ip;
                                }
                            }
                            String txt = std::get<2>(msg);

                            sMsg += time + "[" + user + "]: " + txt + "\n";
                        }
                        messageHistory += sMsg;
                    }
                }
                format = std::regex_replace(format, rxMsgs, messageHistory);

                model.chatMessenger.MarkAllAsRead();
            }
            view.stage.format = format;
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
            String format = Stage::UserDataFile.format;
            {
                String users;
                {
                    for (auto ud : model.udfNavigator.FindUsersInFile())
                    {
                        String user;
                        {
                            user += ud.Address().to_str() + " ";
                            user += ud.Alias() + " ";
                            user += ud.Status().to_str() + "\n";
                        }
                        users += user;
                    }
                }
                format = std::regex_replace(format, rxUsers, users);
            }
            view.stage.format = format;
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
                        if (model.udfNavigator.RemoveUser(ud))
                        {

                            Log::TextInRed(RemoveUser + );

                        }
                        else
                        {

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

                    if (model.udfNavigator.AppendUser(ud))
                    {
                        Log::TextInRed(AppendUser + );
                    }
                    else
                    {
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

                    if (model.udfNavigator.ModifyUser(udOld, udNew))
                    {
                        Log::TextInRed(ModifyUser + );
                    }
                    else
                    {
                        Log::TextInRed(ModifyUser - );
                    }
                    continue;
                }
            }
        }
    }
    __End;
    Log::TextInRed(< -UserDataFile());
}
#endif