#pragma once

#include "BasicSharedFolderNavigator.h"

#include "TcpSocketedEntity.h"

namespace FileShare {

    #define sharePort       (USHORT)0xf15e
    #define requestPort     (USHORT)0xf112

    #ifndef REQUEST_EXCHANGE
    class RequestListener :
        public Listener
    {
    public:
        RequestListener(Listener&& target);
    };
    class RequestReceiver :
        public Receiver
    {
        using Receiver::ReceiveMessage;
    public:
        RequestReceiver(Receiver&& target);
        String ReceiveRequest();
    };
    class RequestSender:
        public Sender
    {
        using Sender::SendMessageToUser;
    public:
        RequestSender(Sender&& target);
        Int ConnectToUser();
        Int SendRequest(const String& request);
    };
    #endif 
    
    #ifndef FILE_EXCHANGE
    class FileListener :
        public Listener
    {
    public:
        FileListener(Listener&&target);
    };
    class FileReceiver :
        public Receiver
    {
        using Receiver::ReceiveMessage;
    public:
        FileReceiver(Receiver&& target);
        Int ReceiveFile(const String& filePath);
    };
    class FileSender :
        public Sender
    {
        using Sender::SendMessageToUser;
    public:
        FileSender(Sender&& target);
        Int ConnectToUser();
        Int SendFile(const String& filePath);
    };
    #endif

    class SharedFolderNavigatorOther :
        protected BasicSharedFolderNavigator
    {
    public:  
        SharedFolderNavigatorOther();

        FileVector RequestFileList(const String& addr, USHORT port = requestPort);
        Int SendMyFileList(const FileVector&, const String& addr, USHORT port = requestPort);

        Int SendFileToUser(const String& filePath, const String& addr, USHORT port = sharePort);
        Int ReceiveFileFromUser(const String& filePath);

    protected:
        RequestListener requestListener;
        FileListener fileListener;

        friend class SharedFolderNavigatorComponent;
    };
}