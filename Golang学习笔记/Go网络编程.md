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