#pragma once
#include <string>
namespace FileShare 
{
    typedef std::string Data;
    
    enum class Stage
    {
        HelloUser,
        MainMenu,
        ///...,
        ///...,
        Exit
    };
    
    namespace StageData {
        Data HelloUser();
        Data GoodbyeUser();
    }      
}
