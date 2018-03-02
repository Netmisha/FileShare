#pragma once
#include <iostream>
#include <string>
#include "Model.h"
#include "Stages.h"

namespace FileShare {
    using Data = std::string;
    using InStream = std::istream;
    using OutStream = std::ostream;

    class BasicView {
    protected:      
        BasicView(Model&);
        virtual void Render() = 0;
        Model& model;
    };

    class ConsoleView :
        protected BasicView
    {
    public:
        ConsoleView(Model&, InStream&, OutStream&);
        
        ConsoleView() = delete;
        ConsoleView(const ConsoleView&) = delete;
        ConsoleView(ConsoleView&&) = delete;

    private:
        virtual void Render()    override;
        
        virtual Data GetDataFromInput();
        virtual Data ProvideStageFormat();
        virtual Data StageToData(Stage);

        virtual void ClearConsole();
        virtual void SetDataToPrint(const Data&);
        virtual void PrintData();
        
        InStream& istrm;
        OutStream& ostrm;
        
        Stage stage;
        Data dataToPrint;

        friend class Controller;
        friend class Stages;
    };
}