#include "Logger.h"
#include "SharedFolderNavigatorOther.h"

#include <fstream>

using namespace FileShare;
using namespace Log;

using Ifstream = std::ifstream;
using Ofstream = std::ofstream;
using Ios = std::ios;

constexpr UINT fileChunk = 16;//b

RequestListener::RequestListener(Listener && target) :
    Listener(std::move(target))
{
    {
        #ifdef LOGGER
        ++Log::depth;
        Log::InRed("RequestListener()");
        --Log::depth;
        #endif
    }
}
RequestReceiver::RequestReceiver(Receiver&& target) :
    Receiver(std::move(target))
{
    {
    #ifdef LOGGER
    ++Log::depth;
    Log::InRed("RequestReceiver created");
    --Log::depth;
    #endif
    }
}
String RequestReceiver::ReceiveRequest()
{
    {
        #ifdef LOGGER
        ++Log::depth;        
        Log::InRed("RequestReceiver trying to recv");
        #endif
    }

    String request = Receiver::ReceiveMessage();

    {
        #ifdef LOGGER
            IF(request.empty())
                Log::InRed("request received empty");
            ELSE
                Log::InRed("request received");
            --depth;
        #endif
    }

    return request;
}
RequestSender::RequestSender(Sender && target):
    Sender(std::move(target))
{
    {
        #ifdef LOGGER
        ++Log::depth;
        Log::InRed("RequestReceiver created");
        --Log::depth; 
        #endif
    }  
}
Int RequestSender::ConnectToUser()
{
    Int result = SOCKET_ERROR;
    
    {
        #ifdef LOGGER   
        ++Log::depth;
        Log::InRed("RequestSender trying to connect");
        #endif
    }

    result = Sender::ConnectToUser();

    {
        #ifdef LOGGER
        Log::InRed("RequestSender done");
        --Log::depth;
        #endif
    }

    return result;
}
Int RequestSender::SendRequest(const String & request)
{
    Int result = SOCKET_ERROR;

    {
        #ifdef LOGGER
        ++Log::depth;
        InRed("ReqSend trying to send request");
        #endif
    }

    result = Sender::SendMessageToUser(request);

    {
        #ifdef LOGGER
        InRed("ReqSend done");
        --Log::depth;
        #endif
    }

    return result;
}

FileListener::FileListener(Listener && target):
    Listener(std::move(target))
{
    {
        #ifdef LOGGER
        ++Log::depth;
        Log::InRed("FileListener()");
        --Log::depth;
        #endif
    }
}
FileReceiver::FileReceiver(Receiver && target):
    Receiver(std::move(target))
{
    {
        #ifdef LOGGER
        ++Log::depth;
        Log::InRed("FileReceiver()");
        --Log::depth;
        #endif
    }
}
FileSender::FileSender(Sender && target):
    Sender(std::move(target))
{
    {
        #ifdef LOGGER
        ++Log::depth;
        Log::InRed("FileSender created");
        #endif
    }
}
Int FileReceiver::ReceiveFile(const String& filePath)
{
    Int result = SOCKET_ERROR;
    {
        #ifdef LOGGER
        ++Log::depth;
        Log::InRed("FileReceiver.ReceiveFile{");
        Log::InRed("opening file");
        #endif
    }
    Ofstream file(filePath, Ios::out | Ios::binary);

    if (!file.is_open())
    {
        result = -2; // HFILE_ERROR == SOCKET_ERROR
        {
            #ifdef LOGGER
            Log::InRed("ofstream() failed");
            #endif 
        }
    }
    else
    { 
        {
            #ifdef LOGGER
            Log::InRed("start recv");
            #endif 
        }
        CHAR buff[fileChunk]{};
        while (result = recv(sc, buff, sizeof(buff), NULL)) 
        {
            if (result == SOCKET_ERROR) 
            {
                {
                    #ifdef LOGGER
                    Log::InRed("recv failed");
                    #endif // 

                }
                break;
            }
            else
            {
                {
                    #ifdef LOGGER
                    Log::InRed("writing recv chunk to file");
                    #endif // 

                }
                file.write(buff, result);
            }
        }
        {
            #ifdef LOGGER
            Log::InRed("doney recv'ing");
            #endif // 

        }
    }
    {
        #ifdef LOGGER
        Log::InRed("}FileReceiver.ReceiveFile");
        --Log::depth;
        #endif 
    }
    return result;
}
Int FileSender::ConnectToUser()
{
    Int result = SOCKET_ERROR;

    #ifdef LOGGER
    {
        ++Log::depth;
        Log::InRed("FileSender trying to connect");
    }
    #endif

    result = Sender::ConnectToUser();

    #ifdef LOGGER
    {
        Log::InRed("FileSender done");
        --Log::depth;
    }
    #endif

    return result;
}
Int FileSender::SendFile(const String & filePath){
    Int result = SOCKET_ERROR;
    {
        #ifdef LOGGER
        ++Log::depth;
        Log::InRed("FileSender.SendFile{");
        Log::InRed("opening file");
        #endif
    }
    Ifstream file(filePath, Ios::in | Ios::binary);
    if (!file.is_open()){
        result = -2; // HFILE_ERROR == SOCKET_ERROR
        {   
            #ifdef LOGGER
            Log::InRed("ifstream() failed");
            #endif 
        }
    }
    else{
        {
            #ifdef LOGGER
            Log::InRed("file opened, trying to send");
            #endif
        }
        while (!file.eof()) {
            CHAR buff[fileChunk];
            file.read(buff, sizeof(buff));
            result = Sender::SendMessageToUser(buff);
            if (result < 0) {
                break;
                {
                    #ifdef LOGGER
                    Log::InRedWithError("FileSender failed, error: ");
                    #endif
                }
            }
        }
        {
            #ifdef LOGGER
            Log::InRed("FileSender probably succeded");
            #endif 
        }
    }
    {
        #ifdef LOGGER
        Log::InRed("}FileSender.SendFile");
        --Log::depth;
        #endif 
    }
    return result;
}

String JoinString(const FileVector& list, const String& del){
    String str;
    for (const String& s : list)
        str += s + del;
    return str;
}
FileVector SplitString(const String& str, const String& del) {
    FileVector list;

    String::size_type pos = 0;
    String::size_type pre = 0;

    while ((pos = str.find(del, pre)) != String::npos) {
        list.push_back(str.substr(pre, pos));
        pre = pos + 1;
    }

    if (pre != String::npos)
        list.push_back(str.substr(pre));

    return list;
}

SharedFolderNavigatorOther::SharedFolderNavigatorOther() :
    requestListener(std::move(Listener(requestPort))),
    fileListener(std::move(Listener(sharePort)))
{
    {
        #ifdef LOGGER
        ++Log::depth;
        Log::InRed("SFNO{");
        #endif
    }

    requestListener.Bind();
    requestListener.Listen();

    {
        #ifdef LOGGER
        --Log::depth;
        Log::InRed("}SFNO");
        #endif
    }  
}

FileVector SharedFolderNavigatorOther::RequestFileList(const String & addr, USHORT port)
{
    String request("%file%list%pls%");
    FileVector fileList;

    {
        #ifdef LOGGER
        ++Log::depth;
        Log::InRed("SFNO.RequestFileList{");
        #endif
    }

    { 
        #ifdef LOGGER
            Log::InRed("create RequestSender");    
        #endif
    }

    RequestSender rs(std::move(Sender(addr, port)));

    {
        #ifdef LOGGER
        Log::InRed("sending request");
        #endif
    }

    Int requestResult = rs.SendRequest(request);

    if (requestResult < 0) {
        {
            #ifdef LOGGER
            Log::InRed("apparently request failed");
            #endif
        }
    }
    else
    {
        {
            #ifdef LOGGER
            Log::InRed("attempting to convert RequestSender->Receiver");
            #endif
        }

        Receiver rc(std::move((TCPSocketedEntity&)rs));
        
        {
            #ifdef LOGGER
            Log::InRed("Receiver trying to recv");
            #endif
        }

        String msg = rc.ReceiveMessage();

        if (!msg.empty()) {

            {
                #ifdef LOGGER
                Log::InRed("we got smth, trying to split");
                #endif
            }

            fileList = SplitString(msg, ";");
        }
        else
        {
            {
                #ifdef LOGGER
                Log::InRed("we recvd shit");
                #endif
            }
        }
    }

    {
        #ifdef LOGGER
        Log::InRed("}SFNO.RequestFileList");
        --Log::depth;
        #endif
    }

    return fileList;
}

Int SharedFolderNavigatorOther::SendMyFileList(const FileVector& list, const String & addr, USHORT port)
{
    String request("%my%file%list%");
    Int result = SOCKET_ERROR;
    {
        #ifdef LOGGER
        ++Log::depth;
        Log::InRed("SFNO.SendMyFileList{");
        #endif
    }
    {
        #ifdef LOGGER
        Log::InRed("creating RequestSender");
        #endif
    }
    RequestSender sd(std::move(Sender(addr, port)));
    if (!sd.InvalidSocket()) {
        {
            #ifdef LOGGER
            Log::InRed("connecting RequestSender");
            #endif
        }
        result = sd.ConnectToUser();
        if (result != SOCKET_ERROR) {
            {
                #ifdef LOGGER
                Log::InRed("RequestSender connected");
                #endif
            }
            String myList = JoinString(list, ";");
            {
                #ifdef LOGGER
                Log::InRed("RequestSender trying to send");
                #endif
            }
            result = sd.SendRequest(request + myList);
            {
                #ifdef LOGGER
                IF (result > 0) 
                    Log::InRed("RequestSender sent");
                ELIF(result == 0)
                    Log::InRedWithError("RequestSender sent 0");
                ELSE
                    Log::InRedWithError("RequstSender fail");
                #endif
            }
        }
        else{
            {
                #ifdef LOGGER
                Log::InRed("connect fail");
                #endif
            }
        }
    }
    else {
        result = sd.sc;

        {
            #ifdef LOGGER
            Log::InRed("creating RequestSender fail");
            #endif
        }
    }
    {
        #ifdef LOGGER
        Log::InRed("}SFNO.SendMyFileList");
        --Log::depth;
        #endif
    }
    return result;
}

Int SharedFolderNavigatorOther::SendFileToUser(const String & filePath, const String & addr, USHORT port)
{
    Int result = SOCKET_ERROR;
    {
        #ifdef LOGGER
        ++Log::depth;
        Log::InRed("SFNO.SendFileToUser{");
        #endif
    }
    {
        #ifdef LOGGER
        Log::InRed("creating FileSender");
        #endif
    }
    FileSender fs(std::move(Sender(addr, port)));
    if (!fs.InvalidSocket()) {
        {
            #ifdef LOGGER
            Log::InRed("connecting FileSender");
            #endif
        }
        result = fs.ConnectToUser();
        if (result != SOCKET_ERROR) {
            {
                #ifdef LOGGER
                Log::InRed("FileSender connected");
                #endif
            }
            result = fs.SendFile(filePath);
        }
        else {
            {
                #ifdef LOGGER
                Log::InRed("FileSender connect fail");
                #endif
            }
        }
    }
    else {
        result = fs.sc;
        {
            #ifdef LOGGER
            Log::InRed("creating FileSender fail");
            #endif
        }
    }
    {
        #ifdef LOGGER
        Log::InRed("}SFNO.SendFileToUser");
        --Log::depth;
        #endif
    }
    return result;
}

Int SharedFolderNavigatorOther::ReceiveFileFromUser(const String & filePath)
{
    Int result = SOCKET_ERROR;

    /*
    for tomorrow
    */


    return result;
}

