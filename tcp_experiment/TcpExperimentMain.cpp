#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <algorithm>

#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define pass {}

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")
constexpr char inetAddr[] = "172.23.50.193";

void WsaStartup();
void Bind(struct SocketedEntity&);
void Listen(struct SocketedEntity&);
void Connect(struct SocketedEntity&);
void Accept(struct SocketedEntity&, struct SocketedEntity&);
void Send(struct SocketedEntity&);
void Receive(struct SocketedEntity&);
void SetSockOpt(struct SocketedEntity&);

struct SocketedEntity {
    SocketedEntity() :
        sc(INVALID_SOCKET),
        addr{}
    {
        addr.sin_family = AF_INET;

        addrPtr = reinterpret_cast<SOCKADDR*>(&addr);
        addrSize = sizeof(addr);
    }
    ~SocketedEntity() {
        closesocket(sc);
    }

    SocketedEntity(SOCKET s, ULONG inetAddr, USHORT port) :
        SocketedEntity()
    {
        sc = s;
        addr.sin_addr.S_un.S_addr = inetAddr;
        addr.sin_port = htons(port);
    }


    SOCKET sc;
    SOCKADDR_IN addr;
    SOCKADDR* addrPtr;
    INT addrSize;
};

bool operator == (const SOCKADDR_IN& s1, const SOCKADDR_IN& s2) {
    return{
        s1.sin_family == s2.sin_family
        &&
        s1.sin_addr.S_un.S_addr == s2.sin_addr.S_un.S_addr
        &&
        s1.sin_port == s2.sin_port
    };
}

int main() {
    WsaStartup();
    USHORT port;
    std::cin >> port;
    SocketedEntity listener(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP), INADDR_ANY, port);

    Bind(listener);
    Listen(listener);

    std::vector<SocketedEntity> accepted;
    std::thread th_accept([&] {
        while (true) {
            SocketedEntity se;

            se.sc = accept(listener.sc, se.addrPtr, &se.addrSize);
            if (se.sc != INVALID_SOCKET) {
                char buff[100]{};
                if (recv(se.sc, buff, sizeof(buff), NULL) == SOCKET_ERROR)
                    continue;
                else {
                    SOCKADDR_IN sin{};
                    int sinSize = sizeof(sin);
                    getsockname(se.sc, (SOCKADDR*)&sin, &sinSize);
                    std::cout << (int)ntohs(sin.sin_port) << " ";

                    std::cout << buff << std::endl;

                }
            }
        }
    });
    while (true) {
        std::cout << "port msg: ";

        USHORT port;  
        std::string msg;
        std::cin >> port;
        std::cin.ignore();
        std::getline(std::cin, msg);

        SocketedEntity cliToSer(socket(AF_INET, SOCK_STREAM, 0), inet_addr(inetAddr), port);

        Connect(cliToSer);

        std::cout << (int)(ntohs(cliToSer.addr.sin_port)) << " ";

        if (send(cliToSer.sc, msg.c_str(), msg.length(), NULL) <= 0)
            std::cout << "sending failed< error: " << WSAGetLastError() << std::endl;
        else
            pass;
            //std::cout << "msg sent" << std::endl;
    }

}
//
//int unmain() {
//    WsaStartup();
//
//    int p1, p2;
//    std::cin >> p1 >> p2;
//
//    SocketedEntity listener(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP), INADDR_ANY, p1);
//    listener.addr.sin_addr.S_un.S_addr = INADDR_ANY;
//
//    SocketedEntity accepted;
//    SocketedEntity cliToSer(socket(AF_INET, SOCK_STREAM, 0), inet_addr(inetAddr) ,p2);
//
//    SetSockOpt(listener);
//    SetSockOpt(accepted);
//    SetSockOpt(cliToSer);
//
//    Bind(listener);
//    Listen(listener);
//
//    Connect(cliToSer);
//
//    Accept(accepted, listener);
//
//    Send(accepted);
//
//    Receive(cliToSer);
//    //Receive(accepted);
//
//    while (true);
//
//    return system("pause");
//}

void WsaStartup() {
    static WSADATA wsaData;
    if (int error = WSAStartup(MAKEWORD(2, 2), &wsaData))
        std::cout << "wsastartup failed with error: " << error << std::endl;
    else
        std::cout << "wsastartup" << std::endl;
}

void Bind(SocketedEntity& se)
{
    if (bind(se.sc, se.addrPtr, se.addrSize) == SOCKET_ERROR)
        std::cout << "listener socket bind failed, error: " << GetLastError() << std::endl;
    else
        pass;
        //std::cout << "listener socket bound." << std::endl;
}

void Listen(SocketedEntity& se)
{
    listen(se.sc, 1);
}

void Connect(SocketedEntity& se)
{
    for(int i=0; i<3; i++) {
        if (connect(se.sc, se.addrPtr, se.addrSize) == SOCKET_ERROR)
            std::cout << "failed to connect, error: " << GetLastError() << std::endl;
        else {
            //std::cout << "connect successful" << std::endl;
            break;
        }
        Sleep(1000);
    }
}

void Accept(SocketedEntity& ac, SocketedEntity& ls)
{
    std::thread th([&]() {
        while (true) {
            ac.sc = accept(ls.sc, ac.addrPtr, &ac.addrSize);
            /*if (ac.sc == INVALID_SOCKET)
                std::cout << "accept failed, error: " << WSAGetLastError() << std::endl;
            else
                break;*/
            Sleep(1000);
        }
    });
    th.detach();
}

void Send(SocketedEntity& se)
{
    std::thread th([&](){
        while (true) {
            if (se.sc == INVALID_SOCKET)
                continue;

            std::string buff;  
            std::cin >> buff;
            /*std::getline(std::cin, buff);
            if(buff.length())*/
            if (send(se.sc, buff.c_str(), buff.length(), NULL) <= 0)
                std::cout << "sending failed< error: " << WSAGetLastError() << std::endl;
            else
                pass;
                //std::cout << "msg sent" << std::endl;
        }
    });
    th.detach();
}

void Receive(SocketedEntity& se)
{
    std::thread th([&] {
        while (true) {
            char buff[100]{};
            if (recv(se.sc, buff, sizeof(buff), NULL) == SOCKET_ERROR)
                pass
            //std::cout << "recv failed, error: " << GetLastError() << std::endl;
            else
                std::cout << buff << std::endl;
        }
    });
    th.detach();
}

void SetSockOpt(SocketedEntity& se)
{
    int scOp = 1;
    setsockopt(se.sc, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&scOp), sizeof(int));
}
