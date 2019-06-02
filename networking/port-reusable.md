## 端口复用
### 适用场景
- 防止服务器重启时，之前绑定的端口未释放
- 程序突然退出但端口没有释放

### 设置方法
`setsockopt()`

### 注意事项
- 必须在端口绑定之前设置端口复用的属性