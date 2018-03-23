#include "Stages.h"
using namespace FileShare;

Stage::ViewStage::ViewStage(ViewStage::Value val, const String& frm, const String& hlp) :
    value(val),
    format(frm),
    help(hlp),
    provideHelp(false)
{}

Stage::ViewStage& Stage::ViewStage::operator=(const ViewStage& other)
{
    value = other.value;
    format = other.format;
    help = other.help;
    return *this;
}

#define dOut ">> "
#define TB "\t"
#define LF "\n"
#define Line_Small "--------"
#define Line "================================="

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
const Stage::ViewStage Stage::HelloNoName{
    Stage::ViewStage::Value::HelloNoName,
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

const Stage::ViewStage Stage::UserDataFile{
    Stage::ViewStage::Value::UserData,
{
    "USRS"         LF
    Line           LF
},
{
    dOut        "User Data File commands"       LF
    TB  dOut    "append userName ip status"     LF
    TB  dOut    "modify oldName newUser"        LF
    TB  dOut    "remove userName"               LF
    TB  dOut    "back"                          LF
    dOut                                        LF
    dOut        "Valid userName should"         LF
    TB  dOut    "not be empty"                  LF
    TB  dOut    "not contain whitespaces"       LF
    TB  dOut    "not contain odd symbols"       LF
    TB  dOut    "not begin with number"         LF
    dOut                                        LF
    dOut        "Valid ip should"               LF
    TB  dOut    "be in a.b.c.d format"          LF
    TB  dOut    "have [a,b,c,d] in 0..255"      LF
    dOut                                        LF
    dOut        "Valid statuses are"            LF
    TB  dOut    "good: your favs"               LF
    TB  dOut    "bad: ppl u know"               LF
    TB  dOut    "ugly: ppl to rather ignore"    LF
}
};

const Stage::ViewStage Stage::MainMenu{
    Stage::ViewStage::Value::MainMenu,
{
    dOut    "Check Presence [USRS]"         LF
    dOut    "Messenger [URMSG/RMSG]"        LF
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
const Stage::ViewStage Stage::CheckPresence{
    Stage::ViewStage::Value::Aura,
{
    dOut "Active Auras [LCH]"       LF
    "AURAS"                         LF
},
{
    dOut "Aura Component Commands"   LF
    TB  dOut    "add ip name status" LF
    TB  dOut    "refresh"            LF
    TB  dOut    "back"               LF
}
};
const Stage::ViewStage Stage::Messenger{
    Stage::ViewStage::Value::Messenger,
{
    "USRS"   LF
    Line                LF
    "MSGS"    LF
},
{
    dOut "Messenger Commands"               LF
    TB  dOut    "send all msg"             LF
    TB  dOut    "send all-status msg"      LF
    TB  dOut    "send name msg"            LF
    TB  dOut    "send [name1;name2;] msg"  LF
    TB  dOut    "back"                      LF
}
};
const Stage::ViewStage Stage::SharedFolder
{
    Stage::ViewStage::Value::SharedFolder,
{   
    dOut "Self Shared Folder [FILES]"   LF
    dOut "Others Shared Folder [OTH]"   LF
},
{
    dOut "SF Commands"                  LF
    TB dOut     "self"                  LF
    TB dOut     "other"                 LF
    TB dOut     "back"                  LF
}
};
const Stage::ViewStage Stage::SharedFolderSelf{
    Stage::ViewStage::Value::SharedFolderSelf,
{
    "PATH"  LF
    Line    LF
    "FILES" LF
},
{
    dOut "SF Self Commands"                 LF
    TB dOut     "screate filename.ext"      LF
    TB dOut     "open filename.ext"         LF
    TB dOut     "rename old.ext new.ext"    LF
    TB dOut     "delete filename.ext"       LF
    TB dOut     "upload file"               LF
    TB dOut     "refresh"                   LF
    TB dOut     "back"                      LF
}
};
const Stage::ViewStage Stage::SharedFolderOther{
    Stage::ViewStage::Value::SharedFolderOther,
{
        "OTHER"     LF
        Line        LF 
},
{
    dOut "SF Other Commands"                 LF
    TB dOut     "request user filename.ext"  LF
    TB dOut     "refresh"                    LF
    TB dOut     "back"                       LF
}
};