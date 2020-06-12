# server.c文件分析

## redisCommand

在server.c文件开始，就有redisCommand数组，里面定义了redis支持的各种命令。redisCommand是一个结构体。里面定义了命令的名字，处理函数指针以及其他部分。

## main

首先是初始化随机种子和获取时间，以及其他的预备工作。

然后进入`initServerConfig`函数，开始配置服务器。

### initServerConfig

服务器的频率默认为10Hz。查询缓冲最大设为1GB，以及其他。



### 各种初始化

根据传入的参数，可能会校验rdb文件或者aof文件

### 守护进程

如果开启了后台进程选项或者supervised选项，就会调用守护进程函数`daemonize`。具体步骤是：

-   创建子进程，退出父进程，此时子进程不是进程组的组长
-   创建新会话，子进程称为新会话的首进程，脱离之前的终端
-   将标准输入、标准输出、标准错误三个IO都重定向到“/dev/null”
-   将其他IO全部关闭

**但是按照APUE的写法，应该再创建一个子进程，然后退出之前的子进程，使最后的进程不是会话组的组长，但是redis这里并没有这一步，后续查阅资料后，再来回答。**

### initServer

避免进程被杀死，将SIGHUP和SIGPIPE信号屏蔽。并且安装其他信号处理函数。

创建ae事件循环。其中ae事件循环根据不同的平台，调用对应的IO多路复用函数，主要有：select、poll、epoll和kqueue。具体信息会单独拿出来分析。

redis的数据库默认是16个。在初始化的时候，会为每个数据库都创建：键哈希表、过期键的哈希表、阻塞的键的哈希表以及其他。

创建pub/sub通道、模式等

配置redis pipe相关属性

**创建定时器回调函数**：每隔1ms执行一次`serverCron`函数。

-   serverCron函数：
    -   如果开启了看门狗，需要重置看门狗
    -   更新缓存时间
    -   如果客户端的数量较多，增大server.Hz。
    -   更新LRU时钟
    -   清理过期键、重置大小、重哈希等
    -   如果没有子进程，后台重写AOF
    -   每100个周期调用一下`clusterCron`，后面会单独分析
    -   如果没有足够的任务，可以阻塞一下线程

将TCP监听套接字和UNIX域套接字加入到ae事件循环中，并且将`accept`作为回调函数。

注册`beforeSleep`和`afterSleep`函数。

其他服务初始化

### InitServerLast

-   BioInit：初始化3个后台线程以及对应的锁和条件变量，每个后台线程都有一个独有的任务列表。
-   initThreadedIO：创建IO线程，与后台线程类似，都有对应的锁和任务列表，但是io线程数是可配置的。

### aeMain

这是整个redis的核心处理函数。里面会不断循环调用`aeProcessEvents`函数。

找出最近的一个定时器的回调函数

每次都会先执行beforeSleep函数

调用`aeApiPoll`函数

调用`afterSleep`函数

先执行可读事件

执行可写事件

执行定时器的回调函数

### aeDeleteEventLoop

删除事件循环

### 退出

