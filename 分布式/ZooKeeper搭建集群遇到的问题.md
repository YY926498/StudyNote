# ZooKeeper搭建集群遇到的问题

## 配置文件

在`zoo.cfg`配置文件中，`dataDir`指示数据文件的目录，修改为合适的目录后，在`dataDir`目录下建立`myid`文件，在`myid`文件中写入ZooKeeper服务器的序号。比如`1`。

在`zoo.cfg`文件末尾加入ZooKeeper集群中各个服务器的ip地址和端口，比如：

~~~sh
server.1=127.0.0.1:2222:2223
server.2=127.0.0.1:3333:3334
server.3=127.0.0.1:4444:4445
~~~

如果在同一台服务器上开的ZooKeeper集群，需要修改每个ZooKeeper服务器的客户端端口，避免多个ZooKeeper服务器监听同一个端口，如果不在同一台服务器上，可以设置为同一个端口。

## 启动ZooKeeper服务器

在启动前确保客户端端口没有被其他进程使用，可以使用`lsof -i:2181`查看端口2181是否被使用，如果被使用，可以修改`zoo.cfg`配置文件或者`kill -9 pid`。

启动ZooKeeper服务器：`./zkServer.sh start-foreground zoo.cfg`。使用`start-foreground`选项，可以在启动服务器后，能实时看到日志输出



在刚启动服务器的时候，由于没有达到法定人数，因此无法选举Leader，因此会尝试连接到其他服务器，一旦开启的服务器数量不小于法定人数后，就会选举出领导者。此时的日志如下：

Leader：

~~~sh
2020-10-01 08:09:15,199 [myid:2] - INFO  [WorkerReceiver[myid=2]:FastLeaderElection$Messenger$WorkerReceiver@389] - Notification: my state:LEADING; n.sid:3, n.state:LOOKING, n.leader:3, n.round:0x1, n.peerEpoch:0x0, n.zxid:0x0, message format version:0x2, n.config version:0x0
~~~

Following：

~~~sh
2020-10-01 08:09:15,208 [myid:1] - INFO  [WorkerReceiver[myid=1]:FastLeaderElection$Messenger$WorkerReceiver@389] - Notification: my state:FOLLOWING; n.sid:3, n.state:LOOKING, n.leader:3, n.round:0x1, n.peerEpoch:0x0, n.zxid:0x0, message format version:0x2, n.config version:0x0
~~~

客户端可以使用如下命令连接ZooKeeper服务器集群：

~~~sh
./zkCli.sh -server 127.0.0.1:2181,127.0.0.1:2182,127.0.0.1:2183
~~~

客户端会以随机顺序连接串中的服务器。