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

## Nginx的命令行控制

默认情况下，Nginx被安装在目录`/usr/local/nginx`中，二进制文件安装在`/usr/local/nginx/sbin/nginx`，配置文件路径为`/usr/local/nginx/conf/nginx.conf`。

1.  默认方式启动

    直接执行Nginx二进制程序：

    ```shell
    /usr/local/sbin/nginx
    ```

    这时会打开在configure命令执行时使用`--conf-path=PATH`指定的nginx.conf文件。

2.  另行指定配置文件的启动方式

    使用`-c`参数指定配置文件：

    ```shell
    /usr/local/nginx/sbin/nginx -c /tmp/nginx.conf
    ```

3.  另行指定安装目录的启动方式

    使用`-p`参数指定Nginx的安装目录：

    ```shell
    /usr/local/nginx/sbin/nginx -p /usr/local/nginx/
    ```

4.  另行指定全局配置项的启动方式

    通过`-g`参数临时指定一些全局配置项，以使新的配置项


