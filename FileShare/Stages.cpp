#include "Stages.h"
#include "Model.h"

#define DontLog
#include "Logger.h"

using namespace FileShare;

#ifndef REGEX_CONSTS
#include <regex>
using Regex = std::regex;

const Regex rxUser("USER");
const Regex rxUsers("USRS");
const Regex rxUnread("URMSG");
const Regex rxReadMs("RMSG");
const Regex rxMsgs("MSGS");
const Regex rxPath("PATH");
const Regex rxFiles("FLS");
const Regex rxAuras("AURAS");
#endif
constexpr ULONG basicLocalIp = 3221225472; // >= 192.0.0.0
extern FileShare::String UlongIpToDotIp(ULONG ul);

Stage::ViewStage::ViewStage(ViewStage::Value val, const String& frm, const String& hlp) :
    value(val),
    format(frm),
    help(hlp),
    UpdateFormat([&](class Model&)->String { return this->format;})
{}
Stage::ViewStage::ViewStage(Value val, const String& frm, const String& hlp, const Function& fun):
    value(val),
    format(frm),
    help(hlp),
    UpdateFormat(fun)
{}
Stage::ViewStage::ViewStage(const ViewStage& sample):
    value(sample.value),
    format(sample.format),
    help(sample.help),
    provideHelp(sample.provideHelp),
    comment(sample.comment),
    UpdateFormat(sample.UpdateFormat)
{}
Stage::ViewStage& Stage::ViewStage::operator=(const ViewStage& other)
{
    value = other.value;
    format = other.format;
    help = other.help;
    provideHelp = other.provideHelp;
    comment = other.comment;
    UpdateFormat = other.UpdateFormat;

    return *this;
}

#define dOut ">> "
#define TB "\t"
#define LF "\n"
#define Line_Small "--------"
#define Line "================================="

#ifdef DEFAULT_STAGE_LAMBDA_TEMPLATE
[&](Model& model)->String
{
    String format;
    Log::InRed("UpdateFormat()->");
    __Begin;
    {
        
    }
    __End;
    Log::InRed("UpdateFormat();");
    return format;
}
#endif // DEFAULT_LAMBDA_TEMPLATE


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
        dOut    "Press any key..."     LF
    },
    String{},
    [&](Model& model)->String
    {
        String format;
        Log::InRed("UpdateFormat()->");
        __Begin;
        {
            format = HelloUser.format;
            {
                String userName;
                {
                    UserData::UserStatus selfStatus("self");
                    UserVector searchResult = model.cudf.FindUsersInFile(selfStatus);
                    if (!searchResult.empty())
                        userName = searchResult.at(0).Alias();
                }
                format = std::regex_replace(format, rxUser, userName);
            }
        }
        __End;
        Log::InRed("UpdateFormat();");
        return format;
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
        dOut    "Press any key..."
    },
    {},
    [&](Model& model)->String
    {
        String format;
        Log::InRed("UpdateFormat()->");
        __Begin;
        {
            format = Exit.format;
            {
                String userName;
                {
                    UserData::UserStatus selfStatus("self");
                    UserVector searchResult = model.cudf.FindUsersInFile(selfStatus);
                    if (!searchResult.empty())
                        userName = searchResult.at(0).Alias();
                }
                format = std::regex_replace(format, rxUser, userName);
            }
        }
        __End;
        Log::InRed("UpdateFormat();");
        return format;
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
        TB  dOut    "p; presence; aura;"      LF
        TB  dOut    "m; messenger; msg;"      LF
        TB  dOut    "u; user data; udf;"      LF
        TB  dOut    "s; shared; sf;"          LF
        TB  dOut    "q; quit; e; exit;"      LF
    },
    [&](Model& model)->String
    {
        String format;
        Log::InRed("UpdateFormat()->");
        __Begin;
        {
            format = Stage::MainMenu.format;
            {
                Int auraCount = model.cpca.activeLocalBroadcasters.size();
                format = std::regex_replace(format, rxUsers, std::to_string(auraCount));

                Int urMsgCount = model.cmsg.MessageUnreadCount();
                format = std::regex_replace(format, rxUnread, std::to_string(urMsgCount));

                Int rMsgCount = urMsgCount + model.cmsg.MessageReadCount();
                format = std::regex_replace(format, rxReadMs, std::to_string(rMsgCount));
            }    
        }
        __End;
        Log::InRed("UpdateFormat();");
        return format;
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

        dOut        "Valid userName should"         LF
        TB  dOut    "not be empty"                  LF
        TB  dOut    "not contain whitespaces"       LF
        TB  dOut    "not contain odd symbols"       LF
        TB  dOut    "not begin with number"         LF
        
        dOut        "Valid ip should"               LF
        TB  dOut    "be in a.b.c.d format"          LF
        TB  dOut    "have [a,b,c,d] in 0..255"      LF

        dOut        "Valid statuses are"            LF
        TB  dOut    "good: your favs"               LF
        TB  dOut    "bad: ppl u know"               LF
        TB  dOut    "ugly: ppl to rather ignore"    LF
    },
    [&](Model& model)->String
    {
        String format;
        Log::InRed("UpdateFormat()->");
        __Begin;
        {
            format = Stage::UserDataFile.format;
            {
                String users;
                {
                    for (auto ud : model.cudf.FindUsersInFile())
                    {
                        String user;
                        {
                            user += ud.Address().to_str() + " ";
                            user += ud.Alias() + " ";
                            user += ud.Status().to_str() + "\n";
                        }
                        users += user;
                    }
                }
                format = std::regex_replace(format, rxUsers, users);
            }
        }
        __End;
        Log::InRed("UpdateFormat();");
        return format;
    }
};
const Stage::ViewStage Stage::CheckPresence{
    Stage::ViewStage::Value::Aura,

    {
        dOut "Active Auras [USRS]"  LF
        Line                        LF
        "AURAS"                     LF
        Line                        LF
    },

    {
        dOut "Aura Component Commands"      LF
        TB  dOut    "append ip name status" LF
        TB  dOut    "back"                  LF
    },

    [&](Model& model)->String
    {
        String format;
        Log::InRed("UpdateFormat()->");
        __Begin;
        {
            format = Stage::CheckPresence.format;
            {
                Int auraCount = model.cpca.activeLocalBroadcasters.size();
                format = std::regex_replace(format, rxUsers, std::to_string(auraCount));
                String auras;
                {
                    for (auto& aura : model.cpca.activeLocalBroadcasters)
                    {
                        String ip = UlongIpToDotIp(aura.first);
                        UserData::UserAddr addr(ip);

                        UserData ud = model.cudf.FindUsersInFile(addr);
                        if (ud != UserData::BadData)
                            auras += ud.Alias() + "\t" + ud.Status().to_str() + "\n";
                        else
                            auras += ip + "\t" + "unknown" + "\n";
                    }
                }
                format = std::regex_replace(format, rxAuras, auras);
            }
        }
        __End;
        Log::InRed("UpdateFormat();");
        return format;
    }
};
const Stage::ViewStage Stage::Messenger{
    Stage::ViewStage::Value::Messenger,
    {
        "USRS"   LF
        Line     LF
        "MSGS"   LF
    },
    {
        dOut "Messenger Commands"              LF
        TB  dOut    "send all msg"             LF
        TB  dOut    "send all-status msg"      LF
        TB  dOut    "send aura msg"            LF
        TB  dOut    "back"                     LF
    },
    [&](Model& model)->String {
        String format;
        Log::InRed("UpdateFormat()->");
        __Begin;
        {
            format = Stage::Messenger.format;
            {
                String activeUsersList;
                {
                    for (auto aura : model.cpca.activeLocalBroadcasters)
                    {
                        String ip = UlongIpToDotIp(aura.first);
                        UserData::UserAddr adr(ip);
                        UserData ud = model.cudf.FindUsersInFile(adr);
    
                        if (ud != UserData::BadData)
                        {
                            activeUsersList += ud.Alias() + "(";
                            activeUsersList += ud.Status().to_str() + ")";
                        }
                        else
                        {
                            activeUsersList += ip;
                        }
    
                        activeUsersList += ";\t";
                    }
    
                    if (activeUsersList.empty())
                        activeUsersList = ">> no auras acound \n";
                }
                format = std::regex_replace(format, rxUsers, activeUsersList);
    
                String messageHistory;
                {
                    for (auto msg : model.cmsg.Messages())
                    {
                        String sMsg;
                        {
                            String time;
                            {
                                time_t tt = Clock::to_time_t(std::get<0>(msg));
                                time = String(std::ctime(&tt));
                                time = time.substr(0, time.length() - 1);
                            }
                            String user;
                            {
                                ULONG addr = std::get<1>(msg);
                                if ((addr ^ (~0)) < basicLocalIp)
                                {
                                    String ipTo = UlongIpToDotIp(addr ^ (~0));
                                    UserData::UserAddr adrTo(ipTo);
                                    UserData udTo = model.cudf.FindUsersInFile(adrTo);
                                    if (udTo != UserData::BadData)
                                        user += "to " + udTo.Alias();
                                    else
                                        user += ipTo;
                                }
                                else
                                {
                                    String ip = UlongIpToDotIp(addr);
                                    UserData::UserAddr adr(ip);
                                    auto udata = model.cudf.FindUsersInFile(adr);
                                    if (udata != UserData::BadData)
                                    {
                                        user += udata.Alias();
                                    }
                                    else
                                    {
                                        user += ip;
                                    }
                                }
                            }
                            String txt = std::get<2>(msg);
    
                            sMsg += time + "[" + user + "]: " + txt + "\n";
                        }
                        messageHistory += sMsg;
                    }
                }
                format = std::regex_replace(format, rxMsgs, messageHistory);
    
                model.cmsg.MarkAllAsRead();
            }
        }
        __End;
        Log::InRed("UpdateFormat();");
        return format;
    }
};
const Stage::ViewStage Stage::SharedFolder{
    Stage::ViewStage::Value::SharedFolder,
{   
    dOut "Self Shared Folder [FLS]"   LF
    dOut "Others Shared Folder [USRS]"  LF
},
{
    dOut "SF Commands"                  LF
    TB dOut     "self"                  LF
    TB dOut     "other"                 LF
    TB dOut     "back"                  LF
},
[&](Model& model)->String
{
    String format;
    Log::InRed("UpdateFormat()->");
    __Begin;
    {
        format = Stage::SharedFolder.format;
        {
            Int fileCount = model.csfn.self.GetFileList().size();
            format = std::regex_replace(format, rxFiles, std::to_string(fileCount));

            Int userCount = model.cpca.activeLocalBroadcasters.size();
            format = std::regex_replace(format, rxUsers, std::to_string(userCount));
        }
    }
    __End;
    Log::InRed("UpdateFormat();");
    return format;
}
};
const Stage::ViewStage Stage::SharedFolderSelf{
    Stage::ViewStage::Value::SharedFolderSelf,
    {
        "PATH"  LF
        Line    LF
        "FLS"   LF
        Line    LF
    },
    {
        dOut "SF Self Commands"                     LF
        TB dOut     "create filename.ext"           LF
        TB dOut     "open filename.ext"             LF
        TB dOut     "rename NL old.ext NL new.ext"  LF
        TB dOut     "delete filename.ext"           LF
        TB dOut     "upload file"                   LF
        TB dOut     "back"                          LF
    },
    [&](Model& model)->String
    {
        String format;
        Log::InRed("UpdateFormat()->");
        __Begin;
        {
            format = Stage::SharedFolderSelf.format;
            {
                String path = model.csfn.self.SharedFolderPath();
                format = std::regex_replace(format, rxPath, path);
    
                String files;
                {
                    for (auto& file : model.csfn.self.GetFileList())
                        files += file + "\n";
                }
                format = std::regex_replace(format, rxFiles, files);
            }
        }
        __End;
        Log::InRed("UpdateFormat();");
        return format;
    }
};
const Stage::ViewStage Stage::SharedFolderOther{
    Stage::ViewStage::Value::SharedFolderOther,
    {
        "Active users: USRS"    LF
        Line                    LF 
        "AURAS"                 LF
        Line                    LF
    },
    {
        dOut "SFOther Commands"                 LF
        TB dOut     "request usr file_list"     LF
        TB dOut     "request usr send file"     LF
        TB dOut     "request usr recv"          LF
        TB dOut     "back"                      LF
    },
    [&](Model& model)->String
    {
        String format;
        Log::InRed("UpdateFormat()->");
        __Begin;
        {
            format = Stage::SharedFolderOther.format;
            {
                Int auraCount = model.cpca.activeLocalBroadcasters.size();
                format = std::regex_replace(format, rxUsers, std::to_string(auraCount));
                String auras;
                {
                    for (auto& aura : model.cpca.activeLocalBroadcasters)
                    {
                        String ip = UlongIpToDotIp(aura.first);
                        UserData::UserAddr addr(ip);

                        UserData ud = model.cudf.FindUsersInFile(addr);
                        if (ud != UserData::BadData)
                            auras += ud.Alias() + "\t" + ud.Status().to_str() + "\n";
                        else
                            auras += ip + "\t" + "unknown" + "\n";
                    }
                }
                format = std::regex_replace(format, rxAuras, auras);
            }
        }
        __End;
        Log::InRed("UpdateFormat();");
        return format;  
    }
};
