#include <Windows.h>
#include <iostream>
#include <fstream>
#include "Logger.h"

namespace Log {
    int depth = 0;

    bool consoleFree = true;

    void Indent(int dpt)
    {
        while (dpt--) std::cerr << "  ";
    }

    void InColor(const std::string& message, int color, bool error=false)
    {
        #ifdef LOGGER
        while (!consoleFree);
        consoleFree = false;

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
        Indent(depth);
        std::cerr << message;
        if (error)
            std::cerr << GetLastError();
        std::cerr << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

        consoleFree = true;
        #endif // LOGGER
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

    void LogToFile(const std::string& message, bool error = false) 
    {
        std::fstream fs;
        fs.open("log.txt", std::ios::out | std::ios::app);
        fs << message;
        if (error)
            fs << GetLastError();
        fs << std::endl;
        fs.close();
    }

    void InFileWithError(const std::string& message)
    {
        LogToFile(message, true);      
    }

    void InFile(const std::string& message)
    {
        LogToFile(message);
    }

    
}
