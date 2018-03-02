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
