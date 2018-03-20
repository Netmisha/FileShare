#pragma once
#include <string>

#ifdef _DEBUG
#define LOGGER

#define IF   if
#define ELSE else
#define ELIF ELSE IF

#define TO_STR(s) (#s)

namespace Log {
    extern int depth;
    void InRed(const std::string& message);
    void InRedWithError(const std::string& message);
    void InWhite(const std::string& message);
};

#define __Begin {++Log::depth;{

#define __End }--Log::depth;}

#endif // _DEBUG