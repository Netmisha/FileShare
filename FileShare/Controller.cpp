#include "Controller.h"
#include "CommandReinterpretation.h"
#include "Model.h"

#pragma warning(disable : 4996) 

using namespace FileShare;
using CommandReinterpretation = ConsoleController::CommandReinterpretation;

BasicController::BasicController(Model& mdl, BasicView& bvw, Stage& stg):
    model(mdl),
    view(bvw),
    stage(stg)
{}

FileShare::ConsoleController::ConsoleController(Model& mdl, ConsoleView& vw):
    BasicController(mdl, vw, vw.stage),
    view(vw)
{}

void FileShare::ConsoleController::OnLoad()
{
    view.Render();

    while (int i=1) {
        Data command = GetCommand();
        //auto commReint = ReinterpretCommand(command);
        int commReint = std::stoi(command);
        
        switch (commReint) {
        case 1:
            view.dataToPrint = "case 1";
            break;
        case 2:
            view.dataToPrint = "case 2";
            break;
        case (int)(Stage::Exit) :
            stage = Stage::Exit;
            view.dataToPrint = view.ProvideStageFormat();
            break;
        
        /// the example of doing stuff
        case (int)(Stage::Experimental) :
            stage = Stage::Experimental;
            Data format = view.ProvideStageFormat();
            /*
                switch(commReint.Action)
                    case ModelActSomeWay:
                        actors params = getParams(commReint.params)
                        model.SomeComponent.ActSomeWay(params);
                        
                        sprintf(&result, &format, model.SomeComponent.Something)

            */
            Data result("", 256);
            sprintf(&result.front(), format.c_str(), command);
            view.dataToPrint = result;
            break;
        }
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
