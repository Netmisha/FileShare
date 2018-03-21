#include "Controller.h"
#include "CommandReinterpretation.h"
#include "Model.h"
#include "Logger.h"

#include <regex>
#include <conio.h>

#pragma warning(disable : 4996) 

using namespace FileShare;
using STR = std::string;
using CommandReinterpretation = ConsoleController::CommandReinterpretation;

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


void FileShare::ConsoleController::OnLoad()
{
//    view.Render();
//
//    Data command;
//    Data format;
//    Data result;
//    
//    for(bool forever=true; forever;){
//        command = GetCommand();
//
//        //auto commReint = ReinterpretCommand(command);
//        int commReint =  std::regex_match(command, std::regex("e(xit|!)|q(uit|!)")) ? 0 : -1;
//     
//        switch (commReint) {
//
//        /// the example of doing stuff
//        case (int)(Stage::Experimental) :
//            view.stage = Stage::Experimental;
//            /*
//            switch(commReint.Action)
//            case ModelActSomeWay:
//            actors params = getParams(commReint.params)
//            model.SomeComponent.ActSomeWay(params);
//
//            */
//            break;
//
//        case (int)(Stage::Exit):
//            view.stage = Stage::Exit;
//            command = "Mister";
//            forever = false;
//            break;
//        }
//
//        format = view.ProvideStageFormat();
//        std::regex re("%s");
//        result = std::regex_replace(format, re, command);
//
//        view.dataToPrint = result;
//        view.Render();
//    }
}

Data FileShare::ConsoleController::GetCommand()
{
    return view.GetDataFromInput();
}

CommandReinterpretation ConsoleController::ReinterpretCommand(const Data& command)
{
    return CommandReinterpretation();
}

void ConsoleController::GetModelToAct(const CommandReinterpretation &)
{
    /*
        model act
    */
}


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
            case Stage::ViewStage::Value::HelloUserNameless    :   StageHelloUserNameless();       break;
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
        view.stage = Stage::HelloUserNameless;
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

