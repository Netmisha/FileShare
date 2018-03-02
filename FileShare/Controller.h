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
        BasicController() = default;   
        BasicController(const BasicController&) = delete;
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
        Model& model;
        ConsoleView& view;
        Stage& stage;
    };
}

