#include <iostream>
#include <string>
#include <thread>

#include <afxsock.h>

constexpr int port = 8888;
bool tracingErrors = true;
void ErrorMessage(const std::string& message, int error = 0, std::ostream& output = std::cerr) {
    std::string error_mess = message + (error ? ", error:" + std::to_string(error) : "");
    if (tracingErrors)
        output << error_mess << std::endl;
    //throw exception(ErrorMessage.c_str());
}

bool mutex = false;


int main() {
    WSADATA wsadata;
    if (int error = WSAStartup(MAKEWORD(2, 2), &wsadata))
        ErrorMessage("WSAStartup failed", error);
    else
        ErrorMessage("WSAStartup succeeded");

#ifndef snd_part
    auto scAddr1 = sockaddr_in{};
    auto scAddr1Ptr = reinterpret_cast<sockaddr*>(&scAddr1);
    scAddr1.sin_family = AF_INET;
    scAddr1.sin_addr.S_un.S_addr = htonl(INADDR_BROADCAST);
    scAddr1.sin_port = htons(port);
#endif

#ifndef rcv_part
    auto scAddr = sockaddr_in{};
    scAddr.sin_family = AF_INET;
    scAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    scAddr.sin_port = htons(port);

    auto scAddrPtr = reinterpret_cast<sockaddr*>(&scAddr);
    auto scAddrSize = sizeof(scAddr);

    auto scAddrSender = sockaddr_in{};
    auto scAddrSenderPtr = reinterpret_cast<sockaddr*>(&scAddrSender);
    int scAddrSenderSize = sizeof(scAddrSender);


    auto sc = socket(AF_INET, SOCK_DGRAM, NULL);
    if (sc == SOCKET_ERROR)
        ErrorMessage("Receiver failed to create socket", GetLastError());
    else
        ErrorMessage("Receiver socket created");

    int scOptions = 1;
    setsockopt(sc, SOL_SOCKET, SO_REUSEADDR, (char*)&scOptions, sizeof(int));
    setsockopt(sc, SOL_SOCKET, SO_BROADCAST, (char*)&scOptions, sizeof(int));


    if (bind(sc, scAddrPtr, scAddrSize) == SOCKET_ERROR)
        ErrorMessage("Receiver failed to bind socket", GetLastError());
    else
        ErrorMessage("Receiver socket bound");
#endif

    std::thread th_out([&]() { 
        std::string buff;

        while (true) {
            std::cin >> buff;
            mutex = true;

            if (sendto(sc, buff.c_str(), buff.size(), NULL, scAddr1Ptr, sizeof(scAddr1)) == SOCKET_ERROR)
                ErrorMessage("Broadcaster failed to send message", GetLastError());
            else
                ErrorMessage("Broadcaster message sent");
            
            mutex = false;
        }

        
    });

    std::thread th_in([&]() {
        while (true) {
            std::string buff(100, 0);

            if (recvfrom(sc, &buff.front(), buff.capacity(), NULL, scAddrSenderPtr, &scAddrSenderSize) == SOCKET_ERROR)
                ErrorMessage("failed to recv message", GetLastError());

            std::string inetInfo(inet_ntoa(scAddrSender.sin_addr));

            buff.shrink_to_fit();
            while (mutex);
            std::cout << inetInfo + ": " + buff << std::endl;
        }
    });


    th_in.join();
    th_out.join();
    return system("pause");
}