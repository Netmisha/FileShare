#pragma once
#include "Model.h"
#include "View.h"
#include "Stages.h"

#include <string>
#include <functional>

namespace FileShare {
    using Data = std::string;
    using Condition = std::function<bool(const String&)>;

    class BasicController{
    public:  
        virtual void OnLoad() = 0;
    protected:    
        ~BasicController();
        BasicController() = default;   
        BasicController(const BasicController&) = delete;
    };

    class StagedController:
        public BasicController
    {
    protected:
        virtual void Exit()         = 0;
        virtual void Inception()    = 0;
        virtual void HelloUser()    = 0;
        virtual void HelloNoName()  = 0;
        virtual void MainMenu()     = 0;
    };

    class ConsoleController:
        public StagedController
    {
    public:
        ConsoleController(Model&, ConsoleView&);
        ~ConsoleController();
        virtual void OnLoad();
    protected:
        virtual Data GetCommand();
        ///STAGES
        virtual void Exit();
        virtual void Inception();
        virtual void HelloUser();
        virtual void HelloNoName();
        virtual void MainMenu();
        virtual void Messenger();
        virtual void UserDataFile();
        virtual void SharedFolder();
        virtual void SharedFolderSelf();
        //virtual void SharedFolderOther();


        virtual void MessengerSendToActiveIf(const String& msg, const Condition& cd);
    private:
        Model& model;
        ConsoleView& view;
    };
}

