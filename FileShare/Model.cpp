#include "Logger.h"
#include "Model.h"

#include <thread>
using Thread = std::thread;

#include <chrono>
using Clock = std::chrono::system_clock;
using TimePoint = Clock::time_point;
using Duration = std::chrono::seconds;

#include <string>
using String = std::string;
#include <vector>
#include <utility>
using Request = std::pair<String, String>; // received
using RequestVector = std::vector<Request>;
using FileVector = std::vector<String>;

using namespace FileShare;
Model::Model() :
    chatMessenger(),
    sfNavigator(),
    presenceAura()
{
    Log::TextInRed(Model() :);
    __Begin;
    {
        StartAuraThreadIn();
        StartAuraThreadOut();
        StartMessageReceivingThread();
    }
    __End;
}

Model::Model(USHORT mp, USHORT rp, USHORT fp, USHORT pp) :
    chatMessenger(mp),
    sfNavigator(rp, fp),
    presenceAura(pp)
{
    Log::TextInRed(Model() :);
    __Begin;
    {
        StartAuraThreadIn();
        StartAuraThreadOut();
        StartMessageReceivingThread();
    }
    __End;
    Log::TextInRed(< -);
}
Model::~Model()
{
    stupidThreadsDie = true;
    Log::TextInRed(~Model());
}

STR msgStandard = "lol-kek-cheburek";
void Model::StartAuraThreadIn()
{
    Log::TextInRed(StartAuraThreadIn()->);
    __Begin;
    static Thread auraThreadIn{ [&]() 
    {
        Log::TextInRed(auraThreadIn->);

        TimePoint lastRefresh = Clock::now();
        Duration refreshRate = Duration(10);
        while (!stupidThreadsDie) 
        {
            if (Clock::now() - lastRefresh > refreshRate) 
            {
                presenceAura.activeLocalBroadcasters.clear();
                lastRefresh = Clock::now();
            }

            STR msg = presenceAura.ReceiveBroadcastedMessage();
            if (msg == msgStandard) 
            {
                ULONG ip = presenceAura.inAddrSdr.addr.sin_addr.S_un.S_addr;
                USHORT port = ntohs(presenceAura.inAddrSdr.addr.sin_port);
                Aura aura(ip, port);

                presenceAura.activeLocalBroadcasters.insert(aura);
            }
        }
    } };
    auraThreadIn.detach();
    __End;
    Log::TextInRed(< -StartAuraThreadIn());
}
void Model::StartAuraThreadOut()
{
    Log::TextInRed(StartAuraThreadOut()->);
    __Begin;
    {
        Thread auraThreadOut
        { 
            [&]() {
                while (!stupidThreadsDie)
                    presenceAura.SendMessageBroadcast(msgStandard);
            } 
        };
        auraThreadOut.detach();
    }
    __End;
    Log::TextInRed(< -StartAuraThreadOut());
}

void Model::StartMessageReceivingThread()
{
    Log::TextInRed(StartMessageReceivingThread()->);
    __Begin;
    {
        Thread messengerThreadReceive
        {
            [&]()
            {
                while (!stupidThreadsDie)
                    chatMessenger.ReceiveMessage();
            }
        };
        messengerThreadReceive.detach();
    }
    __End;
    Log::TextInRed(< -StartMessageReceivingThread());
}
