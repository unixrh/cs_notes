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

#### 相关函数
- `int epoll_create(int size)`:
  - 生成一个epoll专用的文件描述符（树状集合，每个节点和一个文件描述符一一对应）
  - size:epoll上能关注的最大文件描述符数量，但是实际上会自动扩展，所以不必严格
- `int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)`
  - 用于控制某个epoll文件描述符事件，可以注册、修改、删除
  - epfd：epoll_create的返回值，epoll树状结构的根节点
  - op:
    - EPOLL_CTL_ADD 注册
    - EPOLL_CTL_MOD 修改
    - EPOLL_CTL_DEL 删除
  - fd: 关联的文件描述符
  - event：传入参数，告诉内核要监听什么事件
```c
struct epoll_event {
    uint32_t events;
    epoll_data_t data; // 联合体，包含fd等
};
```
- `int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)`
  - 等待IO事件发生 - 可以设置阻塞的函数
  - 对应 select 和 poll 函数
  - events：传出的数据，结构体数组，存储有发生变化的节点
  - maxevents: 告诉内核这个events的大小
  - timeout：超时时间
    - -1：永久阻塞
    - 0：立即返回
    - \>0: 具体超时时间

#### 伪代码
```c
int main() {
    // 创建监听的套接字
    int lfd = socket();
    // 绑定
    bind();
    // 监听
    listen();
    // epoll树根节点
    int epfd = epoll_create(3000);
    // 存储发生变化的fd对应的信息
    struct epoll_event all[3000];
    // init
    // 监听描述符lfd挂到epoll树上
    // struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = lfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
    while(1) {
        // 委托内核检测事件
        int ret = epoll_wait(epfd, all, 3000, -1);
        // 根据ret遍历all数组
        for(int i = 0; i < ret; ++i) {
            int fd = all[i].data.fd;
            // 有新的连接
            if(fd == lfd) {
                // 接受连接请求 - accept 不阻塞
                int cfd = accept();
                // cfd挂到树上
                ev.events = EPOLLIN;
                ev.data.fd = cfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
            }
            // 已连接的客户端有数据发送过来
            else {
                // 只处理客户端发来的数据
                if(all[i].events & EPOLLIN) {
                    // 读数据
                    int len = recv();
                    if(len == 0) {
                        close(fd);
                        // 从树上删除
                        epoll_ctl(epfd,EPOLL_CTL_DEL, fd, NULL);
                    }
                    // 写数据
                    send();
                } else {
                    continue;  
                }
            }
        }
    }
}
```
#### epoll水平触发模式(LT)（根据读来解释）
- 默认的就是水平触发模式
- 只要fd对应的缓冲区有数据，那么`epoll_wait`就会返回
- 返回的次数与发送数据的次数没有关系
- 发送100字节，若每次读1个，则要100次
- 开销大效率低

#### epoll边沿触发模式(ET)
- fd有默认的阻塞属性
- 客户端给server端发数据：
  - 客户端每发一次，server的epoll_wait就返回一次
  - 不在乎数据是否读完，缓冲区会有上一次数据留存
  - 这种模式是为了提高epoll的效率，减小返回的次数
  - 缺点是数据读不完
  - 一般不单独使用ET模式
  - 如何全部读出：
    - while(recv())？ 不行，因为当数据读出来以后会阻塞在这里
    - 解决阻塞问题：
      - 设置非阻塞 - fd

#### epoll边沿非阻塞
- 效率是最高的
- 如何设置非阻塞
  - open()
    - 设置flags
    - 必须 O_WDRW | O_NONBLOCK
    - 终端文件：/dev/tty
  - fcntl
    - int flag = fcntl(fd, F_GETFL);
    - flag |= O_NONBLOCK;
    - fcntl(fd, F_SETFL, flag)
  - 将缓冲区的全部数据都读出
```c
while(recv() > 0) {
    printf();
}
// 当缓冲区数据读完之后，返回值是否为0？
```