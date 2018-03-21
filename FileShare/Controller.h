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
        virtual CommandReinterpretation ReinterpretCommand(const Data&);
        virtual void GetModelToAct(const CommandReinterpretation&);
    protected:
        Model& model;
        ConsoleView& view;
    protected:
        ///STAGES
        virtual void StageExit()                override;
        virtual void StageInception()           override;
        virtual void StageHelloUser()           override;
        virtual void StageHelloUserNameless()   override;
        virtual void StageMainMenu();
    };
}

