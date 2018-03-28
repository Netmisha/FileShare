#include <Windows.h>
#include <iostream>
#include <fstream>
#include "Logger.h"


int Log::depth = 0;

bool outputFree = true;

void Indent(int dpt, std::ostream& os = std::cerr)
    {
        while (dpt--) os << "  ";
    }
#ifdef _DEBUG
    void InColor(const std::string& message, int color, bool error=false)
        {
            #ifdef LOGGER
            while (!outputFree);
            outputFree = false;
    
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
            Indent(Log::depth);
            std::cerr << message;
            if (error)
                std::cerr << GetLastError();
            std::cerr << std::endl;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    
            outputFree = true;
            #endif // LOGGER
        }
#else
void LogToFile(const std::string&, bool);
void InColor(const std::string& message, int color, bool error = false)
{
    LogToFile(message, error);
}
#endif // DEBUG

void Log::InRed(const std::string& message)
{
    InColor(message, 12);
}

void Log::InRedWithError(const std::string& message)
{
    InColor(message, 12, true);
}

void Log::InWhite(const std::string& message)
{
    InColor(message, 15);
}

void LogToFile(const std::string& message, bool error = false) 
{
    std::fstream fs;
    fs.open("log.txt", std::ios::out | std::ios::app);
    Indent(Log::depth, fs);
    fs << message;
    if (error)
        fs << GetLastError();
    fs << std::endl;
    fs.close();
}

void Log::InFileWithError(const std::string& message)
    {
        LogToFile(message, true);      
    }

void Log::InFile(const std::string& message)
{
    LogToFile(message);
}
    
    