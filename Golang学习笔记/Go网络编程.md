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