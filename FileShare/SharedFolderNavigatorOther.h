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
        RequestListener(USHORT port);
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
        RequestSender(Sender&& other);
        RequestSender(RequestReceiver&& other);

        RequestSender(const String& addr, USHORT port);
        RequestSender(ULONG addr, USHORT port);
        Int ConnectToUser();
        Int SendRequest(const String& request);
    };
    #endif 
    
    #ifndef FILE_EXCHANGE
    class FileListener :
        public Listener
    {
    public:
        FileListener(USHORT port);
    };
    class FileReceiver :
        public Receiver
    {
        using Receiver::ReceiveMessage;
        FileReceiver();
    public:
        FileReceiver(Receiver&& target);
        FileReceiver& operator = (Receiver&& other);

        Int ReceiveFile(const String& filePath);
    };
    class FileSender :
        public Sender
    {
        using Sender::SendMessageToUser;
    public:
        FileSender(ULONG addr, USHORT port = sharePort);
        FileSender(const String& addr, USHORT port = sharePort);

        Int ConnectToUser();
        Int SendFile(const String& filePath, Int chunkSize = 4);
    };
    #endif

    class SharedFolderNavigatorOther :
        protected BasicSharedFolderNavigator
    {
    public:  
        SharedFolderNavigatorOther();
        SharedFolderNavigatorOther(USHORT reqPort, USHORT fshPort);

        FileVector RequestFileList(const String& addr, USHORT port = requestPort);

        //int request file
        Int SendMyFileList(const FileVector&, const String& addr, USHORT port = requestPort);

        Int SendFileToUser(const String& filePath, UINT chunkSize, const String& addr, USHORT port = sharePort);

        RequestListener requestListener;
        FileListener fileListener;

        friend class SharedFolderNavigatorComponent;
        friend class Model;
    };
}