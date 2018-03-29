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

        String RequestAndReceiveFileList(Sender& requestTarget);

        Int RequestSendingAndReceiveFile(Sender& target, const String& fileName, const String& recvPath);
        Int RequestReceivingAndSendFile(Sender& target, const String& fileName, const String& filePath);

        Int SendFile(Sender& target, const String& filePath);
        Int RecvFile(Receiver& target, const String& filePath);
    };
};