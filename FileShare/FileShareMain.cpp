#include <iostream>

#include "Model.h"
#include "View.h"
#include "Controller.h"

using namespace FileShare;

int main() {
    std::cout << "the world itself is the will to power and nothing less" << std::endl;

    Model model;
    ConsoleView view(model, std::cin, std::cout);
    ConsoleController controller(model, view);

    controller.OnLoad();

    return system("pause");
}