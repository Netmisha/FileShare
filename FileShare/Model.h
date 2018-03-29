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
        //component: Messenger
        MessengerComponent cmsg;
        // component: Shared Folder Navigator
        SfnComponent csfn;
        // component: User Data File
        UDFComponent cudf;
        // component: Presence Aura
        PresenceComponent cpca;

        static int WsaStartup();

        void StartAuraThreadIn();
        void StartAuraThreadOut();
        void StartMessageReceivingThread();
        //void StartRequestReceivingThread();
        void StartRequestReceivingThread20();

        bool stupidThreadsDie = false;
    };
}
