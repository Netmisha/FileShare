#pragma once

#include "MessengerComponent.h"
#include "SharedFolderNavigatorComponent.h"
#include "UserDataFileComponent.h"
#include "PresenceComponent.h"

namespace FileShare {



    class Model
    {
    public:

    protected:
        MessengerComponent chatMessenger;
        SharedFolderNavigatorComponent sfNavigator;
        //UDFComponent dataFileNavigator;
        PresenceComponent yetToGetAGoodName;
        friend class ConsoleController;
    };
}
