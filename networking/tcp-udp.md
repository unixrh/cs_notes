### tcp-udp 知识复习
#### tcp的三次握手
- 0.server 启动之后，进入listening状态，监听等待连接
- 1.第一次握手：client 发起connect请求， SYN seq=x; client 状态变为 SYN_SENT
- 2.第二次握手：server 收到之后，做一些校验，同意请求，然后给client回包，其中的SYN seq=y, ACK=x+1, server向客户端发起了连接请求，server 状态进入SYN_RCVD
- 3.第三次握手：然后client进入ESTABLISHED，给server回包，ACK=y+1；
- 4.server 收到ACK的时候，server的状态也进入ESTABLISHED

#### tcp的四次挥手
- 0.第一次：client 发起close()请求, FIN seq=x+2 ACK=y+1 client进入状态 FIN_WAIT_1
- 1.第二次：server收到之后，数据校验，然后同意，发送 ACK=x+3，server 进入状态 CLOSE_WAIT，client进入FIN_WAIT_2
此时连接处于半关闭状态
- 2.第三次：server调用close()请求， 给client发送 FIN seq=y+1 server进入 LAST_ACK
- 3.第四次：client进入TIME_WAIT状态，然后给server发ACK=y+2，server关闭进程
- 4.client 会等待1min左右（2MSL）的时间才最终断开，为了给server时间来接受信息并且关闭进程
