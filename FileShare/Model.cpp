#include "Logger.h"
#include "Model.h"

using namespace FileShare;
Model::Model():
    chatMessenger(),
    sfNavigator(),
    udfNavigator(),
    presenceAura()

{
    {
        #ifdef LOGGER
        Log::TextInRed(Model():);
        #endif // LOGGER
    }
    StartAuraThreadIn();
    StartAuraThreadOut();
}

Model::Model(USHORT mp, USHORT rp, USHORT fp, USHORT pp) :
    chatMessenger(mp),
    sfNavigator(rp, fp),
    presenceAura(pp)
{}
Model::~Model()
{
    stupidThreadsDie = true;

    {
        #ifdef LOGGER
        Log::TextInRed(~Model());
        #endif // LOGGER
    }
    
}

STR msgStandard = "lol-kek-cheburek";
void Model::StartAuraThreadIn()
{
    {
        #ifdef LOGGER
        Log::TextInRed(StartAuraThreadIn()->);
        #endif
    }

    static Thread auraThreadIn
    { [&]() {
        {
            #ifdef LOGGER
            __Begin;
            Log::TextInRed(auraThreadIn->);
            __End;
            #endif
        }
        TimePoint lastRefresh = Clock::now();
        Duration refreshRate = Duration(10);
        while (true) {
            if (stupidThreadsDie)
                return;
            if (Clock::now() - lastRefresh > refreshRate) {
                {
                    #ifdef LOGGER
                    Log::TextInRed(auraCount refresh);
                    #endif
                }
                presenceAura.activeLocalBroadcasters.clear();
                lastRefresh = Clock::now();
            }

            STR msg = presenceAura.ReceiveBroadcastedMessage();
            if (msg == msgStandard) {
                ULONG ip = presenceAura.inAddrSdr.addr.sin_addr.S_un.S_addr;
                USHORT port = ntohs(presenceAura.inAddrSdr.addr.sin_port);
                Aura aura(ip, port);

                presenceAura.activeLocalBroadcasters.insert(aura);
            }
        }
    }};
    auraThreadIn.detach();
    {
        #ifdef LOGGER
        Log::TextInRed(<-StartAuraThreadIn());
        #endif
    }
}
void Model::StartAuraThreadOut()
{
    {
        #ifdef LOGGER
        Log::TextInRed(StartAuraThreadOut()->);
        #endif
    }

    Thread auraThreadOut
    {[&](){
        {
            #ifdef LOGGER
            __Begin;
            Log::TextInRed(auraThreadOut->);
            __End;
            #endif
        }
        while (true) 
        {
            if (stupidThreadsDie)
                return;
            presenceAura.SendMessageBroadcast(msgStandard);
            std::this_thread::sleep_for(Duration(2));
        }
    }};
    auraThreadOut.detach();
    {
        #ifdef LOGGER
        Log::TextInRed(<-StartAuraThreadOut());
        #endif
    }
}
