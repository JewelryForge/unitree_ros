#ifndef _TRANSMIT_H_
#define _TRANSMIT_H_

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

class Transmitter {
 public:
  Transmitter(uint16_t in_port, uint16_t out_port) {
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) throw std::runtime_error("socket error");
    sockaddr_in local_addr{};
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(in_port);
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sock_fd, reinterpret_cast<sockaddr *>(&local_addr), sizeof(local_addr)) < 0) {
      throw std::runtime_error("bind error");
    }

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(out_port);
    dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  }

  void transmit() {
    sockaddr_in remote_addr{};
    uint32_t sin_size = sizeof(sockaddr_in);
//    long len = recvfrom(sock_fd, buf, BUFSIZ, 0, nullptr, nullptr);
//    if (len < 0) return;
//    std::cout << "Recv: " << len << std::endl;
    sendto(sock_fd, buf, 10, 0, reinterpret_cast<sockaddr *>(&dest_addr), sin_size);
//    sendto(sock_fd, "buffer", 3, 0, reinterpret_cast<sockaddr *>(&dest_addr), sin_size);
  }

  ~Transmitter() {
    close(sock_fd);
  }
 private:
  int sock_fd;
  sockaddr_in dest_addr{};
  char buf[BUFSIZ]{};
};

#endif //_TRANSMIT_H_
