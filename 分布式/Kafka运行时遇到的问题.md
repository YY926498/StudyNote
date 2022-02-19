# Kafka运行时遇到的问题

1.  启动时，提示ZooKeeper的集群ID不符合

    原因：kafka配置日志路径，用来保存执行过程中的各种信息，当kafka异常关闭时，日志记录就会出现异常，会把当时的情况记录到meta.properties文件中，重新启动时此文件会对启动造成影响，kafka重启报错的原因就在这里。

    解决方法：

    -   清空日志目录【日志不重要或可以容忍日志丢失】
    -   调整日志目录【需要修改配置】
    -   删除日志目录下的meta.properties文件

2.  虚拟机运行Kafka，但是外部电脑无法连接到虚拟机中的Kafka，并且提示：`No connection could be made because the target machine actively refused it`错误

    原因：kafka启动时的配置有问题，可能没有开放对应的端口，可以查看`config/server.properties`文件中的`listeners=PLAINTEXT://192.168.2.184:9092`和`advertised.listeners=PLAINTEXT://192.168.2.184:9092`两个选项是否配置正确

    