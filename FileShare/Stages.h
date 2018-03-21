#pragma once
#include <string>
namespace FileShare 
{
    using Data = std::string;
    
    enum class Stage:int
    {   Experimental = -1,
        Exit = 0,

        Inception,

        HelloUser,
            HelloUserNameless,
        MainMenu,
            SharedFolder,
                SharedFolderSelf,
                    /*
                    stage shows files in shared folder
                    provides commands
                    create file
                    delete file
                    rename file
                    open file

                    upload file to -> stage = active users -> FileSendingComponent?
                    */
                SharedFolderOther//, 
        ///...,
        ///...,      
    };
    
    namespace StageFormat {
        Data Inception();
        Data HelloUserNamed();
        Data HelloUserNameless();
        Data MainMenu();

        Data GoodbyeUser();
        Data Experimental();
    }      
}
