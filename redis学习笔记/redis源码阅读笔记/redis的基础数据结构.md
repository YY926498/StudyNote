# redis的基础数据结构

## sds-一个C风格动态字符串

分为sdshdr5、sdshdr8、sdshdr16、sdshdr32和sdshdr64。结构如下：

~~~c
struct __attribute__ ((__packed__)) sdshdr64 {
    uint64_t len; /* used */
    uint64_t alloc; /* excluding the header and null terminator */
    unsigned char flags; /* 3 lsb of type, 5 unused bits */
    char buf[];
};
~~~

flags的低三位表示当前sds属于哪一种类型。

## dict-哈希表

redis哈希表结构如下：

~~~c
typedef struct dict {
    dictType *type;
    void *privdata;
    dictht ht[2];
    long rehashidx; /* rehashing not in progress if rehashidx == -1 */
    unsigned long iterators; /* number of iterators currently running */
} dict;
~~~

其中`dictType`表示当前哈希表使用的哈希函数、键/值的复制函数、比较函数以及“析构”函数。

`dictht`为装载数据的结构体。使用2个`dictht`，是为了在rehash的时候，能够渐进哈希，避免因为重哈希占用太多时间。

另外关于哈希还有其他类，类似于迭代器。

## ZIPLIST-压缩列表

结构如下：

~~~c
<zlbytes> <zltail> <zllen> <entry> <entry> ... <entry> <zlend>
~~~

存储时按照小端方式存储。

其中`zlbytes`四字节无符号整型，代表整个压缩列表的字节数，包括四字节自己。

`zltail`为四字节无符号整型，表示最后一项距离压缩列表头的偏移量。

`zllen`为两字节的无符号整型，表示整个压缩列表的节点数。最多是2\^16-2。如果超过2\^16-2，那么这一项被设置为2\^16-1，如果此时需要知道压缩列表的节点个数，需要遍历整个压缩列表。

`ZIPLIST ENTRIES`每个节点都包含两个固定元信息：前面一个节点的长度，当前节点的编码信息。具体结构如下：

~~~c
<prevlen> <encoding> <entry-data>
~~~

其中`prevlen`是变字节，当前面一个节点的长度小于254，则`prevlen`为1字节，如果前面一个节点的长度大于253，则`prevlen`为5字节，第一个字节为`0XFE`，表示分隔符。接下来四个字节表示前一个节点的长度。同样采用小端方式存储。

`encoding`规则：如果是字符串，前两位按照sds的flags规则，如果是整型，则全部设置为1，接下来两位表示整型的类别。

~~~c
|11000000| - 3 bytes  Integer encoded as int16_t (2 bytes).
~~~

压缩列表的最后一项固定为`0xFF`.

例子：

~~~c
 *  [0f 00 00 00] [0c 00 00 00] [02 00] [00 f3] [02 f6] [ff]
 *        |             |          |       |       |     |
 *     zlbytes        zltail    entries   "2"     "5"   end
~~~

