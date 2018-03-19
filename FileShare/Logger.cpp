#include <Windows.h>
#include <iostream>
#include "Logger.h"

namespace Log {
    int depth = 0;
    void Indent(int dpt)
    {
        while (dpt--) std::cerr << "  ";
    }

    void InColor(const std::string& message, int color, bool error=false)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
        Indent(depth);
        std::cerr << message;
        if (error)
            std::cerr << GetLastError();
        std::cerr << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    }

    void InRed(const std::string& message)
    {
        InColor(message, 12);
    }

    void InRedWithError(const std::string& message)
    {
        InColor(message, 12, true);
    }

    void InWhite(const std::string& message)
    {
        InColor(message, 15);
    }
}
