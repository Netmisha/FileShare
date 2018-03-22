#include "Controller.h"
#include "Model.h"
#include "Logger.h"

#include <regex>
#include <conio.h>
#include <functional>

#pragma warning(disable : 4996) 

using namespace FileShare;
using STR = std::string;
using Regex = std::regex;

BasicController::~BasicController()
{
    {
        #ifdef LOGGER
        Log::TextInRed(~BasicController() :);
        #endif // LOGGER
    }
}


ConsoleController::ConsoleController(Model& mdl, ConsoleView& vw):
    model(mdl),
    view(vw)
{
    {
        #ifdef LOGGER
        Log::TextInRed(ConsoleController() :);
        #endif // LOGGER
    }
}

ConsoleController::~ConsoleController()
{
    {
        #ifdef LOGGER
        Log::TextInRed(~ConsoleController() :);
        #endif // LOGGER
    }
}
#ifndef ON_LOAD_0
void ConsoleController::OnLoad()
{
    {
        #ifdef LOGGER
        Log::TextInRed(ConsoleController::OnLoad()->);
        __Begin;
        #endif // LOGGER
    }

    for (bool keepGoing = 1; keepGoing; )
    {     
        view.Render();

        switch (view.stage.value)
        {
            case Stage::ViewStage::Value::Inception:    Inception();    break; // break->render
            case Stage::ViewStage::Value::HelloNoName:  HelloNoName();  break; // break->render
            case Stage::ViewStage::Value::HelloUser:    HelloUser();    break; // break->render
            
            case Stage::ViewStage::Value::MainMenu:     MainMenu();     continue; // norender
            case Stage::ViewStage::Value::Exit:         keepGoing = 0;  break; //
        }
    }
    {
        #ifdef LOGGER
        __End;
        Log::TextInRed(<-ConsoleController::OnLoad());
        #endif // LOGGER
    }
}
void ConsoleController::Exit()
{
    {
        #ifdef LOGGER
        Log::TextInRed(Exit()->);
        __Begin;
        #endif
    }
    {
        model.stupidThreadsDie = true;
        getch();
    }
    {
        #ifdef LOGGER
        __End;
        Log::TextInRed(<-Exit());
        #endif
    }
}
void ConsoleController::Inception()
{
    {
        #ifdef LOGGER
        __Begin;
        Log::TextInRed(Inception()->);
        #endif // LOGGER
    }
    {
        String userName;
        {
            {
                #ifdef LOGGER
                __Begin;
                Log::InRed("udf.find(self)");
                #endif // LOGGER
            }
            auto selfStatus = UserData::UserStatus::StatusValue::Self;
            auto vec = model.udfNavigator.FindUsersInFile(selfStatus);
            bool nameFound = !vec.empty();
            if (nameFound)
                userName = vec.at(0).Alias();
            {
                #ifdef LOGGER
                IF(nameFound)
                    Log::TextInRed(nameFound + );
                ELSE
                    Log::TextInRed(nameFound - );
                __End;
                #endif // LOGGER
            }
        }
        if (userName.empty())
        {
            view.stage = Stage::HelloNoName;
        }
        else
        {
            Regex user = std::regex("USER");
            
            String format = Stage::HelloUser.format;
            format = std::regex_replace(format, user, userName);
            
            view.stage = Stage::HelloUser;
            view.stage.format = format;
        }
    }
    {
        #ifdef LOGGER
        Log::TextInRed(<-Inception());
        __End;
        #endif // LOGGER
    }
}
void ConsoleController::HelloUser()
{
    {
        #ifdef LOGGER
        Log::TextInRed(HelloUser()->);
        __Begin;
        #endif
    }
    {
        getch();
        view.stage = Stage::MainMenu;
    }
    {
        #ifdef LOGGER
        __End;
        Log::TextInRed(<-HelloUser());
        #endif
    }
}
void ConsoleController::HelloNoName()
{
    {
        #ifdef LOGGER
        Log::TextInRed(HelloNoName()->);
        __Begin;
        #endif
    }
    {
        for (bool keepGoing = true; keepGoing;)
        {
            String command = GetCommand();

            if (std::regex_match(command, Regex(R"((h(!|elp)|[?]))"))) 
            {
                view.stage.provideHelp = true;
                continue;
            }

            Int bad = UserData::IsBadAlias(command);
            if (!bad)
            {
                auto selfName = command;
                auto selfAddress = model.presenceAura.GetHostIp();
                auto selfStatus = UserData::UserStatus::StatusValue::Self;
               
                auto self = UserData(selfName, selfAddress, selfStatus);              
                model.udfNavigator.AppendUser(self);

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
    {
        #ifdef LOGGER
        __End;
        Log::TextInRed(<-HelloNoName());
        #endif
    }
}
void ConsoleController::MainMenu()
{
    {
        #ifdef LOGGER
        Log::TextInRed(ConsoleController::MainMenu()->);
        __Begin;
        #endif // LOGGER
    }
    {
        for (bool keepGoing = 1; keepGoing;)
        {
            static bool updateMainMenu = true;
            {
                if (updateMainMenu)
                {
                    Regex aura("USRS");
                    Regex urMsg("URMSG");
                    Regex rMsg("RMSG");

                    String format = Stage::MainMenu.format;
                    {
                        Int auraCount = model.presenceAura.activeLocalBroadcasters.size();
                        format = std::regex_replace(format, aura, std::to_string(auraCount));

                        Int urMsgCount = model.chatMessenger.MessageUnreadCount();
                        format = std::regex_replace(format, urMsg, std::to_string(urMsgCount));

                        Int rMsgCount = model.chatMessenger.MessageReadCount();
                        format = std::regex_replace(format, rMsg, std::to_string(rMsgCount));
                    }

                    view.stage.format = format;
                    updateMainMenu = false;
                }
            }
            
            view.Render();

            switch (view.stage.value)
            {
                case Stage::ViewStage::Value::MainMenu: break;
                
                case Stage::ViewStage::Value::SharedFolder: break; // not ready
                case Stage::ViewStage::Value::UserData:     break;
                case Stage::ViewStage::Value::Aura:         break;
                case Stage::ViewStage::Value::Messenger:    break;

                case Stage::ViewStage::Value::Exit: Exit(); keepGoing = false; continue;
            }

            const String command = GetCommand();
            {
                Regex help(R"((h(!|elp)|[?]))");
                Regex exit(R"(e(!|xit)|q(!|uit))");
                Regex refr(R"(r(efresh|efr|!))");

                Regex shared(R"(s(hared|f|!))");
                Regex udf(R"(u(ser[ _]data|df|!))");

                if (std::regex_match(command, help))
                {
                    view.stage.provideHelp = true;
                }
                else if (std::regex_match(command.c_str(), exit))
                {
                    view.stage = Stage::Exit;
                }
                else if (std::regex_match(command.c_str(), refr))
                {
                    updateMainMenu = true;
                }
                else if (std::regex_match(command.c_str(), shared))
                {
                    //view.stage = Stage::SharedFolder;
                }
                else if (std::regex_match(command.c_str(), udf))
                {
                    //view.stage = Stage::UserDataFile;
                }
                /*
                .....
                */
            }
        }
    }
    {
        #ifdef LOGGER
        __End;
        Log::TextInRed(<-ConsoleController::OnLoad());
        #endif // LOGGER
    }
}
#endif

Data ConsoleController::GetCommand()
{
    return view.GetDataFromInput();
}

#ifndef ON_LOAD_1
void ConsoleController::OnLoad2()
{
    {
        #ifdef LOGGER
        __Begin;
        Log::InRed("OnLoad2(){");
        #endif // LOGGER

    }
    
    for (bool keepGoing = true; keepGoing;) {
        view.Render();
        switch (view.stage.value) {
            case Stage::ViewStage::Value::Inception            :   StageInception();               break;
            case Stage::ViewStage::Value::HelloUser            :   StageHelloUser();               break;
            case Stage::ViewStage::Value::HelloNoName    :   StageHelloUserNameless();       break;
            case Stage::ViewStage::Value::MainMenu             :   StageMainMenu();                break;
            case Stage::ViewStage::Value::Exit                 :   StageExit();keepGoing = false;  break;
        }

    }

    {
        #ifdef LOGGER
        Log::InRed("} OnLoad2()");
        __End;
        #endif // LOGGER

    } 
}
void ConsoleController::StageExit() {
    model.stupidThreadsDie = true;
    getch();
}
void ConsoleController::StageInception(){

    {
        #ifdef LOGGER
        __Begin;
        Log::TextInRed(StageInception()->);
        #endif // LOGGER

    }
    String userName;
    {
        {
            #ifdef LOGGER
            __Begin;
            Log::InRed("searching udf for userName");
            #endif // LOGGER
        }
        auto selfStatus = UserData::UserStatus::StatusValue::Self;
        auto vec = model.udfNavigator.FindUsersInFile(selfStatus);
        if (!vec.empty())
            userName = vec.at(0).Alias();

        {
            #ifdef LOGGER
            Log::InRed("we got a name");
            __End;
            #endif // LOGGER
        }
    }

    String format;

    if (userName.empty())
    {
        view.stage = Stage::HelloNoName;
        format = view.ProvideStageFormat();
        
    }
    else
    {
        view.stage.value = Stage::HelloUser.value;
        view.stage = Stage::HelloUser;
        format = view.ProvideStageFormat();
        
        std::regex user("USER");
        format = std::regex_replace(format, user, userName);
    }
    
    view.SetDataToPrint(format);


    {
        #ifdef LOGGER
        Log::TextInRed(<-StageInception());
        __End;
        #endif // LOGGER
    }
}
void ConsoleController::StageHelloUser()
{
    {
        #ifdef LOGGER
        Log::TextInRed(StageHelloUser()->);
        __Begin;
        #endif
    }

    getch();

    view.stage = Stage::MainMenu;
    String format = view.ProvideStageFormat();
    view.SetDataToPrint(format);

    {
        #ifdef LOGGER
        __End;
        Log::TextInRed(<-StageHelloUser());
        #endif
    }
}
void ConsoleController::StageHelloUserNameless() 
{  
    {
        #ifdef LOGGER
        Log::TextInRed(StageHelloUserNameless()->);
        __Begin;
        #endif
    }
    String format;
    while (true) 
    {
        STR userName = view.GetDataFromInput();

        if (std::regex_match(userName, std::regex("(h(!|elp)|[?])"))) 
        {
            format = view.ProvideStageFormat() + view.stage.help;
            break;
        }

        INT bad = UserData::IsBadAlias(userName);
        if (!bad) {
            auto selfAddress = model.presenceAura.GetHostIp();
            auto selfStatus = UserData::UserStatus::StatusValue::Self;
            
            UserData self(userName, selfAddress, selfStatus);

            model.udfNavigator.AppendUser(self);

            view.stage = Stage::Inception;
            format = view.ProvideStageFormat();

            break;
        }         
        else
            switch (bad) {
                case 1: break;// empty
                case 2: break;// whitespace
                case 3: break;// bad characters
                case 4: break;// bad prefix
            }  
    }
    
    view.SetDataToPrint(format);

    {
        #ifdef LOGGER
        __End;
        Log::TextInRed(<-StageHelloUserNameless());
        #endif
    }
}

void ConsoleController::StageMainMenu()
{
    {
        #ifdef LOGGER
        Log::TextInRed(StageMainMenu()->);
        __Begin;
        #endif
    }

    String format;
    for (bool keepGoing = true; keepGoing;)
    {
        String command = view.GetDataFromInput();

        if (std::regex_match(command, std::regex("e(xit|!)|q(uit|!)"))) {
            view.stage = Stage::Exit;
            keepGoing = false;
        }

    }
    format = view.ProvideStageFormat();
    view.SetDataToPrint(format);
    {
        #ifdef LOGGER
        __End;
        Log::TextInRed(<-StageMainMenu());
        #endif
    }
}
#endif