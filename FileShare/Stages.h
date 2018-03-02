#pragma once
#include <string>
namespace FileShare 
{
    typedef std::string Data;
    
    enum class Stage:int
    {   Experimental = -1,
        Exit = 0,
        HelloUser,
        MainMenu,
        ///...,
        ///...,
        
    };
    
    namespace StageFormat {
        Data HelloUser();
        Data GoodbyeUser();
        Data Experimental();
    }      
}
