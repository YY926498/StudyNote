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

最基本的配置项语法格式如下：

配置项名 	配置项值1	配置项值2	…;

配置项的构成部分：

首先，在行首的是配置项名，这些配置项名必须是Nginx的某一个模块想要处理的，否则Nginx会认为配置文件出现了非法的配置项名。配置项名输入结束后，将以空格作为分隔符。

其次是配置项值，可以是数字或字符串（也包括正则表达式）。针对一个配置项，既可以只有一个值，也可以包含多个值，配置项值之间仍然由空格符来分割。一个配置项对应的值究竟有多少个，取决于解析这个配置项的模块。

最后，每行配置的结尾需要加上分号。

注：如果配置项值中包括语法符号，比如空格符，那么需要使用单引号或双引号括住配置项值，否则Nginx会报语法错误。例如：

~~~nginx
log_format main '$remote_addr - $remote_user [$time_local]"$request"'
~~~

### 配置项的注释

如果一个配置项暂时需要注释掉，可以加`#`注释掉这一行配置。例如：

~~~Nginx
#pid logs/nginx.pid;
~~~

### 配置项的单位

当指定空间大小时，可以使用的单位包括：

-   K或者k	千字节
-   M或者m  兆字节

例如：

~~~Nginx
gzip_buffers 4 8k;
client_max_body_size 64M;
~~~

当指定时间时，可以使用的单位包括：

-   ms（毫秒）、s（秒）、m（分钟）、h（小时）、d（天）、w（周）、M（月）、y（年）

注：配置项后的值究竟是否可以使用这些单位，取决于解析该配置项的模块。如果这个模块使用了Nginx框架提供的相应解析配置项方法，那么配置项值才可以携带单位。

### 在配置中使用变量

有些模块运行在配置项中使用变量，如在日子记录部分，具体示例如下：

~~~nginx
log_format main '$remote_addr - $remote_user [$time_local]"$request"' '$status $bytes_sent "$http_referer" "$http_user_agent" "$http_x_forwarded_for"';
~~~

其中`remote_addr`是一个变量，使用它的时候前面要加上`$`符号。这种变量只有少数模块支持，并不是通用的。许多模块在解析请求时都会提供多个变量，以使其他模块的配置可以即时使用。

在执行configure命令时，已经把许多模块编译进Nginx中，但是否启用这些模块，一般取决于配置文件中相应的配置项。大部分模块都必须在nginx.conf中读取某个配置项后才会在运行时启用。

## Nginx服务的基本配置

Nginx在运行时，至少必须加载几个核心模块和一个事件类模块。这些模块运行时所支持的配置项称为基本配置-所有其他模块执行时都依赖的配置项。

基本配置项按照用户使用时的预期功能分成以下4类：

-   用于调试、定位问题的配置项
-   正常运行的必备配置项
-   优化性能的配置项
-   事件类配置项（有些事件类配置项归纳到优化性能类，虽然属于`events{}`块，但作用是优化性能）

有些配置项，即使没有显式地进行配置，也会有默认值。例如：`daemon`，即使在nginx.conf中没有进行配置，但也相当于打开了这个功能。对于这样的配置项，作者会在下面相应的配置项描述上加入一行“默认：”来进行说明。

### 用于调试进程和定位问题的配置项

1.  查看是否以守护进程方式运行Nginx

    语法：`daemon on|off`

    默认：`daemon on`

    守护进程是脱离终端并且在后台运行的进程。

2.  是否以`master/worker`方式工作

    语法：`master_process on|off`

    默认：`master_process on`

    如果用`off`关闭了`master_process`方式，就不会fork出worker子进程来处理请求，而是用master进程自身来处理请求。

3.  `error`日志的设置

    语法：`error_log /path/file level`

    默认：`error_log logs/error.log error`

    `error`日志是定位Nginx问题的最佳工具，可以根据需要妥善设置`error`日志的路径和级别。

    `/path/file`参数可以是一个具体的文件，此时最好将文件放到一个磁盘空间足够大的位置；也可以是`/dev/null`，这样就不会输出任何日志，这也是关闭`error`日志的唯一手段；也可以是`stderr`，日志就会输出到标准错误文件中。

    `level`是日志的输出级别，取值范围是`debug`、`info`、`warn`、`error`、`crit`、`alert`、`emerg`，从左至右依次增大。当设定为一个级别时，大于或等于该级别的日志都会被输出到`/path/file`文件中，小于该级别的日志则不会输出。

    **注**：如果日志级别设定到`debug`，必须在configure时加入`--with-debug`配置项。
    
4.  是否处理几个特殊的调试点
    
    语法：`debug_points [stop|abort]`
    
    Nginx在一些关键的错误逻辑中设置了调试点。如果设置为`stop`，那么Nginx的代码执行到这些调试点时会发出SIGSTOP信号用于调试。如果设置为abort，则会产生一个coredump文件，可以使用gdb来查看Nginx当时的各种信息。
    
5.  仅对指定的客户端输出debug级别的日志
    
    语法：`debug_connection [IP|CIDR]`
    
    这个配置项实际上属于事件类配置，因此，必须放在events{…}中才有效。值必须是IP地址或CIDR地址，例如：
    
    ~~~nginx
    events {
        debug_connection 10.224.66.14;
        debug_connection 10.224.57.0/24;
    }
    ~~~
    
    这样，仅仅来自以上IP地址的请求才会输出debug级别的日志，其他请求仍然沿用error_log中配置的日志级别。
    
6.  限制coredump核心转储文件的大小

    语法：`worker_rlimit_core size`
    
    这个选项可以限制core文件的大小，从而有效帮助用户定位问题。
    
7.  指定coredump文件生成目录

    语法：`working_directory path`

    worker进程的工作目录。需确保worker进程有权限向worker_directory指定的目录中写入文件。

    ### 正常运行的配置项

    1.  定义环境变量

        语法：`env VAR|VAR=VALUE`

        这个配置项可以让用户直接设置操作系统上的环境变量。例如：

        ~~~nginx
        env TESTPATH=/tmp/;
        ~~~

    2.  嵌入其他配置文件

        语法：`include /path/file`

        include配置项可以将其他配置文件嵌入到当前的nginx.conf文件中，它的参数既可以是绝对路径，也可以是相对路径（相对于nginx.conf所在的目录），例如：

        ~~~Nginx
    include mime.types
        include vhost/*.conf
        ~~~
    
        参数的值可以是一个明确的文件名，也可以是含有通配符`*`的文件名，同时可以一次嵌入多个配置文件。
    
    3.  pid文件的路径
    
        语法：`pid path/file`
    
        默认：`pid logs/nginx.pid;`
    
        保存master进程ID的pid文件存放路径。默认与configure执行时的参数`--pid-path`所指定的路径是相同的，也可以随时修改，但应确保Nginx有权在相应的目录中创建pid文件，该文件直接影响Nginx是否可以运行。
    
    4.  Nginx worker进程运行的用户及用户组
    
        语法：`user username [groupname]`
    
        默认：`user nobody nobody;`
    
        user用于设置master进程启动后，fork出的worker进程运行在哪个用户和用户组下。当按照`user username;`设置时，用户组名与用户名相同。
    
        若用户在configure命令执行时使用了参数`--user=username`和`--group=groupname`，此时，nginx.conf将使用参数中指定的用户和用户组。
    
    5.  执行Nginx worker进程可以打开的最大文件句柄数
    
        语法：`worker_rlimit_nofile limit`
    
        设置一个worker进程可以打开的最大文件句柄数。
    
    6.  限制信号队列
    
        语法：`worker_rlimit_sigpenging limit`
    
        设置每个用户发往Nginx的信号队列的大小。当某个用户的信号队列满了，这个用户再发送的信号量会被丢掉。
    
    ### 优化性能的配置项
    
    1.  Nginx  worker进程的个数
    
        语法：`worker_processes number`
    
        默认：`worker_processes 1;`
    
        在master/worker运行方式下，定义worker进程的个数。worker进程的数量会直接影响性能。每个worker进程都是单线程的进程，会调用各个模块以实现多种功能。如果这些模块不会出现阻塞式的调用，那么有多少CPU核心就配置多少个进程；反之，需要配置更多的进程。一般情况下，配置与内核数相等的worker进程，并且使用下面的`worker_cpu_affinity`配置来绑定CPU内核。
    
    2.  绑定Nginx worker进程到指定的CPU内核
    
        语法：`worker_cpu_affinity cpumask [cpumask...]`
    
        通过绑定，可以内核的调度策略上实现完全的并发。
    
        ~~~nginx
        worker_processes 4;
        worker_cpu_affinity 1000 0100 0010 0001;
        ~~~
    
        注：`worker_cpu_affinity`配置仅对Linux操作系统有效。Linux操作系统使用`sched_setaffinity()`系统的调用实现这个功能。
    
    3.  SSL硬件加速
    
        语法：`ssl_engine device`
    
        如果服务器上有SSL硬件加速设备，那么就可以进行配置以加快SSL协议的处理速度，用户可以使用OpenSSL提供的命令查看是否有SSL硬件加速设备：
    
        ~~~shell
        openssl engine -t;
        ~~~
    
    4.  系统调用gettimeofday的执行频率
    
        语法：`timer_resolution t`
    
        默认情况下，每次内核的事件调用`epoll、select、poll、kqueue`等返回时，都会执行一次gettimeofday，实现用内核的时钟来更新Nginx的缓存时钟。例如`timer_resolution 100ms;`表示至少每100ms才调用一次gettimeofday。
    
    5.  
    
    


