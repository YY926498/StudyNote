# Go网络编程

## 基础知识

`net`包定义许多类型，函数，方法用于Go网络编程。`IP`类型被定义为一个字节数组：

~~~go
type IP []byte
~~~

处理`IP`类型的变量。

~~~GO
net.ParseIP(IP string)//将字符串转换成IP地址
~~~

**`IP`掩码**：

~~~go
type IPMask	[]byte
~~~

可以用一个4字节的`IPv4`地址来创建一个掩码：

~~~go
func IPv4Mask(a,b,c,d byte)IPMask
~~~

还可以通过`IP`的方法返回默认的掩码：

~~~go
func (ip IP)DefaultMask()IPMask
~~~

掩码的字符串形式是一个十六进制数，如掩码`255.255.0.0`是`ffff0000`。

掩码可以使用一个`IP`地址的方法，找到该`IP`地址的网络：

~~~go
func (ip IP)Mask(mask IPMask)IP
~~~

**`IPAddr类型`**：在`net`包的许多函数和方法会返回一个指向`IPAddr`的指针。这只是一个包含`IP`类型的结构体：

~~~go
type IPAddr struct{ 
    IP IP
    Zone string
}
~~~

这种类型主要是通过`IP`主机名执行`DNS`查找。

~~~go
func ResolveIPAddr(net, addr string)(*IPAddr,os.Error)
~~~

其中`net`是`ip`、`ip4`或者`ip6`其中的一个。

**主机查询**：`ResolveIPAddr`函数对某个主机名执行DNS查询，并返回一个简单的IP地址。然而，通常主机如果有多个网卡，则可以有多个IP地址。他们也可能有多个主机名，作为别名。

~~~go
func LookupHost(name string)(addrs []string,err os.Error)
~~~

这些地址将会被归类为`canonical`主机名。如果要找到规范名称，使用

~~~go
func LookupCNAME(name string)(cname string, err os.Error)
~~~

**查询服务端口**

~~~go
func LookupPort(network,service string)(port int, err os.Error)
~~~

`network`是一个例如`tcp`或者`udp`，`service`也是一个字符串，如`telnet`或者`domain`（DNS）。

### `TCPAddr`类型

~~~go
type TCPAddr struct{
    IP IP
    Port int
}
~~~

~~~go
func ResolveTCPAddr(net, addr string)(*TCPAddr,os.Error)
~~~

`net`是`tcp`，`tcp4`或者`tcp6`其中之一，`addr`是一个字符串，由主机名或IP地址，以及`:`后跟随着端口号组成：例如`www.google.com:80`或者`127.0.0.1:22`。如果地址是一个IPv6地址，由于已经有冒号，主机部分，必须放在方括号内，例如`[::1]:23`。另一种特殊情况是经常用于服务器，主机地址是0，因此，`TCP`地址实际上就是端口地址，例如`:80`用来表示`HTTP`服务器。

网络编程需要注意错误检查。可能的情况有：

1. 指定的地址中可能存在语法错误
2. 尝试连接到远程服务可能会失败。例如，所请求的服务可能没有运行，或者有可能是主机没有连接到网络
3. 虽然连接已经建立，如果连接突然丢失也可能导致写失败，或网络超时
4. 同样，读操作也可能会失败

针对`TCP`的读取操作，可能会有多个`TCP`数据包，需要不断地读，直到文件的末尾。在`io/ioutil`下的`ReadAll`函数考虑了这些问题，并返回完整的响应。

`TCP`客户端

~~~go
func DialTCP(net string, laddr, raddr *TCPAddr)(c *TCPConn, err os.Error)
~~~

其中`laddr`是本地地址，一般设置为`nil`。`raddr`是一个服务的远程地址，`net`是一个字符串，一般是`tcp`，`tcp4`，`tcp6`的其中的一个。

`TCP`服务器

~~~go
func ListenTCP(net string, laddr *TCPAddr)(l *TCPListener, err os.Error)
func (l * TCPListener)Accept()(c Conn, err os.Error) 
~~~

`net`参数可以设置为`tcp`，`tcp4`或者`tcp6`中的一个。如果要监听所有哦网络接口，可以将`IP`地址设置为0，如果端口设置为0，系统会自动选择一个端口。

### 多线程服务器

利用`goroutine`，达到并发。

### 控制`TCP`连接

~~~go
func (c *TCPConn)SetTimeout(nsec int64) os.Error
~~~

这是套接字读写前使用的超时设置

~~~go
func (c *TCPConn)SetKeepAlive(keepalive bool)os.Error
~~~

保活机制

### `UDP`

~~~go
func ResolveUDPAddr(net, addr string)(*UDPAddr, os.Error)
func DialUDP(net string, laddr, raddr * UDPAddr)(c *UDPConn, err os.Erro)
func ListenUDP(net string, laddr* UDPAddr)(c *UDPConn, err os.Error)
func (c *UDPConn)ReadFromUDP(b []byte)(n int, addr *UDPAddr, err os.Error)
func (c *UDPConn)WriteToUDP(b []byte, addr *UDPAddr)(n int, err os.Error)
~~~

针对`UDP`服务器，可以使用下列函数：

~~~go
func ListenPacket(net, laddr string)(c PacketConn, err os.Error)
~~~

这个接口的主要方法`ReadFrom`和`WriteTo`用来处理数据包的读取和写入。

`GO`的`net`包建议使用接口类型而不是具体的实现类型。但是，通过使用他们，就失去了具体的方法，比如`SetKeepAlive`或`TCPConn`和`UDPConn`的`SetReadBuffer`，除非类型转换。

## 数据序列化

客户端与服务之间通过数据交换来通信。因为数据可能是高度结构化的，所以在传输前必须进行序列化。

### 抽象语法表示法

~~~go
func Marshal(val interface{})([]byte, os.Error)
func Unmarshal(val interface{},b []byte)(rest []byte, err os.Error)
~~~

前一个将数据值编组成序列化的字节数组，后一个将其解组出来，需要对interface类型的参数进行更多的类型检查。

## HTTP

HTTP协议是无状态的，面向连接的和可靠的。

HTTP1.1新增：

1. 主机名识别（支持虚拟主机）
2. 内容协商（多语言）
3. 持久连接（降低TCP开销）
4. 分块传送
5. 字节范围（请求文件部分内容）
6. 代理支持

### 简单用户代理（`User agent`）

~~~GO
func Head(url string)(r *Response, err os.Error)
~~~

该函数可用来发起`HEAD`请求

### `GET`请求

希望接收到一个资源内容而不是有关信息

~~~go
func Get(url string)(r *Response, finalURL string, err os.Error)
~~~

### `Request`请求

~~~go
reuest, err := http.NewRequest("GET",url.String(),nil)
~~~

请求创建成功后，可以修改其内容字段。

~~~go
request.Header.Add("Accept-Charset","UTF-8;q=1,ISO-8859-1;q=0")
~~~

上述调用是指定只接受`UTF-8`。

### 客户端对象

向服务器发送一个请求并取得回复，最简单的方法是使用对象`Client`。此对象可以管理多个请求，并处理一些问题，如与服务器间的`TCP`连接是否保持活动状态等。

### 文件服务器

~~~go
fileserver := http.FileServer(http.Dir("/home"))

err:=http.ListenAndServer(":8000",fileserver)
~~~

### 处理函数

调用方式：

~~~go
func Handle(pattern string, handler Handler)
func HandleFunc(pattern string, handler func(*Conn, *Request))
~~~

### 绕过默认的`multiplexer`

Go服务器接收到的`HTTP`请求通常是由一个`multiplexer`进行处理的，检查`HTTP`请求的路径，然后调用一个合适的`handler`等等。

也可以定义自己的handler。将一个匹配模板参数和一个函数作为参数，调用`http.HandleFunc`，可以将其注册为默认的`multiplexer`。像`ListenAndServer`这样的函数可以使用`nil`作为`handler function`。

如果需要`multiplexer`，就可以给定一个非零函数作为handler function，这个函数将会全权负责管理请求和响应。

## 模板

`template`包描述了适用对象值改变了原来文本的方式从而在输入和输出时获取不同的文本。

命令规定如`{{...}}`。包括文本传输方式不变，以嵌入命令可以作用于和更改文本。

### 插入对象值

模板应用于Go对象中。Go对象的字段被插入到模板后，就能从域中“挖”到它的子域等等。当前对象以`:`代替，所以把当前对象当做字符串插入时，可以采用`{{.}}`的方式。默认采用`fmt`包来作为插入值的字符串输出。

要插入当前对象的一个字段的值，使用的字段名前加前缀`.`，例如：

~~~go
type Person struct{
    Namr
}
~~~

## XML

XML 是一种重要的标记语言，旨在把数据结构序列化成文本文档。

### 解析XML

Go有一个使用`NewParser`创建的XML解析器。这需要一个`io.Reader`作为参数，并返回一个指向`Parser`的指针。这个类型的主要方法是`Token`，这个方法返回输入流中的下一个标记。该标记是`StartElement`,`EndElement`,`CharData`,`Comment`,`ProcInst`和`Directive`其中的一种。

## RPC

1. 客户调用本地存根节点过程，存根节点会把参数打包成网络信息，这个信息被称为编组
2. OS内核里的网络通信函数会被存根节点调用来发送信息
3. 内核把消息传递给远端系统。这个可以使面向连接的或者是无连接传输模式
4. 服务器端的存根节点会把参数从网络消息中拆解出来
5. 服务器端的存根节点会执行一个本地过程调用
6. 等到过程完成，返回结果给服务器端的存根节点
7. 服务器存根节点会把返回值编组成网络消息
8. 消息被返回
9. 客户端存根节点用网络通信函数读取消息
10. 消息被拆解。然后返回值被放到本地程序的堆栈内。

### Go RPC

1. 函数必须是公共的（也就是首字母大写）
2. 有且仅有2个指针参数，第一个指向“接收器”-接收从client端发过来的数据值，第二个指向“发送器”-存放向client端发送的返回值
3. 有一个`os.Error`类型返回值

比如说：

~~~go
F(&T1,&T2)os.Error
~~~

所谓的对参数的限定指的是只需要定义数据类型。Go的RPC会用gob包来编组和解编组数据，所以对于参数类型，只需要按照gob规则定义即可。

HTTP RPC服务器

任何RPC系统都需要一个传输机制来跨网路地传递消息。Go可以用HTTP或TCP。用HTTP机制的优势是可以借助HTTP来支持库文件。可以在`HandleHTTP`在HTTP层上加入一个RPC处理器，然后启动一个HTTP服务器。

HTTP RPC客户端

客户端需要设置一个HTTP连接，来连接RPC服务器。客户端需要发起一个对RPC服务器的连接。它需要准备一个包含要发送数据的结构体，以及一个接受返回值的变量地址。之后就可以用参数来调用。参数如下：

1. 想要调用的远程函数的名字
2. 被发送的数据结构体
3. 储存返回值的变量地址

TCP RPC服务器



JSON RPC客户端

使用`jsonrpc`库

