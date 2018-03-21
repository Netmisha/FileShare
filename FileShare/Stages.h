#pragma once
#include <string>
namespace FileShare 
{
    using Data = std::string;
    using String = std::string;

    #define dOut ">> "
    #define TB "\t"
    #define LF "\n"

    class Stage{
    public:
        class ViewStage {
        public:
            enum class Value
            {
                Experimental = -1,
                Exit = 0,

                Inception,
                    HelloUser,
                    HelloUserNameless,
                MainMenu,
                    SharedFolder,
                    SharedFolderSelf,
                    SharedFolderOther    
            };
            ViewStage(Value, const String&, const String& = {});
            ViewStage(const ViewStage&) = default;

            ViewStage& operator = (const ViewStage&);

            Value  value;
            String format;
            String help;
        };

        static const ViewStage Experimental         ;
        static const ViewStage Exit                 ;
        static const ViewStage Inception            ;
        static const ViewStage HelloUser            ;
        static const ViewStage HelloUserNameless    ;
        static const ViewStage MainMenu             ;
        static const ViewStage CheckPresence        ;
        static const ViewStage Messenger            ;
        static const ViewStage SharedFolder         ;
        static const ViewStage SharedFolderSelf     ;
        static const ViewStage SharedFolderOther    ;
    };



    
    //enum class Stage:int
    //{   Experimental = -1,
    //    Exit = 0,

    //    Inception,

    //    HelloUser,
    //        HelloUserNameless,
    //    MainMenu,
    //        SharedFolder,
    //            SharedFolderSelf,
    //                /*
    //                stage shows files in shared folder
    //                provides commands
    //                create file
    //                delete file
    //                rename file
    //                open file

    //                upload file to -> stage = active users -> FileSendingComponent?
    //                */
    //            SharedFolderOther//, 
    //    ///...,
    //    ///...,      
    //};
    //
    //namespace StageFormat {
    //    Data Inception();
    //    Data HelloUserNamed();
    //    Data HelloUserNameless();
    //    Data MainMenu();

    //    Data GoodbyeUser();
    //    Data Experimental();
    //}      
}
