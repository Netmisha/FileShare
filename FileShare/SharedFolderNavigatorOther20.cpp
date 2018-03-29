#include "SharedFolderNavigatorOther20.h"

#include "Logger.h"
#include <chrono>
#include <thread>
#include <fstream>
#include <Windows.h>

using IfStream = std::ifstream;
using OfStream = std::ofstream;
using Ios = std::ios;
using Double = double;
using Clock = std::chrono::system_clock;
using TimePoint = Clock::time_point;
using Seconds = std::chrono::seconds;
using Thread = std::thread;

using namespace FileShare;
SharedFolderNavigatorOther20::SharedFolderNavigatorOther20():
    SharedFolderNavigatorOther20(requestPort20)
{}

SharedFolderNavigatorOther20::SharedFolderNavigatorOther20(USHORT port):
    requestListener(port)
{
    Log::InRed("Sfno20()->");
    __Begin;
    {
        requestListener.Bind();
        requestListener.Listen();
    }
    __End;
    Log::InRed("Sfno20();");
}

namespace Request
{
    const String recvMyFile = "RECVMYFILE";
    const String fileLstPls = "FILELISTPLS";
    const String sendMeFile = "SENDMEFILE";
    const String noSuchFile = "NOSUCHFILE";
};



String SharedFolderNavigatorOther20::RequestAndReceiveFileList(Sender& rs)
{
    String result;
    Log::InRed("RequestAndReceiveFileList");
    __Begin;
    {
        Int sendRequestResult = SOCKET_ERROR;
        for (TimePoint start = Clock::now(); Clock::now() - start < Seconds(3);)
        {
            sendRequestResult = rs.SendMessageToUser(Request::fileLstPls);
            if (sendRequestResult != SOCKET_ERROR)
                break;
        }
        if (sendRequestResult == SOCKET_ERROR)
        {
            result = "connect timeout";
        }
        else /* request sent alright */
        {
            Receiver rc(std::move(rs));
            while(result.empty())
                result = rc.ReceiveMessage();
        } 
    }
    __End;
    return result;
}

Int SharedFolderNavigatorOther20::RequestSendingAndReceiveFile(Sender& rs, const String& fileName, const String& recvPath)
{
    Int recvFileResult = SOCKET_ERROR;
    Log::InRed("RequestAndReceiveFile()->");
    __Begin;
    {
        Int sendRequestResult = SOCKET_ERROR;
        {
            for (TimePoint start = Clock::now(); Clock::now() - start < Seconds(3);)
            {
                sendRequestResult = rs.SendMessageToUser(Request::sendMeFile + fileName);
                if (sendRequestResult != SOCKET_ERROR)
                    break;
            }
        }
        if (sendRequestResult != SOCKET_ERROR)
        {
            //TCPSocketedEntity& se = static_cast<TCPSocketedEntity&>(rs);
            auto thFileRecvFun = [this, &rs, recvPath]()
            {
                Receiver rc(std::move(rs));
                Int recvFileResult = RecvFile(rc, recvPath);
                if (recvFileResult != SOCKET_ERROR)
                {
                    Log::InRed("recv()+ " + recvPath);
                    Beep(523, 500);
                }
            };
            Thread recvThread{ thFileRecvFun };
            recvThread.detach();
            std::this_thread::sleep_for(Seconds(1));

            recvFileResult = TRUE;
        }
    }
    __End;
    Log::InRed("RequestAndReceiveFile();");
    return recvFileResult;
}

Int SharedFolderNavigatorOther20::RequestReceivingAndSendFile(Sender& fs, const String& fileName, const String& filePath)
{
    Int resultOfSendFile = SOCKET_ERROR;
    Log::InRed("RequestReceivingAndSendFile()->");
    __Begin;
    {
        Int sendRequestResult = SOCKET_ERROR;
        {
            for (TimePoint start = Clock::now(); Clock::now() - start < Seconds(3);)
            {
                sendRequestResult = fs.SendMessageToUser(Request::recvMyFile + fileName);
                if (sendRequestResult != SOCKET_ERROR)
                    break;
            }
        }

        Receiver rc = std::move(fs);
        String msg;
        while (msg.empty())
            msg=rc.ReceiveMessage();

        if (sendRequestResult != SOCKET_ERROR)
        {
            auto thFileSendFun = [this, &rc, filePath]()
            {
                Sender se = std::move(rc);
                if (SendFile(se, filePath) != -1)
                {
                    Log::InRed("send()+ " + filePath);
                    Beep(523, 500);
                }
            };
            Thread sendThread{ thFileSendFun };
            sendThread.detach();
            std::this_thread::sleep_for(Seconds(1));
            resultOfSendFile = TRUE;
        }
    }
    __End;
    Log::InRed("RequestReceivingAndSendFile();");
    return resultOfSendFile;
}
template <class Func>
Int PcharToSocketStreamData(SOCKET sc,PCHAR buff,Int size,Int step, Func fun)
{
    Int result = SOCKET_ERROR;

    PCHAR ptr = buff;
    PCHAR end = buff + size;
    while (Int diff = end - ptr)
    {
        Int chunk = diff > step ? step : diff;
        result = fun(sc, ptr, chunk, NULL);
        if (result == SOCKET_ERROR)
        {
            Log::InRed("RecvFile() -");
        }
        else if (result > 0)
        {
            ptr += result;

            String progress;
            {
                Double percent = (Double)(ptr - buff) / size * 100;
                String prc = std::to_string(percent);
                progress = prc.substr(0, prc.length() - 5) + "%";
                Log::InRed("chunk recv:" + progress);
            }
        }
    }
    return result;
}

Int SharedFolderNavigatorOther20::SendFile(Sender&fs, String const& filePath)
{
    Int sendResult = SOCKET_ERROR;
    Log::InRed("SendFile()->");
    __Begin;
    {
        Log::InRed("ifstream(file);");
        IfStream file(filePath, Ios::in | Ios::binary);
        if (file.is_open())
        {
            Log::InRed("{" + filePath + "}.open +, buffing up");
            ULONG fileSize;
            {
                auto end = file.seekg(0, file.end).tellg();
                auto beg = file.seekg(0, file.beg).tellg();
                fileSize = end - beg;
            }
            if (fileSize == 0)
                return 0;

            ULONG chunkSize;
            {
                chunkSize = fileSize;
                if (fileSize > 20)
                    chunkSize /= 20;
                //chunkSize = chunkSize > 1024 ? 1024 : chunkSize;
            }
            Log::InRed("fileSize: " + std::to_string(fileSize));
            
            ULONG sizes[] = { fileSize, chunkSize };            
            Int sendSizesResult = SOCKET_ERROR;
            while (sendSizesResult == SOCKET_ERROR)
            {
                sendSizesResult = send(fs.sc, (PCHAR)sizes, sizeof(sizes), NULL);
            }

            if (PCHAR fileBuff = new CHAR[fileSize])
            {
                file.read(fileBuff, fileSize);

                PCHAR ptr = fileBuff;
                PCHAR end = fileBuff + fileSize;
                Int step = chunkSize;
                while (Int diff = end - ptr)
                {
                    Int chunk = diff > step ? step : diff;
                    sendResult = send(fs.sc, ptr, chunk, NULL);
                    Log::InRed(String(ptr, ptr + chunk));

                    if (sendResult == SOCKET_ERROR)
                    {
                        Log::InRedWithError("SendFile() -, error: ");
                        break;
                    }
                    else
                    {
                        ptr += sendResult ? sendResult : chunk;

                        String progress;
                        {
                            Double percent = (Double)(ptr - fileBuff) / fileSize * 100;
                            progress = std::to_string(percent);
                            Log::InRed("chunk sent:" + progress.substr(0, progress.length() - 5) + "%");
                        }
                    }
                }
                delete[] fileBuff;
            }
            else
            {
                Log::InRed("buff = new() -");
                //sendResult *= 3;
            }
            Log::InRed("file.close()");
            file.close();
        }
        else
        {
            Log::InRed("ifstream() -");
            //sendResult *= 2;
        }
    }
    __End;
    Log::InRed("SendFile();");
    return sendResult;
}
Int SharedFolderNavigatorOther20::RecvFile(Receiver& rc, String const& filePath)
{
    Int recvResult = SOCKET_ERROR;
    Log::InRed("RecvFile()->");
    __Begin;
    {
        Log::InRed("ofstream(file)");
        OfStream file(filePath, Ios::out | Ios::binary);
        if (file.is_open())
        {
            Log::InRed("file.open()+");

            ULONG sizes[2];
            Int recvSizesResult = SOCKET_ERROR;
            while (recvSizesResult == SOCKET_ERROR)
            {
                Log::InRed("recv(sizes)");
                recvSizesResult = recv(rc.sc, (PCHAR)sizes, sizeof(sizes), NULL);
            }
            ULONG fileSize = sizes[0];
            ULONG chunkSize = sizes[1];

            if (PCHAR fileBuff = new CHAR[fileSize])
            {
                Log::InRed("fileBuff = new()+");

                PCHAR ptr = fileBuff;
                PCHAR end = fileBuff + fileSize;
                Int step = chunkSize;
                while (Int diff = end - ptr)
                {
                    Int chunk = diff > step ? step : diff;
                    recvResult = recv(rc.sc, ptr, chunk, NULL);
                    Log::InRed(String(ptr, ptr + chunk));

                    if (recvResult == SOCKET_ERROR)
                    {
                        Log::InRed("RecvFile() -");
                    }
                    else
                    {
                        ptr += recvResult ? recvResult : chunk;

                        String progress;
                        {
                            Double percent = (Double)(ptr - fileBuff) / fileSize * 100;
                            String prc = std::to_string(percent);
                            progress = prc.substr(0, prc.length() - 5) + "%";
                            Log::InRed("chunk recv:" + progress);
                        }
                    }
                }
                file.write(fileBuff, fileSize);
                delete[] fileBuff;
            }
            else
            {
                Log::InRed("buff = new() -");
                recvResult *= 3;
            }
            Log::InRed("file.close()");
            file.close();
        }
        else
        {
            Log::InRed("ofstream() -");
            recvResult *= 2;
        }
    }
    __End;
    Log::InRed("RecvFile();");
    return recvResult;
}

