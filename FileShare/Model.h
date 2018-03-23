#pragma once

#include "MessengerComponent.h"
#include "SharedFolderNavigatorComponent.h"
#include "UserDataFileComponent.h"
#include "PresenceComponent.h"

namespace FileShare {
    class Model
    {
        friend class StagedController;
        friend class ConsoleController;
        friend int main();
    public:
        Model();
        Model(USHORT messengerPort, 
              USHORT sharedFolderRequestPort, 
              USHORT sharedFolderFileExchangePort, 
              USHORT presenceComponentBroadcastPort);
        ~Model();
    protected:
    public:
        MessengerComponent chatMessenger;
        SfnComponent sfNavigator;
        UDFComponent udfNavigator;
        PresenceComponent presenceAura;

        void StartAuraThreadIn();
        void StartAuraThreadOut();
        void StartMessageReceivingThread();

        bool stupidThreadsDie = false;
    };
}
