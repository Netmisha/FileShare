#pragma once

#include "BasicSharedFolderNavigator.h"
#include "TcpSocketedEntity.h"

namespace FileShare
{
    #define requestPort20 (USHORT)0xabcd

    class SharedFolderNavigatorOther20 :
        protected BasicSharedFolderNavigator
    {
    public:
        Listener requestListener;
        
        SharedFolderNavigatorOther20();
        SharedFolderNavigatorOther20(USHORT port);

        //String ReceiveRequest();

        //Int SendFileListTo(Sender& target);
       // Int SendFileTo(Sender& target, const String& filePath, Int chunkSize);

        //Int RequestFileList(Sender& target);
        //Int RequestFile(Sender& target);

        String RequestAndReceiveFileList(Sender&);
        Int RequestAndReceiveFile(Sender&, const String& fileName);
        Int RequestReceivingAndSendFile(Sender&, const String fileName);

        Int SendFile(Sender&, const String& fileName, Int chunkSize);
        Int RecvFile(Receiver&, const String& fileName, Int chunkSize);
    };
};