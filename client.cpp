#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <cstring>
#include <string>
#include "protocol.h"

#pragma comment(lib, "ws2_32.lib")

constexpr int PORT = 9000;
constexpr const char* SERVER_IP = "127.0.0.1";

int main() {
   WSADATA wsaData;
   if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
   {
      std::cerr << "[Client] WSAStartup failed\n";
      return 1;
   }

   SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
   if (sockfd == INVALID_SOCKET) 
   {
      std::cerr << "[Client] socket() failed: " << WSAGetLastError() << "\n";
      WSACleanup();
      return 1;
   }

   sockaddr_in server_addr{};
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(PORT);

   uint32_t seq = 1;

   if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) != 1) 
   {
      std::cerr << "[Client] Invalid IP address\n";
      closesocket(sockfd);
      WSACleanup();
      return 1;
   }

   while (true) {
      std::string str;
      std::getline(std::cin, str);
      if (str == "exit") 
      {
         break;
      }

      MyMessage msg{};
      msg.type = 1;
      msg.seq = seq++;

      strncpy_s(msg.payload, str.c_str(), sizeof(msg.payload) - 1);
      msg.payload[sizeof(msg.payload) - 1] = '\0';

      int sent = sendto(sockfd, (const char*)&msg, sizeof(msg), 0,
         (sockaddr*)&server_addr, sizeof(server_addr));

      if (sent == SOCKET_ERROR)
      {
         std::cerr << "[Client] Message send failed: " << WSAGetLastError() << "\n";
      }
      else 
      {
         std::cout << "[Client] Message sent (Seq: " << msg.seq << ")\n";
      }
   }

   closesocket(sockfd);
   WSACleanup();
   return 0;
}
