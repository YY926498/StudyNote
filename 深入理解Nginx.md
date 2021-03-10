# 深入理解Nginx

Nginx的三个主要应用场景：

- 静态资源服务：通过本地文件系统提供服务
- 反向代理服务：Nginx的强大性能、缓存加速、负载均衡
- API服务：OpenResty

Nginx的优点：

1. 高并发，高性能
2. 可扩展性好
3. 高可靠性
4. 热部署
5. BSD许可证

Nginx的四个主要组成部分：

1. Nginx二进制可执行文件：由各模块源码编译出的一个文件
2. Nginx.conf配置文件：控制Nginx的行为
3. access.log访问日志：记录每一条Http请求信息
4. error.log错误日志：定位问题

