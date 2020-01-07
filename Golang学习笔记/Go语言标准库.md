# Go语言标准库

## 输入输出

### io-基本的IO接口

io接口不一定是并行安全的。

在io包中最重要的两个接口：`Reader`和`Writer`接口。

**Reader接口**

~~~go
type Reader interface{
    Read(p []byte)(n int, err error)
}
~~~

说明：

Read将len(p)个字节读取到p中。它返回读取的字节数n(0<=n<=len(p))以及任何遇到的错误。即使Read返回的n<len(p)，它也会在调用过程中使用p的全部作为暂存空间。若一些数据可用但不到len(p)个字节，Read会照例返回可用的数据，而不是等待更多数据。

当Read在成功读取n>0个字节后遇到一个错误或EOF，它就会返回读取的字节数。它会从相同的调用中返回（非nil)错误或从随后的调用中返回错误(同时n==0)。一般情况的一个例子就是Reader再输入流结束时会返回一个非零的字节数，同时返回的err不是EOF就是nil。无论如何，下一个Read都应当返回0，EOF。

调用者应当总在考虑到错误err前处理n>0的字节。这样做可以在读取一些字节，以及允许的EOF行为后正确的处理I/O错误。即：当Read方法返回粗偶无时，不代表没有读取到任何数据。调用者应该处理返回的任何数据，之后才处理可能的错误。

io.EOF变量的定义：`var EOF = errors.New("EOF")`，是error类型。根据reader接口的说明，在n>0且数据读完了的情况下，返回的error有可能是EOF也有可能是nil。

**Writer接口**

~~~go
type Writer interface{
    Write(p []byte)(n int, err error)
}
~~~

说明：

Write将len(p)个字节从p中写入到基本数据流中。它返回从p中被写入的字节数n(0<=n<=len(p))以及任何遇到的引起写入提前停止的错误。若Write返回的n<len(p)，它就必须返回一个非nil的错误。

*一般地，Go中接口的命名约定为：接口名以er结尾。注意，这里并非强行要求，完全可以不以er结尾。*

**ReaderAt接口**

~~~go
type ReaderAt interface{
    ReadAt(p []byte, off int64)(n int, err error)
}
~~~

说明：

ReadAt从基本输入源的偏移量off处开始，将len(p)个字节读取到p中。它返回读取的字节数n(0<=n<=len(p))以及任何遇到的错误。

当ReadAt返回的n<len(p)时，它就会返回一个非nil的错误来解释为什么没有返回更多的字节。在这一点上，ReadAt比Read更严格。

即使ReadAt返回的n<len(p)，它也会在调用过程中使用p的全部作为暂存空间。若一些数据可用但不到len(p)个字节，ReadAt就会阻塞直到所有数据都可用或产生一个错误、在这一点上ReadAt不同于Read。

若n\==len(p)个字节在输入源的结尾处由ReadAt返回，那么这是err\==EOF或者err==nil。

若ReadAt按查找偏移量从输入源中读取，ReadAt应当既不影响基本查找偏移量也不被它所影响。

ReadAt的客户端可对相同的输入源并行执行ReadAt调用。示例：

~~~go
reader := strings.NewReader("Go语言标准库")
p := make([]byte,6)
n,err := reader.ReadAt(p,2)
if err!=nil{
    panic(err)
}
fmt.Printf("%s, %d\n",p,n)
~~~



