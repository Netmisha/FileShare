#include "Logger.h"
#include "View.h"
#include "Stages.h"

using namespace FileShare;

using std::endl;

BasicView::BasicView(Model& mdl) :
    model(mdl)
{
    {
        #ifdef LOGGER
        Log::TextInRed(BasicView() :);
        #endif // LOGGER
    }
}

BasicView::~BasicView()
{
    {
        #ifdef LOGGER
        Log::TextInRed(~BasicView());
        #endif // LOGGER
    }
}

ConsoleView::ConsoleView(Model& mdl, InStream& in, OtStream& out) :
    BasicView(mdl),
    istrm(in),
    ostrm(out),
    stage(Stage::Inception),
    dataToPrint(ProvideStageFormat())
{
    {
        #ifdef LOGGER
        Log::TextInRed(ConsoleView() :);
        #endif // LOGGER
    }
}

FileShare::ConsoleView::~ConsoleView()
{
    {
        #ifdef LOGGER
        Log::TextInRed(~ConsoleView() :);
        #endif // LOGGER
    }
    {
        #ifdef NDEBUG
        ClearConsole();
        #endif // _RELEASE
    }
}

void ConsoleView::Render(){
    ClearConsole();

    Data toPrint = stage.format;

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
    {
        {
            #ifdef LOGGER
            Log::TextInRed(GetDataFromInput()->);
            __Begin;
            #endif
        }
        ostrm << "<< ";
        std::getline(istrm, str);
        {
            #ifdef LOGGER
            __End;
            Log::TextInRed(< -GetDataFromInput());
            #endif
        }
    }
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