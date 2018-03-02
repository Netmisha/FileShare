#pragma once

#include "MessengerComponent.h"
#include "SharedFolderNavigatorComponent.h"
#include "UserDataFileComponent.h"
#include "PresenceComponent.h"

namespace FileShare {

    class MessengerComponent;
    class SharedFolderNavigatorComponent;
    class UserDataFileComponent;
    class PresenceComponent;

    class Model
    {
    public:

    protected:
        MessengerComponent chatMessenger;
        SharedFolderNavigatorComponent sfNavigator;
        UserDataFileComponent dataFileNavigator;
        PresenceComponent yetToGetAGoodName;
        friend class ConsoleController;
    };
}
