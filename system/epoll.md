## epoll 相关知识
### 触发模式
#### 水平触发 LT
* 只要缓冲区有数据，那么epoll_wait就会返回，直到读完
* 会多次触发epoll_wait，导致系统开销较大，效率较低
* epoll_wait返回次数与数据的发送次数无关
* epoll 默认的工作模式

#### 边沿触发 ET
* fd对应的内核缓冲区默认是阻塞的
* sruct epoll_events tmp;  tmp.events = EPOLLIN | EPOLLET;
* 每次缓冲区有新的数据进入，则触发epoll_wait就返回一次，
* 不管缓冲区数据是否读完
* 这种模式减少了wait的返回，但是会导致数据读不完

#### 边沿非阻塞 ET NONBLOCK
效率是最高的
如何设置非阻塞
open()函数:
* 设置flag
* O_WDRW | O_NONBLOCK
* 场景/dev/tty
fcntl:
* int flag = fcntl(fd, F_GETFL)
* flag |= O_NONBLOCK;
* fcntl(fd, F_SETFL, flag);
设置为非阻塞之后，epoll_wait返回之后将缓冲区的全部数据都读出
```C
char buf[1024] = {0};
int len;
while((len = recv(fd, buf, sizeof(buf), 0)) > 0){
  // 得到数据
  write(STDOUT_FILENO, buf, len);
  send(fd, buf, len, 0);
}
if(len == 0) {
  // 客户端断开连接
} else if(len == -1){ // 读缓冲区出错
  if(errno == EAGAIN){
    // 缓冲区为空可重试再读
  } else {
    // 读取数据出错
  }
}
```
