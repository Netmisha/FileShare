#include "View.h"
#include "Stages.h"

using namespace FileShare;

using std::endl;

BasicView::BasicView(Model& mdl) :
    model(mdl)
{}

ConsoleView::ConsoleView(Model& mdl, InStream& in, OutStream& out) :
    BasicView(mdl),
    istrm(in),
    ostrm(out),
    stage(Stage::HelloUser),
    dataToPrint(ProvideStageFormat())
{}

void ConsoleView::Render(){
    ClearConsole();
    PrintData();
}

Data ConsoleView::GetDataFromInput(){
    ostrm << "<< ";
    Data str;
    std::getline(istrm, str);
    return str;;
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

Data ConsoleView::StageToData(Stage stage)
{
    switch (stage) {
    case Stage::HelloUser:
        return StageData::HelloUser();
    /*
    ...
    */
    case Stage::Exit:
        return StageData::GoodbyeUser();

    default:
        return StageData::Experimental();
    }
}
