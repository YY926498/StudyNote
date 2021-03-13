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

    通过`-g`参数临时指定一些全局配置项，以使新的配置项生效：
    
    ~~~sh
    /usr/local/nginx/sbin/nginx -g "pid /var/nginx/test.pid"
    ~~~
    
    上面这行的命令代表把`pid`文件写到`/var/nginx/test.pid`中。
    
    `-g`参数的约束条件是指定的配置项不能与默认路径下的`nginx.conf`中的配置项相冲突，否则无法启动。上面的配置项`pid /var/nginx/test.pid`不能存在于默认的`nginx.conf`中。
    
    另一个约束条件是，以`-g`方式启动的Nginx服务执行其他命令行时，需要把`-g`参数也带上，否则可能会出现配置项不匹配的情形。
    
5.  测试配置信息是否有错误

    在不启动Nginx的情况下，使用`-t`参数仅测试配置文件是否有错误：

    ```sh
    /usr/local/nginx/sbin/nginx -t
    ```

    执行结果显示配置是否正确

6.  在测试配置阶段不输出信息

    测试配置选项时，使用`-q`参数可以不把error级别以下的信息输出到屏幕：

    ~~~sh
    /usr/local/nginx/sbin/nginx -t -q
    ~~~

7.  显示编译阶段的参数

    使用`-V`参数可以显示配置编译阶段的信息

    ~~~sh
    /usr/local/nginx/sbin/nginx -V
    ~~~

8.  快速地停止服务

    使用`-s stop`可以强制停止Nginx服务。`-s`参数是告诉Nginx程序向正在运行的Nginx服务发送信号量，Nginx程序通过`nginx.pid`文件中得到的master进程的进程ID，再向运行中的master进程发送TERM信号来快速地关闭Nginx服务：

    ~~~sh
    /usr/local/nginx/sbin/nginx -s stop
    ~~~

    实际上，通过kill命令直接向master进程发送TERM或者INT信号，效果是一样的。

    ~~~sh
    ps -ef | grep nginx #查找Nginx进程ID
    kill -s SIGTERM masterID #向master进程发送TERM信号
    #或者发送INT信号
    kill -s SIGINT masterID
    ~~~

9.  ’优雅‘地停止服务

    如果要Nginx服务正常地处理完当前所有请求再停止服务，可以使用`-s quit`参数来停止服务：

    ~~~sh
    /usr/local/nginx/sbin/nginx -s quit
    ~~~

    当快速停止服务时，worker进程与master进程在收到信号后会立刻跳出循环，退出进程。而’优雅‘地停止进程时，首先会关闭监听端口，停止接收新的连接，然后把当前正在处理的连接全部处理完，最后再退出进程。直接发送QUIT信号给master进程来停止服务，与执行`-s quit`命令一样。

    ~~~sh
    kill -s SIGQUIT masterID
    ~~~

    如果希望’优雅‘地通知某个worker进程，可以通过向该进程发送WINCH信号来停止服务：

    ~~~sh 
    kill -s SIGWINCH workerID
    ~~~

10.  使运行中的Nginx重读配置项并生效

     使用`-s reload`参数可以使运行中的Nginx服务重新加载`nginx.conf`文件：

     ~~~sh
     /usr/local/nginx/sbin/nginx -s reload
     ~~~

     事实上，Nginx会先检查新的配置项是否有误，如果全部正确就以’优雅‘的方式关闭再重新启动Nginx来实现这个目的。同样，使用kill命令发送HUP信号可以达到相同的效果：

     ~~~sh
     kill -s SIGHUP masterID
     ~~~

11.  日志文件回滚

     使用`-s reopen`参数可以重新打开日志文件，这样可以先把当前的日志文件改名或转移到其他目录中进程备份，再执行这条命令就会生成新的日志文件，避免日志文件过大：

     ~~~sh
     /usr/local/nginx/sbin/nginx -s reopen
     ~~~

     这与使用kill命令发送USR1信号效果相同：

     ~~~sh
     kill -s SIGUSR1 masterID
     ~~~

12.  平滑升级Nginx

     当Nginx服务升级到新的版本时，必须要将旧的二进制文件Nginx替换掉。升级时包括以下步骤：

     1.  通知正在运行的旧版本的Nginx准备升级。通过向master进程发送USR2信号可达到目的：

         ~~~sh
         kill -s SIGUSR2 masterID
         ~~~

         这时，运行中的Nginx会将pid文件重命名，这样新的Nginx才有可能启动成功

     2.  启动新版本的Nginx，可使用之前介绍过的任意一种启动方法

     3.  通过kill命令向旧版本的master进程发送SIGQUIT信号，以’优雅‘的方式关闭旧版本的Nginx。随后，将只有新版本的Nginx服务运行，此时平滑升级完成

13.  显式命令行帮助

     使用`-h`或者`-?`参数显示所有命令行参数

     

# Nginx的配置

## 运行中的Nginx进程间的关系

部署Nginx时都使用一个master进程来管理多个worker进程。一般情况下，worker进程的数量与服务器上的CPU核心数相等。每一个worker进程都是繁忙的，它们负责提供互联网服务，master进程值负责监控管理worker进程。worker进程之间通过共享内存、原子操作等一些进程间通信机制来实现负载均衡等功能。

Nginx支持单进程（master）提供服务的，但是一般还是按照master-worker方式配置同时启动多个进程，优点：

-   master进程不会对用户请求提供服务，只用于管理真正提供服务的worker进程，为管理员提供命令行服务。master进程需要拥有较大的权限，例如需要利用root用户启动master进程。worker进程权限小于等于master进程。这样当任意一个worker进程出现错误导致coredump时，master进程会立刻启动新的worker进程。
-   多个worker进程处理请求不但可以提供服务的健壮性，最重要的是，可以充分利用当前的SMP多核架构，从而实现微观上真正的多核并发处理。对于Nginx，一个worker进程可以痛死处理的请求数只受限于内存大小，而且在架构设计上，不同的worker进程之间处理并发请求时几乎没有同步锁的限制，worker进程通常不会进入睡眠状态。因此，当Nginx上的进程数与CPU核心数相等时（最好每一个worker进程都绑定特定的CPU核心），进程间切换的代价是最小的。

## Nginx配置的通用语法

### 块配置项

块配置项由一个块配置项名和一对大括号组成：

~~~nginx
events{
    ...
}
http{
    upstream backend{
        server 127.0.0.1
    }
    gzip on;
    server{
        ...
            location /webstatic{
            gzip off
        }
    }
}
~~~

上面的events、http、server、location、upstream等都是块配置项，块配置项之后是否如`location /webstatic{...}`那样在后面带参数，取决于解析这个块配置项的模块。但块配置项一定会用大括号把一系列所属的配置项全包含进来，表示大括号内的配置项同时生效。所有的事件类配置都要在events块中，http、server等配置也遵循这个规定。

块配置项可以嵌套。内层块直接继承外层块。当内外层块中的配置发生冲突时，究竟是以内层块还是外层块的配置为准，取决于解析这个配置项的模块。

### 配置项的语法格式






