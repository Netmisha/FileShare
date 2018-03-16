#include <Windows.h>
#include <iostream>
#include "Logger.h"

namespace Log {
    int depth = 0;
    using String = std::string;

    void Indent(int dpt)
    {
        while (dpt--) std::cerr << "  ";
    }

    void InColor(const String& message, int color, bool error=false)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
        Indent(depth);
        std::cerr << message;
        if (error)
            std::cerr << GetLastError();
        std::cerr << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    }

    void InRed(const String& message)
    {
        InColor(message, 12);
    }

    void InRedWithError(const String& message)
    {
        InColor(message, 12, true);
    }

    void InWhite(const String& message)
    {
        InColor(message, 15);
    }
}
