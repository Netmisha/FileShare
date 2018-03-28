#include "View.h"
#include "Stages.h"

using namespace FileShare;
#define DontLog
#include "Logger.h"
using std::endl;

BasicView::BasicView(Model& mdl) :
    model(mdl)
{
    Log::InRed("BasicView() :");
}

BasicView::~BasicView()
{
    Log::InRed("~BasicView()");
}

ConsoleView::ConsoleView(Model& mdl, InStream& in, OtStream& out) :
    BasicView(mdl),
    istrm(in),
    ostrm(out),
    stage(Stage::Inception),
    dataToPrint(ProvideStageFormat())
{
        Log::InRed("ConsoleView() :");
}

FileShare::ConsoleView::~ConsoleView()
{
    Log::InRed("~ConsoleView() :");
    #ifdef NDEBUG
        ClearConsole();
    #endif // _RELEASE
}

void ConsoleView::Render(){
    ClearConsole();

    Data toPrint = stage.UpdateFormat(model);

    if (!stage.comment.empty())
        toPrint += ">> " + std::move(stage.comment) + "\n";

    if (stage.provideHelp)
    {
        toPrint += stage.help;
        stage.provideHelp = false;
    }
    
    SetDataToPrint(toPrint);
    PrintData();
}

Data ConsoleView::GetDataFromInput() {
    Data str;
    Log::InRed("GetDataFromInput()->");
    __Begin;
    {
        ostrm << "<< ";
        std::getline(istrm, str);
    }
    __End;
    Log::InRed("< -GetDataFromInput()");
    return str;
}

void ConsoleView::ClearConsole()
{
    system("CLS");
}

void ConsoleView::SetDataToPrint(const Data& data){
    dataToPrint = data;
}

void FileShare::ConsoleView::PrintData()
{
    ostrm << dataToPrint;
}

void FileShare::ConsoleView::PrintData(const Data& data)
{
    ostrm << data;
}

Data ConsoleView::ProvideStageFormat()
{
    return stage.format;
}