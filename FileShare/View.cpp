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

ConsoleView::ConsoleView(Model& mdl, InStream& in, OutStream& out) :
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
}

void ConsoleView::Render(){
    ClearConsole();
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

Data ConsoleView::ProvideStageFormat(){
    return StageToData(stage);
}

Data ConsoleView::StageToData(Stage stg)
{
    switch (stg) {
        case Stage::Experimental:
            return StageFormat::Experimental();

        case Stage::Inception:
            return StageFormat::Inception();

        case Stage::HelloUser:
            return StageFormat::HelloUserNamed();
        case Stage::HelloUserNameless:
            return StageFormat::HelloUserNameless();

        case Stage::MainMenu:
            return StageFormat::MainMenu();
        /*
        ...
        */
        case Stage::Exit:
            return StageFormat::GoodbyeUser();

        default:
            return Data{ "yet to be implemented" };
    }
}
