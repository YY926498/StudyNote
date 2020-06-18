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

`encoding`规则：如果是字符串，根据后续数据的长度不同，前两位分别对应00,01,10，然后剩下6位，14位，38位代表后续的长度。如果是整型，则全部设置为1，接下来两位表示整型的类别。

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

## quicklist-快速列表

快速列表是通过双向列表和压缩列表结合在一起形成，其中每个节点构成双向列表，每个节点内部指向一个压缩列表。这样既可以享受压缩列表带来的内存高效利用，又尽量避免压缩列表插入和删除导致的连锁更新。

结构如下：

~~~c
typedef struct quicklist {
    quicklistNode *head;
    quicklistNode *tail;
    unsigned long count;        /* total count of all entries in all ziplists */
    unsigned long len;          /* number of quicklistNodes */
    int fill : QL_FILL_BITS;              /* fill factor for individual nodes */
    unsigned int compress : QL_COMP_BITS; /* depth of end nodes not to compress;0=off */
    unsigned int bookmark_count: QL_BM_BITS;
    quicklistBookmark bookmarks[];
} quicklist;
~~~

count代表整个列表中元素的总个数，list代表快速列表节点的个数，fill代表每个快速列表节点的压缩列表的长度，负数根据相应规则代表最大长度，正数代表最大个数。compress代表两端各有多少个节点不用压缩。

其中每个节点的结构如下：

~~~c
typedef struct quicklistNode {
    struct quicklistNode *prev;
    struct quicklistNode *next;
    unsigned char *zl;
    unsigned int sz;             /* ziplist size in bytes */
    unsigned int count : 16;     /* count of items in ziplist */
    unsigned int encoding : 2;   /* RAW==1 or LZF==2 */
    unsigned int container : 2;  /* NONE==1 or ZIPLIST==2 */
    unsigned int recompress : 1; /* was this node previous compressed? */
    unsigned int attempted_compress : 1; /* node can't compress; too small */
    unsigned int extra : 10; /* more bits to steal for future usage */
} quicklistNode;
~~~

`zl`指针指向的就是压缩列表。如果compress不为0，zl指向quickLZF结构。结构如下：

~~~c
typedef struct quicklistLZF {
    unsigned int sz; /* LZF size in bytes*/
    char compressed[];
} quicklistLZF;
~~~

如果数据小于48字节，不会进行压缩，否则会调用`lzf_compress`函数进行压缩。

## intset

结构如下：

~~~c
typedef struct intset {
    uint32_t encoding;
    uint32_t length;
    int8_t contents[];
} intset;
~~~

一般用于set中全是整数的情形。分为：`INTSET_ENC_INT16` 、 `INTSET_ENC_INT32 `、`INTSET_ENC_INT64`三种。采用二分查找。