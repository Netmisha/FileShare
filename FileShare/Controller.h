#pragma once
#include "Model.h"
#include "View.h"
#include "Stages.h"

#include <string>

namespace FileShare {
    using Data = std::string;

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
        virtual void StageExit()              = 0;
        virtual void StageInception()         = 0;
        virtual void StageHelloUser()         = 0;
        virtual void StageHelloUserNameless() = 0;

    };

    class ConsoleController:
        public StagedController
    {
    public:
        class CommandReinterpretation;

        ConsoleController(Model&, ConsoleView&);
        ~ConsoleController();

        ConsoleController() = delete;
        ConsoleController(const ConsoleController&) = delete;

        virtual void OnLoad();
        void OnLoad2();
    protected:
        virtual Data GetCommand();
    protected:
        Model& model;
        ConsoleView& view;
    protected:
        ///STAGES
        virtual void StageExit()              ;
        virtual void StageInception()         ;
        virtual void StageHelloUser()         ;
        virtual void StageHelloUserNameless() ;
        virtual void StageMainMenu()          ;

        virtual void Exit();
        virtual void Inception();
        virtual void HelloUser();
        virtual void HelloNoName();
        virtual void MainMenu();
    };
}

