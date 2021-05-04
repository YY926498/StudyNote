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

Raft使用心跳机制来触发leader election。当服务器启动时，默认为follower。只要服务器从leader或candidate接收到有效的RPC，就保持在follower。leader定期向所有followers发送心跳（没有日志条目的AppendEntries），维护它们的权限。如果follower在一段成为选举超时的时间内没有收到任何通信，会假设没有可行的leader，并开始选举一个新的leader。

为了开始选举，follower会增加其term，并将自身转为candidate状态。然后为自己投票，并并行地向集群中的每个其他服务器发送RequestVote RPC。candidate会一直维持在这个状态，直到发生以下三件事的一种：a)赢得选举；b)另一个服务器成为新的leader；c)一段时间过去没有任何服务器成为leader。

如果一个candidate在同一个term内获得了整个集群中大多数服务器的选票，那么就赢得了选举。每个服务器在一个任期内最多给投票给一个candidate，以先到先得的方式。多数决定原则确保最多一个candidate可以在一个特定的任期内赢得了选举（选举安全属性）。candidate一旦赢得选举，就会成为领袖。然后向其他所有服务器发送心跳信息，以获得其权威，防止新的选举。

在等待投票时，candidate可能从另一个服务器收到一个自称leader的AppendEntries RPC。如果leader的term（包括RPC）至少和candidate的当前term一样大，那么candidate认为leader是合法的，并转换为follower。如果RPC中的term小于candidate的当前term，那么candidate拒绝RPC并继续维持在candidate。

第三种可能的结果是，一个candidate既没有赢得选举，也没有输掉选举：如果选多follower同时成为candidate，选票可能被分割，因此没有一个candidate获得多数票。当发生这种情况时，每个candidate都将超时并通过增加term并启动另一轮RequestVote RPC来开始新的选举。然而，如果没有额外的措施，分裂投票可能会无限期地重复。

Raft使用随机的选举超时来确保很少出现分裂的投票，并且能够快速解决。为了防止选票分裂，选举超时是在固定的时间间隔（如150-300ms）中随机选择的。同样的机制也用于处理分裂的投票。每个candidate在选举开始时重新启动它的随机选举超时，并在开始下一个选举之前等待该超时过去，这降低了在新选举中再次出现分裂选票的可能性。

### 日志复制

一旦一个leader当选，就开始服务客户端的请求。每个客户端请求都包含一个由由复制状态机执行的命令。leader将该命令作为一个新条目添加到其日志中，然后并行地向每个其他服务器发出AppendEntries RPC，复制该条目。当条目被安全复制时，leader将条目应用到它的状态机，并将执行的结果返回给客户端。如果follower崩溃或运行缓慢，或者如果网络数据包丢失，leader将无限期地重试AppendEntries RPC（即使已经对客户端发出响应之后），直到所有follower最终存储所有日志条目。

每个日志条目存储一个状态机命令，以及leader接收条目时的term。日志条目中的term用于检测日志之间的不一致性，并确保某些属性。每个日志条目有一个整数索引，用来标识其在日志中的位置。

leader决定何时将日志条目应用到状态机是安全的，这样的条目称为提交。Raft保证提交的条目是持久的，最终由所有可用的状态机执行。一旦创建条目的leader在大多数服务器上复制了一个日志条目，这个日志条目就被提交。这会在leader的日志中提交所有之前的条目，包括之前的leader创建的条目。leader跟踪它知道要提交的最高索引，并将该索引包含在未来的AppendEntries RPC中（包括心跳），以便其他服务器最终发现。一旦follower知道一个日志条目被提交，就将这个条目应用到它的本地状态机（按日志顺序）。

Raft日志机制维护不同服务器上日志之间的高一致性。不仅简化了系统的行为，使其更易于预测，而且是确保安全的一个重要组成部分。Raft维护以下属性：

- 如果不同日志中的两个条目具有相同的索引和term，则存储相同的命令。
- 如果不同日志中的两个条目具有相同的索引和term，则之前所有条目的日志是相同的。

第一个属性源于：在特定的term中，一个leader最多创建一个带有给定日志索引的条目，而日志条目永远不会改变它们在日志中的位置。第二个属性由AppendEntries RPC执行的一个简单的一致性检查来保证。当发送AppendEntries RPC时，leader在其日志中包含条目的索引和term，该条目紧接在新条目之前。如果follower在其日志中没有找到相同索引和term的条目，将拒绝新的条目。因此，每当AppendEntries RPC成功返回时，leader就知道follower的日志通过新条目与它自己的日志是相同的。

在正常的操作过程中，leader和follower的日志保持一致，所以AppendEntries一致性检查不会失败。但是leader宕机可能会使日志不一致（旧的leader可能没有完全复制其日志中的所有条目）。这些不一致会导致一系列leader和follower崩溃。follower可能会缺少leader上存在的条目，也可能有额外的leader上不存在的条目，或者两者都有。

在Raft中，leader通过强制follower的日志复制自身的日志来处理不一致性。这意味着follower日志中的冲突条目将被leader日志中的条目覆盖。

为了使follower的日志与自己保持一致，leader必须找到两个日志一致的最新日志条目，删除follower日志中在该点之后的所有条目，并将该点之后leader的所有条目发送给follower。所有这些操作都是为了响应AppendEntries RPC执行的一致性检查。leader为每一个follower维护一个nextIndex，这是leader将发送给那个follower的下一个日志条目的索引。当一个leader首次掌权时，将所有nextIndex初始化为其日志中最后一个索引之后的索引。如果follower的日志与leader的不一致，AppendEntries一致性检查将在下一次AppendEntries RPC中失败。拒绝后，leader将nextIndex递减，并重试AppendEntries RPC。

如果需要，可以优化协议减少被拒绝的AppendEntries RPC的数量。如果当拒绝一个AppendEntries请求时，follower可以包含冲突项的term和该term内的第一个索引。有了这个信息，leader就可以递减nextIndex来绕过该term中所有冲突的条目，每个有冲突项的term都需要一个AppendEntries RPC，而不是每一个条目一个RPC。

由于这种机制，leader在掌权时不需要采取任何特殊动作来恢复日志的一致性。开始只有正常的操作，当AppendEntries一致性检查失败时，日志自动收敛。leader从不覆盖或删除自身日志中的条目。

这个日志复制机制展示了一致性属性：只要大多数服务器都是正常的，Raft就可以接受、复制和应用新的日志条目。通常情况下，一个新条目可以用一轮RPC复制到集群的大部分，单一的慢follower不会影响性能。

### 安全

#### 选举限制

在任何基于leader的一致性算法中，leader最终必须存储所有提交的日志条目。Raft使用一种简单的方法，保证从选举的那一刻起，每个新的leader上提交的所有条目都存在，不需要将这些条目转移给leader。这表示日志条目只向一个方向流动，从leader到follower，leader从不覆盖它们日志中的现有条目。

Raft使用投票过程防止一个candidate赢得选举，除非其日志包含所有已提交的条目。candidate必须获得集群中的大多数才能被选中，这意味着每个提交的条目至少出现在其中的一个服务器上。如果candidate的日志至少和大多数的其他日志一样是最新的，那么将保存所有提交的条目。RequestVote RPC实现了这个限制：RPC包含关于candidate日志的信息，如果投票者自己的日志比candidate的日志更新，拒绝对其投票。

Raft通过比较日志中最后一个条目的索引和term来确定两个日志中哪个更新。如果日志中有带有不同term的最后一个条目，那么带有较大的term的日志是最新的。如果日志的term相同，索引大的更新。

#### 提交之前的条目

leader知道一旦当前条目存储在多数服务器上，就会提交该项。如果一个leader在提交一个条目之前崩溃，未来的leader将尝试完成这个条目的复制。然而，一旦前一个条目存储在大多数服务器上，leader不能立即断定它已经提交。仍然存在旧的条目存储在多数服务器上，但仍然可以被未来的leader覆盖。

为了消除该问题，Raft不通过副本来提交之前term的日志条目。只有来自leader当前term的日志条目被多数副本提交。一旦当前term中的一个条目以这种方式提交，那么由于日志匹配属性，所有之前的条目都将间接提交。在某些情况下，leader可以安全地断定已提交了一个较旧的日志条目（比如该条目存储在每个服务器上），但为了简单起见，Raft采用更保守的方法。

因为当leader复制前一个term的条目时，日志条目保留了它们原来的term。Raft的方法使得对日志条目进行推理变得更容易，因为随着时间的推移和跨日志保持相同的term。另外，新leader发送的来自以前term的日志条目更少。

#### 安全参数

假设term为T时的leader为$leader_T$，该leader提交一个日志条目，但是未来的新leader没有对应存储。假设未来的新leader所在的term为U。其中$leader_U$没有存储这个对应的条目。

1.  在$leader_U$选举时，其日志中不存在提交的条目（leader从不删除或覆盖条目）
2.  $leader_T$将条目复制到集群的多数，并且$leader_U$收到了集群的多数投票。因此，至少有一个投票既接受了$leader_T$的条目，又给$leader_U$投票。这个重叠的服务器就是关键。
3.  投票的服务器在投票选举$leader_U$前必须接受$leader_T$提交的条目，否则，将拒绝来自$leader_T$的AppendEntries请求（其当前term大于T）
4.  投票的服务器在投票给$leader_U$时仍然存储条目，因为每个介入的leader都包含条目，leader从不删除条目，follower只在与leader冲突时删除条目。
5.  投票的服务器将自己的票投给$leader_U$，所以$leader_U$的日志必须和投票人一样是最新的。
6.  首先，如果投票的服务器和$leader_U$的上一个term相同。那么$leader_U$的日志必须至少和投票的服务器一样长，所以它的日志包含投票服务器日志中的每个条目。

leader completeness属性：如果一个服务器将给定索引的日志条目应用到状态机，那么其他的服务器将不会为同一索引应用到不同的日志条目。当服务器将一个日志条目应用到它的状态机时，它的日志必须通过该条目与leader的日志相同，并且必须提交该条目。log completeness属性保证所有具有更大term的leader将存储相同的日志条目，因此应用后面term的索引的服务器将应用相同的值。因此，状态机的安全属性保持不变。

最后，Raft要求服务器按照日志索引顺序应用条目。结合状态机安全属性，这意味着所有服务器将以相同的顺序对其状态机应用完全相同的日志条目集合。

### follower和candidate崩溃

follower和candidate崩溃的处理方式相同。如果follower或candidate崩溃，那么发送给它的RequestVote和AppendEntries RPC失败。Raft通过反复尝试来处理这些失败。如果崩溃的服务器重新启动，则RPC将成功完成。如果服务器在完成RPC后但在响应之前崩溃，那么在重新启动后再次收到相同的RPC。Raft的RPC是幂等的，没有副作用。如果一个follower接收到一个已经在其日志中的条目的AppendEntries请求，将忽略新请求中的那些条目。

### 时间和可用性

Raft的安全性不依赖时间：系统不能仅仅因为某些事件的发生比预期的快或慢就产生不正确的结果。然而，可用性必然依赖于时间。

leader选举是Raft的一个方面。其中时间最关键。只要系统满足以下时序要求，Raft就能选出并保持一个稳定的leader：

$broadcastTime << electionTimeout <<  MTBF$

$broadcastTime$是服务器向集群中的每个服务器并行发送RPC并接收它们的响应所需的平均时间；$electionTimeout$为选举超时；$MTBF$是单个服务器发生故障之间的平均时间。广播时间应该比选举超时时间小一个数量级，以便leader能够可靠地发送防止follower开始选举所需的心跳信息；考虑到选举超时使用的随机方法，这种不平等也使得分裂选票不太可能。选举超时时间应该比$MTBF$小几个数量级，这样系统才能稳定运行。当leader崩溃时，系统将在选举超时时间内不可用。

广播时间和$MTBF$是底层系统的属性，而选举超时是我们选择的。Raft的RPC通常要求接收方将信息持久化到稳定的存储中，因此广播时间可能从0.5ms到20ms不等，这取决于存储技术。因此，选举超时可能在10ms到500ms之间。典型的服务器$MTBF$是几个月或更长时间，因此时间要求比较容易满足。

## 集群成员变更

在实践中，有时需要更改配置。虽然可以通过使整个集群脱机。更新配置文件，然后重新启动集群来完成，但这将使集群在转换期间不可用。此外，有操作错误的风险。

为了确保配置变更机制的