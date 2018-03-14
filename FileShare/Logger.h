#pragma once
#include <iostream>
#include <string>

#ifdef _DEBUG

#ifndef LOGGER
#define LOGGER
#endif LOGGER

#define IF if
#define ELIF else if
#define ELSE else

#define TO_STR(s) std::string(#s)

#define InRed(message) [](const std::string& s)->void {              \
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);   \
    std::cerr << s << std::endl;                                    \
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);   \
} (message)

#define InRedWithError(message) [](const std::string& s)->void {     \
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);   \
    std::cerr << s << GetLastError() << std::endl;                  \
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);   \
} (message)

#define InWhite(message) [](const std::string& s)->void {           \
    std::cout << s << std::endl;                                   \
}(message)


#endif _DEBUG





