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

#### 伪代码
```c
// 假设有ABCDEFG六个客户端连接，连接描述符分别为3,4,5,100,102,103
fd_set reads; // 用户空间的文件描述符列表
FD_SET(3, &reads);
select(103+1, &reads, NULL, NULL, NULL); // 拷贝一份到内核空间

内核检测文件描述符对应缓冲区的变化，将它对应的fd_set（内核）对应位置的状态进行修改；最后用修改过的表拷贝覆盖用户空间的表
```
#### select代码
```c
int main() {
    int lfd = socket();
    bind();
    listen();
    // 创建一个文件描述符表
    fd_st reads, temp; // 留一个reads保留监听备份，temp用来传给select，它最终会被内核的表覆盖
    fd_zero(&reads); // 清空
    // 监听描述符lfd加入到读集合中
    fd_set(lfd, &reads);
    int maxfd = lfd;
    while(1) {
        // 委托内核监听这一组文件描述符
        temp = reads;
        int ret = select(maxfd+1, &temp, NULL, NULL, NULL);

        // 判断是不是监听描述符
        if(fd_isset(lfd, &temp)) {
            // 接受新连接
            int cfd = accept();
            // 将cfd加入到读集合中
            fd_set(cfd, &reads);
            // 更新maxfd
            maxfd = maxfd < cfd ? cfd:maxfd;
        }
        // 判断是客户端发送数据
        for(int i = lfd+1; i <= maxfd; i++) {
            if(fd_isset(i, &temp)) {
                int len = read();
                if(len == 0) { // 客户端断开连接
                    // cfd 从集合中删除
                    fd_clr(i, &reads);
                }
            }
        }
    }
}
```
#### 优缺点分析
- 优点
  - 跨平台
- 缺点
  - 每次调用select，都需要把fd集合从用户态拷贝到内核态，开销较大
  - 每次调用都需要在内核遍历传递进来的所有fd，这个开销很大
  - select支持的文件描述符数量太小了，默认为1024，因为是数组实现的

### poll
可以超过1024，因为内部是链表实现的。

`int poll(struct pollfd *fd, nfds_t, nfds, int timeout);`

### epoll
内核不仅告诉有几个，同时会告诉是哪几个。