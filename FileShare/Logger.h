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

    void InFile(const std::string& message);
    void InFileWithError(const std::string& message);

    #define TextInRed(s) InRed(#s)
};


#define __Begin {++Log::depth;

#define __End --Log::depth;}

#define LoggingBlock(SomeCode) ++Log::depth; { SomeCode }--Log::depth;

#ifdef LOG_BLOCK
{
    #ifdef LOGGER
    Log::TextInRed();
    __Begin;
    #endif
}
{
    #ifdef LOGGER
    __End;
    Log::TextInRed();
    #endif
}
{
    #ifdef LOGGER
    __Begin;
    Log::TextInRed();
    __End;
    #endif
}


#endif

#endif // ! LOGGER