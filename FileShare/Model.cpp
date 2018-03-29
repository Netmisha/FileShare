#include "Logger.h"
#include "Model.h"
using namespace FileShare;

#include <regex>
using Regex = std::regex;

#include <thread>
using Thread = std::thread;

#include <chrono>
using Duration = std::chrono::duration<Int>;
using Seconds = std::chrono::seconds;
using MSeconds = std::chrono::milliseconds;

#include <vector>
#include <utility>
using FileVector = std::vector<String>;

namespace Request
{
    const String bcStandard = "LOLKEK";
    const String recvMyFile = "RECVMYFILE";
    const String fileLstPls = "FILELISTPLS";
    const String sendMeFile = "SENDMEFILE";
    const String noSuchFile = "NOSUCHFILE";
};

const Regex rxRecMyFile(Request::recvMyFile);
const Regex rxFileListPls(Request::fileLstPls);
const Regex rxSendMeFile(Request::sendMeFile);


Model::Model() :
    cmsg(),
    csfn(),
    cpca()
{
    Log::InRed("Model() :");
    __Begin;
    {
        StartAuraThreadIn();
        StartAuraThreadOut();
        StartMessageReceivingThread();
        StartRequestReceivingThread20();
    }
    __End;
}

Model::Model(USHORT mp, USHORT rp, USHORT fp, USHORT pp) :
    cmsg(mp),
    csfn(rp, fp),
    cpca(pp)
{
    Log::InRed("Model():");
    __Begin;
    {
        StartAuraThreadIn();
        StartAuraThreadOut();
        StartMessageReceivingThread();
    }
    __End;
    Log::InRed("Model();");
}
Model::~Model()
{
    stupidThreadsDie = true;
    Log::InRed("~Model()");
}
int Model::WsaStartup()
{
    static WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData);
}
#define InRed DoNothing
void Model::StartAuraThreadIn()
{
    Log::InRed("StartAuraThreadIn()->");
    __Begin;
    static Thread auraThreadIn{ [&]() 
    {
        Log::InRed("auraThreadIn->");

        TimePoint lastRefresh = Clock::now();
        Duration refreshRate = Seconds(10);
        while (!stupidThreadsDie) 
        {
            if (Clock::now() - lastRefresh > refreshRate) 
            {
                cpca.activeLocalBroadcasters.clear();
                lastRefresh = Clock::now();
                Log::InRed("presenceAura.refresh();");
            }

            STR msg = cpca.ReceiveBroadcastedMessage();
            if(msg == Request::bcStandard)
            {
                ULONG ip = cpca.inAddrSdr.addr.sin_addr.S_un.S_addr;
                USHORT port = ntohs(cpca.inAddrSdr.addr.sin_port);
                Aura aura(ip, port);
                
                Bool auraNotInSet = false;
                {
                    auto searchResult = std::find(
                        cpca.activeLocalBroadcasters.begin(),
                        cpca.activeLocalBroadcasters.end(),
                        aura);
                    auraNotInSet = searchResult == cpca.activeLocalBroadcasters.end();
                }

                if (auraNotInSet)
                    cpca.activeLocalBroadcasters.push_back(aura);
            }
            else
            {
                Log::InRed("<" + msg + "><" + Request::bcStandard + ">");
            }
        }
    } };
    auraThreadIn.detach();
    __End;
    Log::InRed("<-StartAuraThreadIn()");
}
void Model::StartAuraThreadOut()
{
    Log::InRed("StartAuraThreadOut()->");
    __Begin;
    {
        auto auraThreadOutFunction = [&]() 
        {
            Log::InRed("auraThreadOut->");
            while (!stupidThreadsDie)
            {
                cpca.SendMessageBroadcast(Request::bcStandard);
                std::this_thread::sleep_for(MSeconds(300));
            }
        };
        Thread auraThreadOut{ auraThreadOutFunction };
        auraThreadOut.detach();
    }
    __End;
    Log::InRed("StartAuraThreadOut();");
}
#undef InRed
void Model::StartMessageReceivingThread()
{
    Log::InRed("StartMessageReceivingThread()->");
    __Begin;
    {
        Thread messengerThreadReceive
        {
            [&]()
            {
                while (!stupidThreadsDie)
                    cmsg.ReceiveMessage();
            }
        };
        messengerThreadReceive.detach();
    }
    __End;
    Log::InRed("< -StartMessageReceivingThread()");
}
///#undef InRed

void Model::StartRequestReceivingThread20()
{
    Log::InRed("StartRequestReceivingThread20()->");
    __Begin;
    {
        auto acceptingRequestFunction = [&]()
        {
            Log::InRed("acceptingRequestFunction()->");
            __Begin;
            while (!stupidThreadsDie) 
            {
                Receiver requestReceiver(csfn.other20.requestListener.Accept());

                auto recvRequestAndDoSmthAboutIt = [&]()
                {
                    Log::InRed("acceptingRequestFunction()->");
                    Receiver rc = std::move(requestReceiver);

                    Log::InRed("recvingRequestAndDoingSmth");
                    String request;
                    {
                        while (request.empty())
                            request = rc.ReceiveMessage();
                    }                    

                    if (request == Request::fileLstPls)
                    {
                        String fileList = "SharedFolder/" "\n";
                        {
                            for (auto& file : this->csfn.self.GetFileList())
                            {
                                fileList += file + "\n";
                            }
                        }

                        Sender responder(std::move(rc));

                        Int sendResult = SOCKET_ERROR;
                        while(sendResult == SOCKET_ERROR)
                            sendResult = responder.SendMessageToUser(fileList);
                    }

                    if (std::regex_search(request, rxSendMeFile))
                    {
                        String fileName = std::regex_replace(request, rxSendMeFile, "");
                        String filePath = csfn.self.SharedFolderPath();

                        Sender responder(std::move(rc));
                        csfn.other20.SendFile(responder, filePath + fileName);
                    }

                    if (std::regex_search(request, rxRecMyFile))
                    {
                        String fileName = std::regex_replace(request, rxRecMyFile, "");
                        fileName = csfn.self.FileCreate(fileName);

                        String filePath = csfn.self.SharedFolderPath();

                        Sender imOk = std::move(rc);
                        imOk.SendMessageToUser("IMOK");
                        Receiver responder = std::move(imOk);
                        csfn.other20.RecvFile(responder, filePath + fileName);
                    }
                };

                Thread recvRequest(recvRequestAndDoSmthAboutIt);
                recvRequest.detach();
                std::this_thread::sleep_for(Seconds(2));
            }
            __End;
            Log::InRed("acceptingRequestFunction()->");
        };

        Thread acceptingRequestThread(acceptingRequestFunction);
        acceptingRequestThread.detach();
        Log::InRed("acceptingRequestThread.detached+");
    }
    __End;
    Log::InRed("StartRequestReceivingThread20();");
}
