#pragma once

#include "MessengerComponent.h"
#include "SharedFolderNavigatorComponent.h"
#include "UserDataFileComponent.h"
#include "PresenceComponent.h"

namespace FileShare 
{
    class Model
    {
        friend class StagedController;
        friend class ConsoleController;
        friend int main(); 
    public:
        Model();
       ~Model();
    protected:        
        Model(USHORT messengerPort, 
              USHORT sharedFolderRequestPort, 
              USHORT presenceComponentBroadcastPort);
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

        void StartAuraBroadcastingThread();
        void StartAuraBroadcastReceivingThread();
        void StartMessageReceivingThread();
        void StartRequestReceivingThread20();

        // this variable telss thread loops to stop
        bool stupidThreadsDie = false;
    };
}
