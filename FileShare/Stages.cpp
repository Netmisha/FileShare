#include "Stages.h"
using namespace FileShare;

#define dOut ">> "
#define TB "\t"
#define LF "\n"

//Data StageFormat::Inception()
//{
//    return Data{
//        dOut    "Conversion, software version 7.0"                 LF
//        dOut    "Looking at life through the eyes of a tire hub"   LF
//        dOut    "Eating seeds as a pastime activity"               LF
//        dOut    "The toxicity of our city, of our city"            LF
//    };
//}
//
//Data StageFormat::HelloUserNamed()
//{
//    return Data{
//        dOut    "Hello, USER!"          LF
//        dOut    "How is it goin?"      LF
//        dOut    "Press any k or smth"  LF
//    };
//}
//Data StageFormat::HelloUserNameless()
//{
//    return Data{
//        dOut    "Hello, Boi!"                  LF
//        dOut    "What was your name again?"    LF
//    };
//}
//
//Data FileShare::StageFormat::MainMenu(){
//    return Data{
//        dOut    "Check Presence [USRS]"         LF
//        dOut    "Messenger [URMSGS/RMSGS]"      LF
//        dOut    "User Data File"                LF
//        dOut    "Shared Folder"                 LF
//        dOut    "Quit"                          LF
//    };
//
//    /*return Data{
//        dOut "Action x"             LF
//        dOut "..."                  LF
//        dOut "My Shared folder"     LF
//        dOut "..."                  LF
//        dOut "Exit"                 LF
//    };*/
//}
//
//Data StageFormat::GoodbyeUser() {
//    return Data{
//        dOut    "Goodbye, USER!"        LF
//        dOut    "Was good to have ya!"  LF
//    };
//}
//
//
//
//Data FileShare::StageFormat::Experimental()
//{
//    return Data{
//        dOut "%s??"                             LF
//        dOut "This is experimental stage."      LF
//        dOut "You have to let it all go, Neo."  LF
//        dOut "Fear, doubt, and disbelief."      LF
//        dOut "Free your mind."                  LF
//    };
//}

Stage::ViewStage::ViewStage(ViewStage::Value val, const String& frm, const String& hlp):
    value(val),
    format(frm),
    help(hlp)
{}

Stage::ViewStage& Stage::ViewStage::operator=(const ViewStage& other)
{
    value = other.value;
    format = other.format;
    help = other.help;
    return *this;
}

const Stage::ViewStage Stage::Experimental{
    Stage::ViewStage::Value::Experimental,
    String
{
    dOut "%s??"                             LF
    dOut "This is experimental stage."      LF
    dOut "You have to let it all go, Neo."  LF
    dOut "Fear, doubt, and disbelief."      LF
    dOut "Free your mind."                  LF
}
};
const Stage::ViewStage Stage::Inception{
    Stage::ViewStage::Value::Inception,
{
    dOut    "Conversion, software version 7.0"                 LF
    dOut    "Looking at life through the eyes of a tire hub"   LF
    dOut    "Eating seeds as a pastime activity"               LF
    dOut    "The toxicity of our city, of our city"            LF
    dOut    "Conversion, software version 7.0"                 LF
    dOut    "Looking at life through the eyes of a tire hub"   LF
    dOut    "Eating seeds as a pastime activity"               LF
    dOut    "The toxicity of our city, of our city"            LF
}
};
const Stage::ViewStage Stage::HelloUser{
    Stage::ViewStage::Value::HelloUser,
{
    dOut    "Hello, USER!"         LF
    dOut    "How is it goin?"      LF
    dOut    "Press any k or smth"  LF
}

};
const Stage::ViewStage Stage::HelloUserNameless{
    Stage::ViewStage::Value::HelloUserNameless,
{
    dOut    "Hello, Boi!"                  LF
    dOut    "What was your name again?"    LF
},
{
    dOut        "Valid userName should"    LF
    TB  dOut    "not be empty"             LF
    TB  dOut    "not contain whitespaces"  LF
    TB  dOut    "not contain odd symbols"  LF
    TB  dOut    "not begin with number"    LF
}
};
const Stage::ViewStage Stage::Exit{
    Stage::ViewStage::Value::Exit,
{
    dOut    "Goodbye, USER!"        LF
    dOut    "Was good to have ya!"  LF
}
};
const Stage::ViewStage Stage::MainMenu{
    Stage::ViewStage::Value::MainMenu,
{
    dOut    "Check Presence [USRS]"         LF
    dOut    "Messenger [URMSGS/RMSGS]"      LF
    dOut    "User Data File"                LF
    dOut    "Shared Folder"                 LF
    dOut    "Quit"                          LF
},
{
    dOut        "Main Menu commands"       LF
    TB  dOut    "p!; presence; aura;"      LF
    TB  dOut    "m!; messenger; msg;"      LF
    TB  dOut    "u!; user data; udf;"      LF
    TB  dOut    "s!; shared; sf;"          LF
    TB  dOut    "q!; quit; e!; exit;"      LF
}
};
//const Stage::ViewStage Stage::CheckPresence;
//const Stage::ViewStage Stage::Messenger;
//const Stage::ViewStage Stage::SharedFolder;
//const Stage::ViewStage Stage::SharedFolderSelf;
//const Stage::ViewStage Stage::SharedFolderOther;