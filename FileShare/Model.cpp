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

//void ProceedWithRequestReceiver(Model& mdl, RequestReceiver& rr)
//{
//    String request;
//    
//    while(request.empty())
//        request = rr.ReceiveRequest();
//
//    RequestSender rs(std::move(rr));
//    
//    if (std::regex_match(request, rxFileListPls))
//    {
//        String fileList;
//        {
//            for (auto& f : mdl.csfn.self.GetFileList())
//            {
//                fileList += f + "\n";
//            }
//        }
//        RequestSender rs(std::move(rr));
//        rs.SendRequest(fileList);
//    }
//
//    if (std::regex_search(request, rxRecMyFile))
//    {
//        String fileName = std::regex_replace(request, rxRecMyFile, {});
//        mdl.csfn.self.FileCreate(fileName);
//
//        FileReceiver fr = mdl.csfn.other.fileListener.Accept();
//        fr.ReceiveFile(mdl.csfn.self.SharedFolderPath() + fileName);
//    }
//
//    if (std::regex_search(request, rxSendMeFile))
//    {
//        String fileName = std::regex_replace(request, rxSendMeFile, {});
//        
//        if (mdl.csfn.self.FileExists(fileName))
//        {
//            rs.SendRequest(Request::recvMyFile + fileName);
//
//            String sfPath = mdl.csfn.self.SharedFolderPath();
//            FileSender fs(rs.addr.sin_addr.S_un.S_addr);
//            fs.SendFile(sfPath + request);
//        }
//        //else
//        //{
//        //    rs.SendRequest(Request::noSuchFile + fileName);
//        //}
//    }
//
//}
//void Model::StartRequestReceivingThread()
//{
//    Log::InRed("StartRequestReceivingThread()->");
//    __Begin;
//    {
//        auto requestThreadFunclion = [&]()
//        {
//            while (!stupidThreadsDie)
//            {
//                RequestReceiver rr = csfn.other.requestListener.Accept();
//
//                auto thFun = [&]()
//                {
//                    ProceedWithRequestReceiver(*this, rr);
//                };
//
//                Thread th{ thFun };
//                th.detach();
//            }
//        };
//        Thread sfoThreadReceiveRequests{ requestThreadFunclion };
//        sfoThreadReceiveRequests.detach();
//    }
//    __End;
//    Log::InRed("StartRequestReceivingThread();");
//}

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

                    Sender responder(std::move(rc));

                    if (request == Request::fileLstPls)
                    {
                        String fileList = "SharedFolder/" "\n";
                        {
                            for (auto& file : this->csfn.self.GetFileList())
                            {
                                fileList += file + "\n";
                            }
                        }

                        Int sendResult = SOCKET_ERROR;
                        while(sendResult == SOCKET_ERROR)
                            sendResult = responder.SendMessageToUser(fileList);
                    }

                    if (std::regex_search(request, rxSendMeFile))
                    {
                        String fileName = std::regex_replace(request, rxSendMeFile, "");
                        String filePath = csfn.self.SharedFolderPath();

                        Int sendResult = SOCKET_ERROR;
                        //while (sendResult == SOCKET_ERROR)
                            sendResult = csfn.other20.SendFile(responder, filePath + fileName);
                    }
                };

                Thread recvRequest(recvRequestAndDoSmthAboutIt);
                recvRequest.detach();
                //while (!requestReceiver.InvalidSocket());
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
