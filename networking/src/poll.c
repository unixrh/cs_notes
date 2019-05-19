#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h> 
#include <sys/socket.h>
#include <sys/poll.h>

int main(int argc, char* argv[]) {
  if(argc < 2) {
    printf("eg: ./a.out port\n");
    exit(1);
  }

  struct sockaddr_in serv_addr;
  socklen_t serv_len = sizeof(serv_addr);
  int port = atoi(argv[1]);

  // 创建套接字
  int lfd = socket(AF_INET, SOCK_STREAM, 0);

  memset(&serv_addr, 0, serv_len);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(port);
  
  // 绑定IP和端口
  bind(lfd, (struct sockaddr*)&serv_addr, serv_len);

  // 设置同时监听的最大个数
  listen(lfd, 36);

  printf("start accept ...\n");

  struct sockaddr_in client_addr;
  socklen_t cli_len = sizeof(client_addr);

  struct pollfd allfd[1024];

  // 最大有效下标
  int max_index = 0;

  // init
  for(int i = 0; i < 1024; i++) {
    allfd[i].fd = -1;
  }
  allfd[0].fd = lfd;
  allfd[0].events = POLLIN;

  while(1) {
    // 委托内核做IO检测
    int ret = poll(allfd, max_index + 1, -1);
    if(ret == -1) {
      perror("poll error");
      exit(1);
    }

    // 客户端发起了新连接
    if(allfd[0].revents & POLLIN) {
      int cfd = accept(lfd, (struct sockaddr*)&client_addr, &cli_len); // 此时accept不阻塞
      if(cfd == -1) {
        perror("accpet error");
        exit(1);
      }

      char ip[64];
      printf("new client ip: %s, port: %d\n", inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, ip, sizeof(ip)), ntohs(client_addr.sin_port));
      int i = 0;
      for(i = 0; i < 1024; i++) {
        if(allfd[i].fd == -1) {
          allfd[i].fd = cfd;
          allfd[i].events = POLLIN;
          break;
        }
      }
      printf("new max_index: %d\n", max_index);
      max_index = max_index < i ? i : max_index;
    }

    for(int i = 1; i <= max_index; i++) {
      int fd = allfd[i].fd;
      printf("current fd: %d\n", fd);
      if(fd == -1) {
        continue;
      }
      if(allfd[i].revents & POLLIN) {
        char buf[1024] = {0};
        int len = recv(fd, buf, sizeof(buf), 0);
        if(len == -1) {
          perror("recv error");
          exit(1);
        }
        else if(len == 0) {
          allfd[i].fd = -1;
          printf("客户端已经断开了连接\n");
          close(fd);
        } else {
          printf("recv buf: %s\n", buf);
          for(int k = 0; k < len; ++k) {
            buf[k] = buf[k] - 'a' + 'A';
          }
          send(fd, buf, strlen(buf)+1, 0);
        }
      }
    }
  }
}
