# 快速入门

## 消息队列功能介绍

分布式消息队列可以提供应用解耦、流量削峰、消息分发等功能。

### 应用解耦

复杂的应用里会存在多个子系统，如果子系统之间的耦合性太高，整体系统的可用性就会大幅降低。多个低错误率的子系统强耦合在一起，得到的是一个高错误率的整体系统。

### 流量削峰

通过利用消息队列，把大量的请求暂存起来，分散到相对长的一段时间内处理，能大大提高系统的稳定性和用户体验。 

### 消息分发

消息队列还保证最终一致性、方便动态扩容等功能。

## RocketMQ简介

