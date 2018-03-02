#include "Controller.h"
#include "CommandReinterpretation.h"
#include "Model.h"

#include <regex>

#pragma warning(disable : 4996) 

using namespace FileShare;
using CommandReinterpretation = ConsoleController::CommandReinterpretation;

FileShare::ConsoleController::ConsoleController(Model& mdl, ConsoleView& vw):
    model(mdl),
    view(vw),
    stage(vw.stage)
{}

void FileShare::ConsoleController::OnLoad()
{
    view.Render();

    Data command;
    Data format;
    Data result;
    
    for(bool forever=true; forever;){
        command = GetCommand();

        //auto commReint = ReinterpretCommand(command);
        int commReint =  std::regex_match(command, std::regex("e(xit|!)|q(uit|!)")) ? 0 : -1;
     
        switch (commReint) {

        /// the example of doing stuff
        case (int)(Stage::Experimental) :
            stage = Stage::Experimental;
            /*
            switch(commReint.Action)
            case ModelActSomeWay:
            actors params = getParams(commReint.params)
            model.SomeComponent.ActSomeWay(params);



            */
            break;

        case (int)(Stage::Exit):
            stage = Stage::Exit;
            command = "Mister";
            forever = false;
            break;
        }

        format = view.ProvideStageFormat();
        std::regex re("%s");
        result = std::regex_replace(format, re, command);

        view.dataToPrint = result;
        view.Render();
    }
}

Data FileShare::ConsoleController::GetCommand()
{
    return view.GetDataFromInput();
}

CommandReinterpretation ConsoleController::ReinterpretCommand(const Data& command)
{
    return CommandReinterpretation();
}

void FileShare::ConsoleController::GetModelToAct(const CommandReinterpretation &)
{
    /*
        model act
    */
}
