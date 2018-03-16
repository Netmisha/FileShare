#pragma once
#include <string>

#ifdef _DEBUG

#ifndef LOGGER
#define LOGGER
#endif  LOGGER

#define IF   if
#define ELSE else
#define ELIF ELSE IF

#define TO_STR(s) (#s)

namespace Log {
    extern int depth;
    void Indent(int);
    void InRed(const std::string& message);
    void InRedWithError(const std::string& message);
    void InWhite(const std::string& message);
};

#endif _DEBUG