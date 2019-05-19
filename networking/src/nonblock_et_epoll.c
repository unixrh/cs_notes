#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h> 
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>

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

  // 创建epoll树的根节点
  int epfd = epoll_create(2000);
  struct epoll_event all[2000];
  struct epoll_event ev;
  ev.events = EPOLLIN;
  ev.data.fd = lfd;
  epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);

  while(1) {
    // 使用epoll委托内核做IO检测
    int ret = epoll_wait(epfd, all, 2000, -1);
    printf("+++++++++++++++epoll wait returned+++++++++++++++\n");
    for(int i = 0; i < ret; i++) {
      int fd = all[i].data.fd;
      if(fd == lfd) {
        int cfd = accept(lfd, (struct sockaddr*)&client_addr, &cli_len);
        if(cfd == -1) {
          perror("accept error");
          exit(1);
        }

        int flag = fcntl(cfd, F_GETFL);
        flag |= O_NONBLOCK;
        fcntl(cfd, F_SETFL, flag);

        struct epoll_event tmp;
        // 设置边沿触发
        tmp.events = EPOLLIN | EPOLLET;
        tmp.data.fd = cfd;
        epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &tmp);
      } else {
        if(all[i].events & EPOLLIN) {
          char buf[5] = {0};
          int len;
          while((len = recv(fd, buf, 5, 0)) > 0)  {
            write(STDOUT_FILENO, buf, len);
            send(fd, buf, len, 0);
          }
          if(len == 0) {
            printf("client disconnected\n");
            int del = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
            if(del == -1) {
              perror("del error");
            }
            close(fd);
          } else if(len == -1) {
            if(errno == EAGAIN) {
              printf("buffer no data\n");
            } else {
              perror("recv error\n");
              exit(1);
            }
          }
        }
      }
    }
  }
  close(lfd);
  return 0;
}
