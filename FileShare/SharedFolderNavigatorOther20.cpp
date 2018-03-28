#include "SharedFolderNavigatorOther20.h"

#include "Logger.h"

#include <chrono>

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
    const String bcStandard = "LOLKEK";
    const String recvMyFile = "RECVMYFILE";
    const String fileLstPls = "FILELISTPLS";
    const String sendMeFile = "SENDMEFILE";
    const String noSuchFile = "NOSUCHFILE";
};

using Clock = std::chrono::system_clock;
using TimePoint = Clock::time_point;
using Seconds = std::chrono::seconds;

String SharedFolderNavigatorOther20::RequestAndReceiveFileList(Sender& rs)
{
    String result;
    Log::InRed("RequestAndReceiveFileList");
    __Begin;
    {
        Int sendResult = SOCKET_ERROR;
        /*for (TimePoint start = Clock::now(); Clock::now() - start < Seconds(3);)
        {*/
            sendResult = rs.SendMessageToUser(Request::fileLstPls);
           /* if (sendResult != SOCKET_ERROR)
                break;
        }
        if (sendResult == SOCKET_ERROR)
        {
            result = "connect timeout";
        }
        else
        {*/
            Receiver rc(std::move(rs));
            
            /*while (result.empty())*/
                result = rc.ReceiveMessage();

    //    } 
    }
    __End;
    return result;
}

Int SharedFolderNavigatorOther20::RequestAndReceiveFile(Sender &, const String & fileName)
{
    return Int();
}

Int FileShare::SharedFolderNavigatorOther20::RequestReceivingAndSendFile(Sender &, const String fileName)
{
    return Int();
}

