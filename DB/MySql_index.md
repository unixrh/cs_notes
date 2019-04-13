### MySQL索引笔记

#### Myisam vs innoDB
##### Myisam
* 不支持事务
* 聚集索引，索引和数据处于分离的文件中
* 对于每个表，包含的文件有.frm(表结构fragment)，.MYD(data)，.MYI(index)
* 主键索引和非主键索引的数据结构和查找方式是一致的，在读取数据方面性能很高
* 只支持表锁

##### InnoDB
* 支持事务，支持四种隔离（读未提交，读已提交，可重复读，串行化），默认为可重复读
* 支持表锁、支持行锁，行锁在命中索引的情况下才可以起作用
* .frm表结构fragment，.idb是数据文件

#### 磁盘结构
* 盘面、柱面、磁道、扇区
* 寻道：磁头移动定位到指定磁道
* 旋转：等待指定扇区从磁道下旋转经过
* 数据传输：数据再磁盘与内存之间的实际传输

#### B-Tree
![B-Tree](http://mmbiz.qpic.cn/mmbiz_png/0vU1ia3htaaMHicUIHWzjgbYJFb7gseSvJp2szEHdqfn06vscPSE7c9Smg6KREic2csXWxibWtcdTs2GslBoXEAKQQ/?wx_fmt=png)

#### B+Tree
![Myisam](http://mmbiz.qpic.cn/mmbiz_png/0vU1ia3htaaMHicUIHWzjgbYJFb7gseSvJ0JtC3qxsNWGdIpicUPCrBSkDibR8PzKyLlt7Eb5SXcG3O4dRIMYuruicQ/?wx_fmt=png)
![innoDB](http://mmbiz.qpic.cn/mmbiz_png/0vU1ia3htaaMHicUIHWzjgbYJFb7gseSvJZAT1VcoCCPdKa6X1C89Gvqsz7a9ibLa6y1cvf66mqf5yJKicKeyatuiaA/?wx_fmt=png)
