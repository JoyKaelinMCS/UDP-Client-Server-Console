#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include "protocol.h"

#pragma comment(lib, "ws2_32.lib")

constexpr int PORT = 9000;

int main() {
   WSADATA wsaData;
   int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
   if (ret != 0) 
   {
      std::cerr << "[Server] WSAStartup failed: " << ret << "\n";
      return 1;
   }

   SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
   if (sockfd == INVALID_SOCKET) 
   {
      std::cerr << "[Server] socket() failed: " << WSAGetLastError() << "\n";
      WSACleanup();
      return 1;
   }

   sockaddr_in server_addr{};
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(PORT);
   server_addr.sin_addr.s_addr = INADDR_ANY;

   if (bind(sockfd, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
   {
      std::cerr << "[Server] bind() failed: " << WSAGetLastError() << "\n";
      closesocket(sockfd);
      WSACleanup();
      return 1;
   }

   /* activate non blocking mode */
   u_long mode = 1;
   if (ioctlsocket(sockfd, FIONBIO, &mode) == SOCKET_ERROR) {
      std::cerr << "[Server] ioctlsocket failed: " << WSAGetLastError() << "\n";
      closesocket(sockfd);
      WSACleanup();
      return 1;
   }

   std::cout << "[Server] Listening on port " << PORT << "\n";

   while (true) {
      MyMessage msg{};
      sockaddr_in client_addr{};
      int client_len = sizeof(client_addr);

      int len = recvfrom(sockfd, (char*)&msg, sizeof(msg), 0,
         (sockaddr*)&client_addr, &client_len);

      if (len == sizeof(MyMessage)) 
      {
         std::cout << "[Server] Received type=" << (int)msg.type
            << ", seq=" << msg.seq
            << ", payload=" << msg.payload << "\n";
      }
      else if (len == SOCKET_ERROR) 
      {
         int err = WSAGetLastError();
         if (err == WSAEWOULDBLOCK) 
         {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
         }

         std::cerr << "[Server] recvfrom failed: " << err << "\n";
      }
      else if (len > 0) 
      {
         std::cout << "[Server] Invalid message length: " << len << "\n";
      }
   }

   closesocket(sockfd);
   WSACleanup();
   return 0;
}
