#include "Stages.h"
using namespace FileShare;

#define OutDelim ">> "
#define Endline "\n"

Data StageFormat::HelloUser()
{
    return{
        OutDelim "Hello USER!"          Endline
        OutDelim "How is it goin?"      Endline
    };
}

Data FileShare::StageFormat::MainMenu(){
    return{
        OutDelim "Action x"             Endline
        OutDelim "..."                  Endline
        OutDelim "My Shared folder"     Endline
        OutDelim "..."                  Endline
        OutDelim "Exit"                 Endline
    };
}

Data StageFormat::GoodbyeUser() {
    return{
        OutDelim "Goodbye, USER!"       Endline
        OutDelim "Was good to have ya!" Endline
    };
}



Data FileShare::StageFormat::Experimental()
{
    return
        OutDelim "%s??"                             Endline
        OutDelim "This is experimental stage."      Endline
        OutDelim "You have to let it all go, Neo."  Endline
        OutDelim "Fear, doubt, and disbelief."      Endline
        OutDelim "Free your mind."                  Endline
    ;
}
