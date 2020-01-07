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

输出：

~~~go
语言, 6
~~~

**WriteAt接口**

~~~go
type WriteAt interface{
    WriteAt(p []byte, off int64)(n int, err error)
}
~~~

说明：

WriteAt从p中将len(p)个字节写入到偏移量off处的基本数据流中。它返回从p中被写入的字节数n(0<=n<=len(p))以及任何遇到的引起写入提前停止的错误。若WriteAt返回的n<len(p)，它就必须返回一个非nil的错误。

若WriteAt按查找偏移量写入到目标中，WriteAt应当既不影响基本查找偏移量也不被它所影响。

若区域没有重叠，WriteAt的客户端可对相同的目标并行执行WriteAt调用。

~~~go
file, err := os.Create("writeAt.txt")
if err !=nil{
    panic(err)
}
defer file.Close()
file.WriteString("Golang中文社区")
n, err:=file.WriteAt([]byte("Go语言学习园地"),24)
if err != nil {
    panic(err)
}
fmt.Println(n)
~~~

**ReadFrom和WriteTo接口**

**ReadFrom定义如下**

~~~go
type ReadFrom interface{
    ReadFrom(r Reader)(n int64, err error)
}
~~~

说明：

ReadFrom从r中读取数据，直到EOF或发生错误。其返回值n为读取的字节数。除了io.EOF之外，在读取过程中遇到的任何错误也将被返回。

如果ReadFrom可用，Copy函数就会使用它。

注：ReadFrom方法不会返回err==EOF。

~~~go
file, err := os.Open("writeAt.txt")
if err != nil {
    panic(err)
}
defer file.Close()
writer := bufio.NewWriter(os.Stdout)
writer.ReadFrom(file)
writer.Flush()
~~~

**WriteTo定义如下**

~~~go
type WriteTo interface{
    WriteTo(w Writer)(n int64, err error)
}
~~~

说明：

WriteTo将数据写入w中，直到没有数据可写或发生错误。其返回值n为写入的字节数。在写入的过程中遇到的任何错误也将被返回。

如果WriteTo可用，Copy函数就会使用它。

**Seeker接口**

~~~go
type Seeker interface{
    Seek(offset int64, whence int)(ret int64, err error)
}
~~~

说明：

Seek设置下一次Read或Write的偏移量为offset，它的解释取决于whence:0表示相对于文件的起始处，1表示相对于当前的偏移，而2表示相对于结尾处。Seek返回新的偏移量和一个错误，如果有的话。

whence的值在os包中定义了相应的常量。

~~~go
const (
	SEEK_SET int = 0
    SEEK_CUR int = 1
    SEEK_END int = 2
)
~~~

**Closer接口**

~~~go
type Closer interface{
    Close() error
}
~~~

注意：以下用法是错误的：

~~~go
file,err := os.Open("student.txt")
defer file.Close()
if err != nil {
    ...
}
~~~

当student.txt不存在或找不到时，file.Close()会panic，因为file是nil。因此应该将`defer file.Close()`放在错误检查之后。

**ByteReader和ByteWriter接口**

这两个接口是读或写一个字节。

定义如下：

~~~go
type ByteReader interface {
    ReadByte()(c byte, err error)
}
type ByteWriter interface {
    WriteByte(c byte) error
}
~~~

这两个接口一般在二进制数据或归档压缩时用的比较多。

**ByteScanner、RuneReader和RuneScanner**

~~~go
type ByteScanner interface {
    ByteReader
    UnreadByte() error
}
~~~

该接口内嵌了ByteReader接口，UnreadByte方法的意思是：将上一次ReadByte的字节还原，使得再次调用ReadByte返回的结果和上一次调用相同，也就是说，UnreadByte是重置上一次的ReadByte。注意，UnreadByte调用之前必须调用了ReadByte，且不能连续调用UnreadByte。即：

~~~go
buffer := bytes.NewBuffer([]byte{'a','b'})
err ；= buffer。UnreadByte()
~~~

和

~~~go
buffer:=bytes.NewBuffer([]byte{'a','b'})
buffer.ReadByte()
err := buffer.UnreadByte()
err := buffer.UnreadByte()
~~~

err都非nil。

RuneReader接口和ByteReader类似，只是ReadRune方法读取单个UTF-8字符，返回其rune和该字符占用的字符数。



