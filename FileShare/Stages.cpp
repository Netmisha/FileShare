#include "Stages.h"
using namespace FileShare;

#define dOut ">> "
#define LF "\n"

Data StageFormat::Inception()
{
    return Data{
        dOut    "Conversion, software version 7.0"                 LF
        dOut    "Looking at life through the eyes of a tire hub"   LF
        dOut    "Eating seeds as a pastime activity"               LF
        dOut    "The toxicity of our city, of our city"            LF
    };
}

Data StageFormat::HelloUserNamed()
{
    return Data{
        dOut    "Hello, USER!"          LF
        dOut    "How is it goin?"      LF
        dOut    "Press any k or smth"  LF
    };
}
Data StageFormat::HelloUserNameless()
{
    return Data{
        dOut    "Hello, Boi!"                  LF
        dOut    "What was your name again?"    LF
    };
}

Data FileShare::StageFormat::MainMenu(){
    return Data{
        dOut    "Check Presence [USRS]"         LF
        dOut    "Messenger [URMSGS/RMSGS]"      LF
        dOut    "User Data File"                LF
        dOut    "Shared Folder"                 LF
        dOut    "Quit"                          LF
    };

    /*return Data{
        dOut "Action x"             LF
        dOut "..."                  LF
        dOut "My Shared folder"     LF
        dOut "..."                  LF
        dOut "Exit"                 LF
    };*/
}

Data StageFormat::GoodbyeUser() {
    return Data{
        dOut    "Goodbye, USER!"        LF
        dOut    "Was good to have ya!"  LF
    };
}



Data FileShare::StageFormat::Experimental()
{
    return Data{
        dOut "%s??"                             LF
        dOut "This is experimental stage."      LF
        dOut "You have to let it all go, Neo."  LF
        dOut "Fear, doubt, and disbelief."      LF
        dOut "Free your mind."                  LF
    };
}
