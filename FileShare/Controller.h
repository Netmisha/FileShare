#pragma once
#include "Model.h"
#include "View.h"
#include "Stages.h"

#include <string>

namespace FileShare {
    using Data = std::string;

    class BasicController{
    public:
        BasicController(Model&, BasicView&, Stage&);
        
        BasicController() = delete;
        BasicController(const BasicController&) = delete;

     /*   virtual void OnLoad() = 0;
        virtual Data GetCommand() = 0;
        virtual CommandReinterpretation ReinterpretCommand(const Data&) = 0;
        virtual void GetModelToAct(const CommandReinterpretation&) = 0;*/

    protected:
        Model& model;
        BasicView& view;
        Stage& stage;
    };

    class ConsoleController:
        public BasicController
    {
    public:
        class CommandReinterpretation;

        ConsoleController(Model&, ConsoleView&);

        ConsoleController() = delete;
        ConsoleController(const ConsoleController&) = delete;

        virtual void OnLoad();
    protected:
        virtual Data GetCommand();
        virtual CommandReinterpretation ReinterpretCommand(const Data&);
        virtual void GetModelToAct(const CommandReinterpretation&);
    protected:
        ConsoleView& view;
    };
}

