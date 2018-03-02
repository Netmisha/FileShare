#include "Stages.h"
using namespace FileShare;

#define OutDelim ">> "
#define Endline "\n"

Data StageData::HelloUser()
{
    return{
        OutDelim "Hello %s!"            Endline
        OutDelim "How is it goin?"      Endline
    };
}

Data StageData::GoodbyeUser() {
    return{
        OutDelim "Goodbye %s!"          Endline
        OutDelim "Was good to have ya!" Endline
    };
}

Data FileShare::StageData::Experimental()
{
    return{
        OutDelim "%s??" Endline
        OutDelim "This is experimental stage." Endline
        OutDelim "You have to let it all go, Neo." Endline
        OutDelim "Fear, doubt, and disbelief." Endline
        OutDelim "Free your mind." Endline
    };
}
