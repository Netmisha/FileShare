#ifndef _DEBUG
#include "Model.h"
#include "View.h"
#include "Controller.h"

#include <iostream>

using namespace FileShare;

int main() {
    std::cout << "the world itself is the will to power and nothing less" << std::endl;

    Model model;
    ConsoleView view(model, std::cin, std::cout);
    ConsoleController controller(model, view);

    controller.OnLoad();
    return 0;
}
#endif