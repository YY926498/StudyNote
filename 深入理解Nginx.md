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

5.  Nginx worker进程优先级设置

语法：`worker_priority nice`

默认：`worker_priority 0;`

优先级越高，进程分配到的时间片越大，因此优先级高的进程会占用更多的系统资源。优先级由静态优先级和内核根据进程执行情况所做的动态调整。nice值是进程的静态优先级，取值范围是-20~+19。-20是最高优先级，建议不小于-5。

### 事件类配置项
1.  是否打开accept锁

语法：`accept_mutex [on|off]`

默认：`accept_mutex on;`

accept_mutex是Nginx的负载均衡锁，这把锁可以让多个worker进程轮流地、序列化地与新的客户端建立TCP连接。当某一个worker进程建立的连接数量达到worker_connections配置的最大连接数的7/8时，会大大减少该worker进程试图建立新的TCP连接的机会，以此实现所有worker进程之上处理的客户端请求数尽量接近。

2.  lock文件的路径

语法：`lock_file path/file`

默认：`lock_file logs/nginx.lock`

accept锁可能需要这个lock文件，如果accept锁关闭，lock_file配置完全不生效。如果打开了accept锁，并且由于编译程序、操作系统架构等因素导致Nginx不支持原子锁，这时才会用文件实现accept锁，这样lock_file指定的lock文件才会生效。

3.  使用accept锁后到真正建立连接之间的延迟时间

语法：`accept_mutex_delay Nms`

默认：`accept_mutex_delay 500ms;`

在使用accept锁后，同一时间只有一个worker进程能够取得accept锁。这个accept锁不是阻塞锁，如果取不到会立刻返回。如果一个worker进程试图获取accept锁但是没有取到，至少要等accept_mutex_delay定义的时间间隔后才能再次试图获取锁。

4.  批量建立新连接

语法：`multi_accept [on|off]`

默认：`multi_accept off;`

当事件模型通知有新连接时，尽可能地对本次调度中客户端发起的所有TCP请求都建立连接。

5.  选择事件模型

语法：`use [kqueue|rtsig|epoll|/dev/poll|select|poll|eventpoll]`

Nginx会自动使用最适合的事件模型。

6.  每个worker的最大连接数

语法：`worker_connections number`

定义每个worker进程可以同时处理的最大连接数。

## 用HTTP核心模块配置一个静态Web服务器

   静态Web服务器的主要功能由`ngx_http_core_module`模块（HTTP框架的主要成员）实现。除了之前提到的基本配置项外，一个典型的静态Web服务器还会包括多个server块和location块。

所有的HTTP配置项都必须直属于http块、server块、location块、upstream块或if块等（HTTP配置项自然必须全部在http{}块之内，“直属于”是指配置项直接所属的大括号对应的配置块）。

### 虚拟主机与请求的分发

由于IP地址的数量有限，存在多个主机域名对应着同一个IP地址的情况，在nginx.conf中按照server_name并通过server块来定义虚拟主机，每个server块就是一个虚拟主机，只处理与之相对应的主机域名请求。这样一台服务器上的Nginx能以不同的方式处理不同主机域名的HTTP请求。

1.  监听端口

    语法：`listen address:port [default|default_server|[backlog=num|rcvbuf=size|sndbuf=size|accept_filter=filter|deferred|bind|ipv6only=[on|off]|ssl]]`

    默认：`listen 80;`

    配置块：server

    listen参数决定Nginx如何监听端口。例如：

    ```nginx
    listen 127.0.0.1;#不加端口时，默认监听80端口
    listen [:::a8c9:1234]:80;
    listen 443 default_server ssl;
    ```

    listen可用参数的意义：

    -   default：将所在的server块作为整个的Web服务的默认server块。如果该参数没有设置，就以在nginx.conf中找到的第一个server块作为默认的server块。
    -   default_server：同上。
    -   backlog=num：表示TCP中backlog队列的大小。默认为-1.表示不予设置。
    -   rcvbuf=size：设置监听句柄的SO_RCVBUF参数。
    -   sndbuf=size：设置监听句柄的SO_SNDBUF参数。
    -   accept_filter：设置accept过滤器，只对FreeBSD操作系统有效。
    -   deferred：设置该参数后，如果发起建立连接请求，并且完成了TCP的三次握手，内核也不会为了这次的连接调度worker进程来处理，只有用户真正发送请求数据时，内核才会唤醒worker进程处理这个连接。适用于大并发的情况下，减轻worker进程的负担。
    -   bind：绑定当前端口/地址对，如127.0.0.1:8080，只有对一个端口监听多个地址时才会生效。
    -   ssl：在当前监听的端口上建立的连接必须基于SSL协议。

2.  主机名称

    语法：`server_name name [...]`

    默认：`server_name “”;`

    配置块：server

    server_name后可以跟多个主机名称，如server_name www.testweb.com download.testweb.com;

    在开始处理一个HTTP请求时，Nginx会取出header头中的Host，与每个server中的server_name进行匹配，以此决定由哪一个server块来处理请求。有可能一个Host与多个server块中的server_name都匹配，这时会根据匹配优先级来实际处理server块。匹配优先级如下：

    1.  首先选择所有字符串完全匹配的server_name，如www.testweb.com。
    2.  其次选择通配符在前面的server_name，如*.testweb.com。
    3.  再次选择通配符在后面的server_name，如www.testweb.*。
    4.  最后选择使用正则表达式才匹配的server_name，如`~^\.testweb\.com$`。

    如果Host与所有的server_name都不匹配，按下列顺序选择处理server块：

    1.  优先选择在listen配置项后加入`[default|default_server]`的server块。
    2.  找到匹配的listen端口的第一个server块。

    如果server_name后跟着空字符串，那么表示匹配没有Host这个HTTP头部的请求。

3.  server_names_hash_bucket_size

    语法：`server_name_hash_bucket_size size`

    默认：`server_name_hash_bucket_size 32|64|128;`

    配置块：http、server、location

    为了提高快速找寻到相应的serve name的能力，Nginx采用散列表存储server name。

4.  server_name_hash_max_size

    语法：`server_name_hash_max_size size`

    默认：`server_name_hash_max_size 512;`

    配置块：http、server、location

    该选项影响散列表的冲突率。

5.  重定向主机名称的处理

    语法：`server_name_in_redirect on|off`

    默认：`server_name_in_redirect on;`

    配置块：http、server或者location

    该配置需要配合server_name使用。在使用on打开时，表示在重定向请求时会使用server_name里配置的第一个主机名代替原先请求中的Host头部，而使用off关闭时，表示在重定向请求时使用请求本身的头部。

6.  location

    语法：`location [=|~|~*|^~|@] /uri/ {...}`

    配置块：server

    location会尝试根据用户请求中的URI来匹配上面的/uri表达式，如果可以匹配，选择location{}块中的配置来处理用户请求。匹配方式如下：

    1.  =表示把URI作为字符串，以便于参数中的uri做完全匹配
    2.  ~表示匹配URI时是字母大小写敏感的
    3.  ~*表示匹配URI时忽略字母大小写问题
    4.  ^~表示匹配URI时只需要其前半部分与uri参数匹配即可
    5.  @表示仅用于Nginx服务内部请求之间的重定向，带有@的location不直接处理用户请求。
    6.  在uri参数里可以使用正则表达式

    注：location有顺序，当有多个location可以匹配时，只被第一个location采用。

    在最后一个location使用`/`作为参数，会匹配所有HTTP请求。

### 文件路径的定义

1.  以root方式设置资源路径

    语法：`root path`

    默认：`root html;`

    配置块：http、server、location、if

    例如，定义资源文件相对于HTTP请求的根目录

    ~~~Nginx
    location /download/ {
        root /opt/web/html/;
    }
    ~~~

    在上面的配置中，如果有一个请求的URI是`/download/index/test.html`，那么Web服务器将会返回服务器上`/opt/web/html/download/index/test.html`文件的内容。

2.  以alias方式设置资源路径

    语法：`alias path`

    配置块：location

    alias也是用来设置文件资源路径的，与root不同点主要在于如何解读紧跟location后面的uri参数，会使alias与root以不同的方式将用户请求映射到真正的磁盘文件上。如果一个请求的URI是`/conf/nginx.conf`，用户实际想访问的文件在`/usr/local/nginx/conf/nginx.conf`，两种方式如下：

    ~~~nginx
    #alias方式
    location /conf {
        alias /usr/local/nginx/conf/;
    }
    #root方式
    location /conf {
        root /usr/local/nginx/;
    }
    ~~~

    使用alias时，在URI想实际文件路径的映射过程中，已经把location后配置的`/conf`丢弃掉。这也是root可以放置到http、server、location或if块中，而alias只能放置到location块。

    alias后面还可以添加正则表达式，例如：

    ~~~nginx
    location ~ ^/test/(\w+)\.(\w+)$ {
        alias /usr/local/nginx/$2/$1.$2;
    }
    ~~~

    请求访问`/test/nginx.conf`时，Nginx会访问`/usr/local/nginx/conf/nginx.conf`中的内容。

3.  访问首页

    语法：`index file`

    默认：`index index.html`

    配置块：http、server、location

    有时，访问站点时的URI是`/`，这时一般是返回网站的首页。index后可以跟多个文件参数，Nginx将会按照顺序来访问这些文件，例如：

    ```nginx
    location / {
        root path;
        index /index.html /html/index.php /index.php;
    }
    ```

    接收到请求后，Nginx首先会尝试访问`path/index.html`文件，如果可以被访问，就直接范湖文件内容结束请求，否则再试图返回`path/html/index.php`文件的内容，以此类推。

4.  根据HTTP返回码重定向页面

    语法：`error_page code [code...] [= | =answer-code] uri|@named_location`

    配置块：http、server、location、if

    当对于某个请求返回错误码时，如果匹配上了`error_page`中设置的code，则重定向新的URI中。例如：

    ~~~nginx
    error_page 404	/404.html;
    error_page 502 503 504 http://example.com/forbidden.html;
    error_page 404 = @fetch;
    ~~~

    注意，虽然重定向了URI，但返回的HTTP错误码还是与原来的相同。可以通过`=`来更改返回的错误码，例如：

    ~~~nginx
    error_page 404 =200 /empty.gif;
    ~~~

    也可以不指定确切的返回错误码，而是由重定向后实际处理的真实结果来决定，这时只要把`=`后面的错误码去掉即可。

    如果不想修改URI，只想让这样的请求重定向到另一个location中进行处理，可以如下处理：

    ~~~nginx
    location / {
        error_page 404 @fallback;
    }
    location @fallback {
        proxy_pass http://backend;
    }
    ~~~

    这样，返回404的请求会被反向代理到`http://backend`上游服务器中处理。

5.  是否允许递归使用`error_page`

    语法：`recursive_error_page [on|off]`

    默认：`recursive_error_page off;`

    配置块：http、server、location

    确定是否允许递归地定义`error_page`

6.  try_files

    语法：`try_files path1 [path2] uri`

    配置块：server、location

    `try_files`后要跟若干个路径，如path1、path2、…，而且最后必须要有uri参数，意义如下：尝试按照顺序访问每一个path，如果可以有效地读取，就直接向用户返回这个path对应的文件结束请求，否则继续向下访问。如果所有的path都找不到有效的文件，就重定向到最后的参数uri上。因此，最后这个参数uri必须存在，而且应该是可以有效重定向。例如：

    ~~~nginx 
    try_files /system/maintenance.html $uri $uri/index.html $uri.html @other;
    location @other {
        proxy_pass http://backend;
    }
    ~~~

    如果前面的路径都找不到，就会反向代理到http://backend服务上，还可以指定错误码的方式与error_page配合使用。


### 内存及磁盘资源的分配

1.  HTTP包体只存储到磁盘文件中

    语法：`client_body_in_file_only [on|clean|off]`

    默认：`client_body_in_file_only off;`

    配置块：http、server、location

    当值为非off时，用户请求的HTTP包体一律存储到磁盘文件中，即使只有0字节也会存储为文件。当请求结束时，如果配置为on，则这个文件不会被删除（该配置一般用于调试、定位问题），但如果配置为clean，则会删除该文件。

2.  HTTP包体尽量写入到一个内存buffer中

    语法：`client_body_in_single_buffer on|off`

    默认：`client_body_in_single_buffer off;`

    配置块：http、server、location

    用户请求中的HTTP包体一律存储到内存buffer中。当然，如果HTTP包体的大小超过了`client_body_buffer_size`设置的值，包体还是会写入到磁盘文件中。

3.  存储HTTP头部的内存buffer大小

    语法：`client_header_buffer_size size`

    默认：`client_header_buffer_size 1k;`

    配置块：http、server

    该配置项定义了正常情况下Nginx接收用户请求中HTTP header部分时分配的内存buffer大小。

4.  存储超大HTTP头部的内存buffer大小

    语法：`large_client_header_buffers number size`

    默认：`large_client_header_buffers 4 8k;`

    配置块：http、server

    该配置项定义Nginx接收一个超大HTTP头部请求的buffer个数和每个buffer的大小。如果HTTP请求行的大小超过上面的单个buffer，则返回`Request URI too large (414)`。请求中一般会有多个header，每一个header的大小不能超过单个的buffer的大小，否则会返回`Bad request (400)`。另外，请求行和请求头部的总和也不能超过buffer个数\*buffer大小。

5.  存储HTTP包体的内存buffer大小

    语法：`client_body_buffer_size size`

    默认：`client_body_buffer_size 8k/16k;`

    配置块：http、server、location

    该配置项定义Nginx接收HTTP包体的内存缓冲区大小。

    注：如果请求中含有HTTP头部`Content-Length`，并且其标识的长度小于定义的buffer大小，那么Nginx会自动降低本次请求所使用的内存buffer，以降低内存消耗。

6.  

