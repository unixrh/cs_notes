## epoll 相关知识
### 触发模式
#### 水平触发 LT
* 只要缓冲区有数据，那么epoll_wait就会返回，直到读完
* 会多次触发epoll_wait，导致系统开销较大

#### 边沿触发
* tmp.events = EPOLLIN | EPOLLET;
* 每次触发epoll_wait就读一次数据，不管缓冲区数据是否读完

#### 边沿非阻塞
* int flag = fcntl(fd, GETFL)
