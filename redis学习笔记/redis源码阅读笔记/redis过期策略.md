# redis过期策略

## 删除键 - expire.c

主要的一个函数为`activeExpireCycle`，这个函数有两种执行方式，一种是`ACTIVE_EXPIRE_CYCLE_FAST`，另一种是`ACTIVE_EXPIRE_CYCLE_SLOW`。

-   `ACTIVE_EXPIRE_CYCLE_FAST`：主要调用方式是每次ae事件循环的时候，在调用`aeApiPoll`之前，会调用`beforeSleep`函数。在`beforeSleep`函数里，如果开启了`server.active_expire_enabled`选项，就会进入过期键函数中。此时，该函数调用不会超过`EXPIRE_FAST_CYCLE_DURATION`
-   `ACTIVE_EXPIRE_CYCLE_SLOW`：在定时函数`serverCron`中的`databasesCron`函数中如果开启了`server.active_expire_enabled`选项，会调用过期键函数。**这里有个疑问：定时函数根据源码来看是1ms调用1次，但是serverCron的注释却是跟随server.Hz**。

`activeExpireCycle`源码分析：

如果是`ACTIVE_EXPIRE_CYCLE_FAST`选项，确定最长可调用时间。

针对每个数据库(redis单实例有16个数据库)，遍历`db->expires`，对每个具有过期时间的键，调用`activeExpireCycleTryExpire`，看当前时间是否大于过期时间，如果大于，就进行删除。如果开启`server.lazyfree_lazy_expire`选项，会将删除过程放在后台线程中（后台线程一般会开启3个，一个负责关闭文件，一个负责AOF_FSYNC，一个负责懒惰删除），否则就直接删除过期键。

每次执行16次循环，就会判断函数执行时间是否过长，如果过长，为了避免阻塞，就会退出循环。



## evict.c

`freeMemoryIfNeeded`函数是过期策略的一个重要函数

调用过程：

每次接收到客户端的命令，调用`processCommand`函数。

如果设置了`server.maxmemory && !server.lua_timedout`，就会调用`freeMemoryIfNeededAndSafe`函数

里面调用`freeMemoryIfNeeded`函数

里面根据redis的过期策略执行相应动作。

`MAXMEMORY_NO_EVICTION`：循环等待后台线程删除一些键，直到有空闲内存

`MAXMEMORY_FLAG_LRU`、`MAXMEMORY_FLAG_LFU`或者`MAXMEMORY_VOLATILE_TTL`：根据对应的规则在设置了过期键的哈希表或者全部键中进行采样，一般是采样16个。

`MAXMEMORY_ALLKEYS_RANDOM`和`MAXMEMORY_VOLATILE_RANDOM`：针对是否设置`MAXMEMORY_ALLKEYS_RANDOM`，选择设置了过期键的哈希表或则全部键的哈希表，随机挑选键。

然后删除键，跟前面的类似，有懒惰删除和直接删除两种。