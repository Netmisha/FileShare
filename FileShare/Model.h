#pragma once

#include "MessengerComponent.h"
#include "SharedFolderNavigatorComponent.h"
#include "UserDataFileComponent.h"
#include "PresenceComponent.h"

#include <thread>
//#include <utility>

namespace FileShare {

    using Thread = std::thread;
    using Request = std::pair<String, String>; // received
    using RequestVector = std::vector<Request>;
    using FileVector = std::vector<String>;
   
    using Clock = std::chrono::system_clock;
    using TimePoint = Clock::time_point;
    using Duration = std::chrono::seconds;

    class Model
    {
    public:

        Model();
        Model(USHORT messengerPort, USHORT sfoRequestPort, USHORT sfoFilePort, USHORT preCompPort);
        ~Model();

        friend class StagedController;
        friend class ConsoleController;
    //protected:
    public:
        MessengerComponent chatMessenger;
        SfnComponent sfNavigator;
        UDFComponent udfNavigator;
        PresenceComponent presenceAura;

        void StartAuraThreadIn();
        void StartAuraThreadOut();

        bool stupidThreadsDie = false;
    };
}
