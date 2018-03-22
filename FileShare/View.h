#pragma once
#include <iostream>
#include <string>
#include "Model.h"
#include "Stages.h"

namespace FileShare {
    using Data = std::string;
    using InStream = std::istream;
    using OtStream = std::ostream;

    class BasicView {
    protected:      
        BasicView(Model&);
        ~BasicView();

        virtual void Render() = 0;
        Model& model;
    };

    class ConsoleView :
        protected BasicView
    {
        friend class ConsoleController;
        friend int main();
    public:
        ConsoleView(Model&, InStream&, OtStream&);
        ~ConsoleView();
    protected:
        virtual void Render()    override;
        
        virtual Data GetDataFromInput();
        virtual Data ProvideStageFormat();

        virtual void ClearConsole();
        virtual void SetDataToPrint(const Data&);
        virtual void PrintData();
        virtual void PrintData(const Data&);
        
        InStream& istrm;
        OtStream& ostrm;
        
        Stage::ViewStage stage;
        Data dataToPrint;
    };
}