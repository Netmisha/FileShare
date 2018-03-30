#ifndef _DEBUG
#include "Model.h"
#include "View.h"
#include "Controller.h"

using namespace FileShare;

int main() 
{
    Model::WsaStartup();
    Model model;
    ConsoleView view(model, std::cin, std::cout);
    ConsoleController controller(model, view);

    controller.OnLoad();
    return 0;
}
#endif