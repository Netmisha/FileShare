#pragma once
#include <string>
#define LOGGER
#define TO_STR(s) (#s)

class Log 
{
public:
    static int depth;
    static void Log::InRed(const std::string& message);
    static void Log::InRedWithError(const std::string& message);
    static void InWhite(const std::string& message);
    static void InFile(const std::string& message);
    static void InFileWithError(const std::string& message);
    static void DoNothing(const std::string&) {};
};
#define TextInRed(s) Log::InRed(#s)
#define __Begin ++Log::depth;
#define __End --Log::depth;

#ifdef DontLog
#define InRed(x) DoNothing(x)
#endif