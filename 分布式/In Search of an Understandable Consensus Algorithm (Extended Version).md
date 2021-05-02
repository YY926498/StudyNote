# In Search of an Understandable Consensus Algorithm (Extended Version)

Raft的特点：

- strong leader：Raft相比其他共识算法具有更强烈的领导力。日志条目只从领导者到其他服务器，简化了复制日志的管理。
- Leader election：Raft使用随机计时器选出Leader
- Membership changes：允许在配置更改期间仍然能够正常运行

## 复制状态机

复制状态机用于解决分布式系统中的各种容错问题。例如具有单个集群领导者的大型系统，如GFS、HDFS、RAMCloud。通常使用单独的复制状态机来管理领导选举，并存储当领导者宕机时必须要保留下来的配置信息。采用复制状态机的如Chubby和ZooKeeper。

复制状态机通常使用复制的日志实现。每个服务器存储一系列命令的日志，其状态机按顺序执行该命令。每个日志都包含相同顺序的相同命令，因此每个状态机处理相同的命令序列。由于状态机是确定的，因此每个机器具有相同的状态和相同的输出序列。

共识算法的功能是确保复制的日志一致。服务器上的公式模块是从客户端接收命令并将其添加到日志中。并与其他服务器上的公式模块通信，确保即使某些服务器下线每个日志最终顺序是相同的。一旦正确复制命令，每个服务器的状态机在日志顺序中处理它们，输出返回给客户端。

共识算法通常具有以下属性：

- 在所有非拜占庭条件下，确保安全，包括网络延迟、分区和数据包丢失、复制和重新排序
- 只要大多数服务器在工作，并且能够互相通信和与客户端通信，就可以完全发挥作用。服务器如果因为停机而掉线，稍后可以从硬盘上读取状态重新加入集群。
- 不依赖时间来确保日志的一致性，这是因为错误的时间和极端的消息延迟在最坏的情况下可能导致可用性问题。
- 通常情况下，只要集群中的大多数对单个RPC做出响应，命令就可以完成，少数慢服务器不影响整体系统性能。

## Paxos的问题

Paxos定义了一个协议，该协议能够将单个决策（如单个复制的日志条目）达成一致。这个子集称为单一法令Paxos。然后Paxos将该协议的多个实例组合起来，以促进一系列决策。Paxos确保安全性和活动性，并且支持集群成员的更改。

Paxos有两个明显的缺点：

- Paxos非常难以理解。假设Paxos的不透明性源于它选择单一法令子集作为其基础。单一法令的Paxos是密集而微妙的：分为两个阶段，没有简单的直观解释，不能独立理解。因此，很难直观地理解为什么单一法令协议能起作用。multi-Paxos的组合规则显著增加了复杂性和微妙性。
- 没有为构建实际实现提供良好基础。一个原因是，对于multi-Paxos没有得到广泛认可的算法。

另外由于单一法令的分解的原因，Paxos架构很难构建。例如，独立地选择一组日志条目，然后将它们合并成一个连续的日志，并没有什么好处，只是增加了复杂性。围绕日志设计系统更简单、更高效，其中新条目接受约束的顺序依次添加。另一个问题是Paxos在其核心使用了Peer-to-Peer。这在一个只有一个决策的简化世界中是有意义的，但是很少有实际系统使用这种方法。如果必须要做一系列的决定，那么首先选举一个领导，然后让领导协调决定，这样做比较简单快捷。

”Paxos算法的描述与真实系统的需求之间存在显著差距，最终的系统将基于一个未经证实的方案。“

## 设计可理解性

设计Raft时，必须为系统构建提供一个完整而实用的基础，从而大大减少开发人员的设计工作量；必须在所有条件下都是可行的，并且在典型操作条件下是可用的；对于一般的操作，必须是有效的。最重要也是最困难的挑战是理解。必须能够让大量的观众舒服地理解算法。

采用如下两种方法来分析是否便于理解：

- 问题分解方法：只要可能，将问题划分成可以相对独立地解决、解释和理解的独立部分。
- 通过减少要考虑的状态的数量来简化状态空间，使系统更加一致并尽可能消除不确定性。具体来说，日志不允许中间有间隔，并且Raft限制了日志之间可能不一致的方式。尽管在大多数情况下，试图消除非确定性，但在某些情况下，非确定性实际上提高了可理解性。随机化方法引入了非确定性，倾向于通过以类似的方式处理所有可能的选择来减少状态空间。

## Raft一致性算法

Raft是一种管理复制日志的算法。Raft首先选出一位leader，然后赋予leader管理复制日志的完全责任来实现共识。leader接受来自客户端的日志条目，将它们复制到其他服务器上，并告诉服务器何时可以安全地将日志条目应用到它们的状态机。

存在单一leader简化了对复制日志的管理。例如，leader可以决定在日志中放置新条目的位置，而不需要咨询其他服务器，数据以一种简单的方式从leader流到其他服务器。单一leader可以宕机或与其他服务器断开连接，此时，将选举一个新的leader。

鉴于leader方法，Raft将共识问题分解为三个相对独立的子问题：

- leader选举：当现有leader失败时，必须选出新的leader
- 日志复制：leader必须接受来自客户端的日志条目，并在整个集群中复制它们，迫使其它日志与自己的一致。
- 安全：Raft的关键安全属性是状态机安全属性：如果任何服务器对其状态机应用了特定的日志条目，那么其他服务器就不能对相同的日志索引应用不同的命令。

#### 状态

所有服务器上的持久状态（在响应RPCs之前对稳定存储进行更新）：

- `currentTerm`：服务器看到的最新的term。第一次初始化为0，单调增加
- `votedFor`：当前任期内获得投票的`candidatId`（候选者Id），如果没有则为null
- `log[]`：日志条目，每个条目包含状态机的命令，以及leader接收到这条目时的term（第一个索引是1）

所有服务器上的不稳定状态：

- `commitIndex`：已知提交的最高日志项的索引（初始化为0，单调增加）
- `lastApplied`：应用于状态机的最高日志条目的索引（初始化为0，单调地增加）

leader的不稳定状态（初始化后的选举）：

- `nextIndex[]`：对于每一个服务器，发送到该服务器的下一个日志条目的索引（初始化为leader最后一个日志索引+1）
- `matchIndex[]`：对于每一个服务器，已知要在服务器上复制的最高日志条目的索引（初始化为0，单调地增加）

#### RequestVote RPC

由候选人调用，收集选票

参数：

- term：候选人的term
- candidateId：请求投票的候选人
- lastLogIndex：候选人最后一个日志条目的索引
- lastLogTerm：候选人最后一个日志条目的term

结果：

-  term：currentTerm，以便候选人更新自身
- voteGranted：true表示候选人获得了选票

接收实现逻辑：

1. 如果term<currentTerm，返回false（如果候选人比我的term还小，直接拒绝它）
2. 如果votedFor==null或者候选人的日志至少比接受者新，则给它投票要投票的人的日志不能比我还少，Raft总想选择日志最多的节点作为Leader）

#### AppendEntries RPC

由leader调用来复制日志条目，也可用于心跳

参数：

- term：leader的term
- leaderId：所有follower可以重定向的客户端
- prevLogIndex：紧挨着新条目的日志条目的索引
- prevLogTerm：prevLogIndex的term
- entries[]：要存储的日志条目（空为心跳，可能为了效率发送多个）
- leaderCommit：leader的commitIndex

结果：

- term：currentTerm，由leader更新
- success：如果follower包含匹配prevLogIndex和prevLogTerm，则为true

接收实现逻辑：

1. 如果term<currentTerm，则返回false
2. 如果prevLogIndex位置的日志所属的任期和prevLogTerm不匹配，返回false（Raft要求，相同位置有相同日志）
3. 如果一个现有的条目和一个新的条目冲突(相同的索引但是不同的任期)，删除现有的条目和它后面的所有条目(5.3)（这里Raft发现问题之后，强制同步日志）
4. 追加任何尚未在日志中出现的新条目
5. 如果leaderCommit > commitIndex，设置 commitIndex = min(leaderCommit, index of last new entry)（有可能有节点没有复制完，那么全局的提交位置是最小值）

#### Rules for servers

所有节点适用的规则：

- 如果commitIndex>lastApplied：增加lastApplied，将log[lastApplied]应用到状态机
- 如果RPC请求或者响应中的term  T>currentTerm，设置currentTerm=T，并将自身转换为follower。（收到的RPC请求或响应中包含的term比我当前term大，说明自身不是leader，转为follower）。

followers：

- 响应候选人和领导的要求
- 如果选举超时，没有收到当前leader的AppendEntries RPC（超过选举超时时间没有收到心跳就转为候选人，目的为了重新开始选举，长期收不到心跳可能是leader宕机）或授予候选人投票：转换为候选人

Candidates：

- 转为候选人，开始选举
  - 增加currentTerm
  - 给自己投票
  - 重置选举定时器
  - 发送RequestVote RPCs到所有服务器
- 如果从大多数服务器获得票数：成为leader
- 如果从新领导着接收到AppendEntries RPC：转换为follower
- 如果选举超时，开始新的选举

leaders：

- Upon election：发送初始空的AppendEntries RPCs（心跳）到每个服务器；在空闲期间重复，防止选举超时
- 如果从客户端接收到命令：追加条目到本地日志，在条目应用到状态机后进行响应
- 如果follower的最后一个日志条目（last log index）>= nextIndex：发送AppendEntries RPC，其中日志条目从nextIndex开始
  - 如果成功：更新follower的nextIndex和matchIndex
  - 如果由于日志不一致导致AppendEntries失败：递减nextIndex并重试
- 如果存在一个N，使得N>commitIndex，并且matchIndex数组中大多数元素都满足matchIndex[i]>=N，并且log[N].term==currentTerm：设置commitIndex=N（当前term下，大多数位置N后面的日志条目都已经复制到了其他节点上，那么就可以认为N位置之前的都可以提交了。注意：复制日志条目和提交不同，有可能存在复制了日志条目，但是最后没有设置commitIndex=N的情况）

Raft重要属性：

- Election Safety：在给定的term中最多只能选举一位leader
- Leader Append-Only：leader从不覆盖或删除日志中的条目，只添加新条目
- Log Matching：如果两个log中的一个条目具有相同的term和index，那么它们之前的log应该是一致的
- Leader Completeness：如果一个log条目在一个term中被committed，那么它会出现在所有更高term的leader的log中。
- State Machine Safety：如果一个server已经把一个日志条目应用到它的状态机，那么没有其他的server可以应用一个具有同样的term和index的不同日志条目。

### Raft基础知识

一个Raft集群包含多个服务器，5是一个典型数字，允许系统容忍两次故障。在任何给定的时间，每个服务器都处于三种状态之一：leader、follower或candidate。在正常运行中，只有一个leader，其他所有服务器都是follower。follower是被动的：自己不发出任何请求，只是回应leader和candidate的请求。leader处理所有客户端请求（如果客户端联系到一个follower，follower将它重定向到leader）。candidate用于选举新leader。

Raft将时间划分为任意长度。term用连续整数进行编号。每个term以选举开始，其中一名或多名candidate试图成为leader。如果candidate赢得了选举，那么将在剩下的term内担任leader。某些情况下，选举导致分裂投票。此时，term将会以没有leader结束，新的term即将开始。Raft确保在一个给定的term内最多有一个leader。

不同的服务器可能会在不同的时间观察到term之间的转换，在某些情况下，服务器可能不会观察到选举甚至整个term。term在Raft中充当逻辑时钟，允许服务器检测过时的信息，比如过时的leader。每个服务器存储一个currentTerm，随着时间单调地增加。currentTerm在服务器通信时进行交换，如果一个服务器的currentTerm小于另一个服务器的，那么将currentTerm更新为较大的值。

如果一个candidate或leader发现自身的term过时，立即恢复到follower状态。如果一个服务器接收到一个过期term的请求，拒绝该请求。

Raft服务器使用RPC进行通信，基本共识算法只需要两种RPC：RequestVote RPC在选举期间由candidate发起，AppendEntries RPC由leader发起，以复制日志条目并提供一种心跳的形式。另外添加用于在服务器之间传输快照的RPC。

### Leader election

Raft使用心跳机制来触发leader election。
