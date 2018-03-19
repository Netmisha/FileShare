#include "Logger.h"
#include "SharedFolderNavigatorOther.h"

#include <fstream>
#include <regex>
#include <iostream>

using namespace FileShare;
using namespace Log;

using Ifstream = std::ifstream;
using Ofstream = std::ofstream;
using Ios = std::ios;

constexpr UINT fileChunk = 8;//b
#ifndef FS_STRING_HELPERS
String JoinString(const FileVector& list, const String& del) {
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
#endif

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

FileListener::FileListener(USHORT port):
    Listener(port)
{
    {
        #ifdef LOGGER
        ++Log::depth;
        Log::InRed("FileListener()");
        --Log::depth;
        #endif
    }
}

FileReceiver::FileReceiver(Receiver &&other):
    Receiver(TCPSocketedEntity(other.sc, other.addr.sin_addr.S_un.S_addr, ntohs(addr.sin_port)))
{
    other.sc = INVALID_SOCKET;

    {
        #ifdef LOGGER
        ++Log::depth;
        Log::InRed("FileReceiver() moved");
        --Log::depth;
        #endif
    }
}

FileReceiver &FileReceiver::operator=(Receiver && other)
{
    sc = other.sc;
    addr = other.addr;

    other.sc = INVALID_SOCKET;

    {
        #ifdef LOGGER
        ++Log::depth;
        Log::InRed("FR move-assigned");
        --Log::depth;
        #endif
    }

    return *this;
}

FileSender::FileSender(ULONG addr, USHORT port) :
    Sender(addr, port)
{
    {
        #ifdef LOGGER
        ++Log::depth;
        Log::InRed("FileSender()");
        --Log::depth;
        #endif
    }
}
FileSender::FileSender(const String& addr, USHORT port) :
    FileSender(inet_addr(addr.c_str()), port)
{}

Int FileReceiver::ReceiveFile(const String& filePath)
{
    Int result = SOCKET_ERROR;
    {
        #ifdef LOGGER
        ++Log::depth;
        Log::InRed("FileReceiver.ReceiveFile {");
        Log::InRed("opening file");
        #endif
    }
    Ofstream file(filePath, Ios::out | Ios::binary);

    if (file.is_open()) 
    {
        INT sizes[2];

        while (result = recv(sc, (PCHAR)sizes, sizeof(sizes), NULL) == -1);

        INT fileSize = sizes[0];
        INT chunkSize = sizes[1];
        //{
        //    /*String preSendMessage = ReceiveMessage();

        //    auto v = SplitString(preSendMessage, ";");
        //    fileSize = std::stoi(v[0]);
        //    chunkSize = std::stoi(v[1]);*/

        //    {
        //        #ifdef LOGGER
        //        Log::InRed("recved preSM: <" 
        //                   + std::to_string(fileSize) + ";" 
        //                   + std::to_string(chunkSize) + ">");
        //        #endif
        //    }
        //}
     
        if (PCHAR fileBuff = new CHAR[fileSize]) 
        {
            PCHAR ptr = fileBuff;
            PCHAR end = fileBuff + fileSize;
            INT step = chunkSize;
            while (INT diff = end - ptr) 
            {
                INT chunk = diff > step ? step : diff;

                result = recv(sc, ptr, chunk, NULL);

                for (int i = 0; i < chunk; ++i)
                std::cout << ptr[i];

                if (result == SOCKET_ERROR) {
                    {
                        #ifdef LOGGER
                        Log::InRed("ReceiveFile failed");
                        #endif // 

                    }
                    break;
                }
                else if (result > 0)
                {
                    ptr += chunk;
                    {
                        #ifdef LOGGER
                        Log::InRed("chunk recved");
                        #endif 
                    }
                }
                /*else if (result == 0) {
                    std::cout << chunk << " " << diff << " " << step;
                    if (diff <= step)
                        break;
                }*/
            }

            file.write(fileBuff, fileSize);        
            delete[] fileBuff;
        } 
        file.close();
    }
    else
    {
        result = -2;
        {
            #ifdef LOGGER
            Log::InRed("ofstream() failed");
            #endif 
        }
    }
    {
        #ifdef LOGGER
        Log::InRed("} FileReceiver.ReceiveFile");
        --Log::depth;
        #endif 
    }
    return result;

    //Int result = SOCKET_ERROR;
    //{
    //    #ifdef LOGGER
    //    ++Log::depth;
    //    Log::InRed("FileReceiver.ReceiveFile{");
    //    Log::InRed("opening file");
    //    #endif
    //}
    //Ofstream file(filePath, Ios::out | Ios::binary);

    //if (!file.is_open())
    //{
    //    result = -2; // HFILE_ERROR == SOCKET_ERROR
    //    {
    //        #ifdef LOGGER
    //        Log::InRed("ofstream() failed");
    //        #endif 
    //    }
    //}

    //else
    //{ 
    //    {
    //        #ifdef LOGGER
    //        Log::InRed("start recv");
    //        #endif 
    //    }
    //    CHAR buff[fileChunk]{};
    //    while (result = recv(sc, buff, sizeof(buff), NULL)) 
    //    {
    //        if (result == SOCKET_ERROR) 
    //        {
    //            {
    //                #ifdef LOGGER
    //                Log::InRed("recv failed");
    //                #endif // 

    //            }
    //            break;
    //        }
    //        else
    //        {
    //            {
    //                #ifdef LOGGER
    //                Log::InRed("writing recv chunk to file");
    //                #endif // 

    //            }
    //            file.write(buff, result);
    //        }
    //    }
    //    {
    //        #ifdef LOGGER
    //        Log::InRed("doney recv'ing");
    //        #endif // 

    //    }
    //}
    //{
    //    #ifdef LOGGER
    //    Log::InRed("}FileReceiver.ReceiveFile");
    //    --Log::depth;
    //    #endif 
    //}
    //return result;
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
        Log::InRed("FileSender done connecting");
        --Log::depth;
    }
    #endif

    return result;
}

Int FileSender::SendFile(const String & filePath, Int chunkSize){
    Int result = SOCKET_ERROR;
    {
        #ifdef LOGGER
        ++Log::depth;
        Log::InRed("FileSender.SendFile {");
        ++Log::depth;
        Log::InRed("opening file");
        #endif
    }
    Ifstream file(filePath, Ios::in | Ios::binary);

    if (file.is_open()) 
    {
        {
            #ifdef LOGGER
            Log::InRed("file <" + filePath + "> opened, buffing it up");
            #endif
        }
        
        INT fileSize;
        {
            Int end = file.seekg(0, file.end).tellg();
            INT beg = file.seekg(0, file.beg).tellg();
            fileSize = end - beg;
            {
                #ifdef LOGGER
                Log::InRed("fileSize: " + std::to_string(fileSize));
                #endif
            }
        }
        /*String preSendMessage = std::to_string(fileSize) + ";" + std::to_string(chunkSize);
        SendMessageToUser(preSendMessage);
        {
            #ifdef LOGGER
            Log::InRed("premessage: {" + preSendMessage + "}");
            #endif
        }
        */

        INT sizes[] = { fileSize, chunkSize };

        while(result = send(sc, (PCHAR)sizes, sizeof(sizes), NULL) == -1);

        if(PCHAR fileBuff = new CHAR[fileSize])
        {
            file.read(fileBuff, fileSize);          
            {
                #ifdef LOGGER
                Log::InRed("file is buffed. attepting send");
                #endif
            }

            PCHAR ptr = fileBuff;
            PCHAR end = fileBuff + fileSize;
            INT step = chunkSize;
            while(INT diff = end - ptr)
            {
                INT chunk = diff > step ? step : diff;
                
                result = send(sc, ptr, chunk, NULL);

                for (int i = 0; i < chunk; ++i)
                    std::cout << ptr[i];

                if (result == SOCKET_ERROR) {
                    {
                        #ifdef LOGGER
                        Log::InRedWithError("SendFile failed, error: ");
                        #endif
                    }
                    break;
                }
                else if(result > 0)
                {
                    ptr += chunk;
                    {
                        #ifdef LOGGER
                        Log::InRed("chunk sent");
                        #endif 
                    }
                }
            }
            delete[] fileBuff;
        }
        file.close();
    }
    else
    {
        result = -2; // HFILE_ERROR == SOCKET_ERROR
        {
            #ifdef LOGGER
            Log::InRed("ifstream() failed");
            #endif 
        }        
    }
    {
        #ifdef LOGGER
        Log::InRed("closing file");
        Log::InRed("} FileSender.SendFile");
        --Log::depth;
        #endif
    }
    return result;


    //Int result = SOCKET_ERROR;
    //{
    //    #ifdef LOGGER
    //    ++Log::depth;
    //    Log::InRed("FileSender.SendFile{");
    //    ++Log::depth;
    //    Log::InRed("opening file");
    //    #endif
    //}
    //Ifstream file(filePath, Ios::in | Ios::binary);
    //if (!file.is_open()){
    //    result = -2; // HFILE_ERROR == SOCKET_ERROR
    //    {   
    //        #ifdef LOGGER
    //        Log::InRed("ifstream() failed");
    //        #endif 
    //    }
    //}
    //else{
    //    {
    //        #ifdef LOGGER
    //        Log::InRed("file opened, trying to send");
    //        #endif
    //    }
    //    for(;;) 
    //    {
    //        if (file.eof()) {
    //            {
    //                #ifdef LOGGER
    //                Log::InRed("FileSender probably succeded");
    //                #endif 
    //            }

    //            result = 1;
    //            break;
    //        } 
    //        else
    //        {
    //            CHAR buff[fileChunk];
    //            
    //            file.read(buff, sizeof(buff));

    //            result = send(sc, buff, sizeof(buff), NULL);

    //            if (result == SOCKET_ERROR) {  
    //                {
    //                    #ifdef LOGGER
    //                    Log::InRedWithError("FileSender failed, error: ");
    //                    #endif
    //                }
    //                break;
    //            }
    //            else if (result != 0) {
    //                {
    //                    #ifdef LOGGER
    //                    Log::InRed("chunk sent");
    //                    #endif 
    //                }
    //            }
    //        } 
    //    }
    //    file.close();
    //}

    //{
    //    #ifdef LOGGER
    //    --Log::depth;
    //    Log::InRed("}FileSender.SendFile");
    //    --Log::depth;
    //    #endif 
    //}
    //return result;
}

SharedFolderNavigatorOther::SharedFolderNavigatorOther() :
    requestListener(requestPort),
    fileListener(sharePort)
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
    FileSender fs(addr, port);
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

