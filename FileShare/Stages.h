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
        Data HelloUser();
        Data MainMenu();

        Data GoodbyeUser();
        Data Experimental();
    }      
}
