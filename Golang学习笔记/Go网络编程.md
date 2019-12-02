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
func ResolveIPAddr(net, addr string)(*IPAddr,os.Erro)
~~~

