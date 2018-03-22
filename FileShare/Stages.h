#pragma once
#include <string>
namespace FileShare 
{
    using String = std::string;
    using Bool = int;

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
                    HelloNoName,
                MainMenu,
                    Aura,
                    Messenger,
                    UserData,
                    SharedFolder,
                    SharedFolderSelf,
                    SharedFolderOther    
            };
            ViewStage(Value, const String&, const String& = {});
            ViewStage(const ViewStage&) = default;

            ViewStage& operator = (const ViewStage&);

            Value   value;
            String  format;
            String  help;
            Bool    provideHelp;
        };

        static const ViewStage Experimental         ;
        static const ViewStage Exit                 ;
        static const ViewStage Inception            ;
        static const ViewStage HelloUser            ;
        static const ViewStage HelloNoName          ;
        static const ViewStage MainMenu             ;
        static const ViewStage CheckPresence        ;
        static const ViewStage Messenger            ;
        static const ViewStage UserDataFile         ;
        static const ViewStage SharedFolder         ;
        static const ViewStage SharedFolderSelf     ;
        static const ViewStage SharedFolderOther    ;
    };
}
