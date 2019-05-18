## IO 多路复用

### 什么是IO多路复用
- 构造一个文件描述符的列表，将要监听的文件描述符加入到该表中。
- 调用一个函数（select/poll），监听该表中的文件描述符，直到任意描述符进行I/O操作时，该函数才返回。
  - 该函数为阻塞函数
  - 函数对文件描述符的检测操作是
- 函数返回时，告诉进程有多少（哪些）需要处理

### select/poll
内核告诉用户进程有几个连接有数据到来，用户进程遍历检查所有连接（存在线性表中），对其中有数据者进行处理。
select函数会返回数量。

```C
int select(
int nfds,   // 最大的文件描述符 + 1     
fd_set *readfds,  // 读操作集合 文件描述符对应的缓冲区是否有数据
fd_set *writefds, // 写操作集合 一般写NULL
fd_set *exceptfds, // 异常集合
struct timeval *timeout
)
// fd_set的最大为1024(内核规定的有1024个标志位)
timeout：
- NULL: 永久阻塞
    - 当检测到fd变化的时候返回
- timeval a:
    - 到时间了自动返回

相关函数：
- void FD_ZERO(fd_set *set)
- void FD_CLR(int fd, fd_set *set)
- void FD_SET(int fd, fd_set *set)
- int FD_ISSET(int fd, fd_set *set)

返回值：
- 返回已准备好的文件描述符 
```


### epoll
内核不仅告诉有几个，同时会告诉是哪几个。