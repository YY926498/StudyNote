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

**ReadCloser、ReadSeeker、ReadWriteCloser、ReadWriteSeeker、ReadWriter、WriteCloser和WriteSeeker接口**

这些接口是上面介绍的接口的两个或三个组合而成的新接口。例如：

~~~go
type ReadWriter interface{
    Reader
    Writer
}
~~~

这是Reader和Writer接口的简单组合（内嵌）

这些接口的作用是：有些时候同时需要某两个接口的所有功能，即必须同时实现了某两个接口的类型才能够被传入使用。

**SectionReader类型**

SectionReader是一个struct（没有任何导出的字段），实现了Read，Seek和ReadAt，同时，内嵌了ReadAt接口。结构定义如下：

~~~go
type SectionReader struct{
    r 		ReadAt
    base 	int64
    off		int64
    limit	int64
}
~~~

该类型读取数据流中部分数据。

~~~go
func NewSectionReader(r ReaderAt, off int64, n int64)*SectionReader
~~~

NewSectionReader返回一个SectionReader，它从r中的偏移量off处读取n个字节后以EOF停止。

这个类型的作用是：方便重复操作某一段section数据流；或者同时需要ReadAt和Seek的功能。

**LimitedReader类型**

~~~go
type LimitedReader struct{
    R 	Reader
    N 	int64
}
~~~

说明

从R中读取但将返回的数据量限制为N字节。没调用一个Read都将更新N来反映新的剩余数量。最多只能返回N字节数据。LimitedReader只实现了Read方法。

~~~go
content := "This is LimitedReader example"
reader := strings.NewReader(content)
limitReader := &io.LimitedReader{R: reader, N: 8}
for limitReader.N > 0 {
    tmp := make([]byte, 3)
    limitReader.Read(tmp)
    fmt.Println(tmp, limitReader.N)
}
~~~

输出：

~~~go
[84 104 105] 5
Thi 5
[115 32 105] 2
s i 2
[115 32 0] 0
s 
~~~

可见，通过该类型可以达到只允许读取一定长度数据的目的。

在io包中，。LimitReader函数的实现其实就是调用LimitedReader:

~~~go
func LimitReader(r Reader, n int64)Reader{return &LimitedReader{r,n}}
~~~

**PipeReader和PipeWriter类型**

PipeReader（一个没有任何导出字段的struct）是管道的读取端。它实现了io.Reader和io.Closer接口。

关于Read方法的说明：从管道中读取数据。该方法会堵塞，直到管道写入端开始写入数据或写入端关闭。如果写入端关闭时带上了error（即调用CloseWithError关闭），该方法返回的err就是写入端传递的error；否则err为EOF。

PipeWriter（一个没有任何导出的字段的struct）是管道的写入端。它实现了io.Writer和io.Closer接口。

关于Write方法的说明：写数据到管道中。该方法会阻塞，直到管道读取端读完所有数据或读取端关闭了。如果读取端关闭时带上了error（即调用CloseWithError关闭），该方法返回的err就是读取端传递的error；否则err为ErrClosedPipe。

io.Pipe()用于创建一个同步的内存管道，函数签名：

~~~go
func Pipe()(*PipeReader, *PipeWriter)
~~~

将io.Reader连接到io.Writer。一端的读取匹配另一端的写入，直接在这两端之间赋值数据；它没有内部缓存。它对于并行调用Read和Write以及其他函数或Close来说都是安全的。一旦等待的I/O结束，Close就会完成。并行调用Read或并行调用Write也同样安全：同种类的调用将按顺序进行控制。因此，不能再同一个goroutine中进行读或写。

**Copy和CopyN函数**

~~~go
func Copy(dst Writer, src Reader)(written int64, err error)
~~~

说明：

Copy将src复制到dst，直到在src上到达EOF或发生错误。它返回复制的字节数，如果有的话，还会返回在复制时遇到的第一个错误。

成功的Copy返回err\==nil，而非err==EOF。由于Copy被定义为从src读取直到EOF为止，因此它不会将来自Read的EOF当做错误来报告。

若dst实现了ReaderFrom接口，其复制操作可通过使用dst.ReadFrom(src)实现。此外，若src实现了WriteTo接口，其复制操作可通过调用src.WriteTo(dst)实现。

~~~go
io.Copy(os.Stdout,strings.NewReader("Go语言学习"))
~~~

**CopyN函数**

~~~go
func CopyN(dst Writer,src Reader, n int64)(written int64, err errno)
~~~

说明：

CopyN将n个字节从src复制到dst。它返回复制的字节数以及在复制时遇到的最早的错误。由于Read可以返回要求的全部数量及一个错误（包括EOF），因此CopyN也能如此。

若dst实现了ReaderFrom接口，复制操作就会使用它来实现。

**ReadAtLeast和ReadFull函数**

~~~go
func ReadAtLeast(r Reader, buf []byte,min int)(n int, err error)
~~~

说明：

ReadAtLeast将r读取到buf中，直到读取了最少min个字节为止。它返回复制的字节数，如果读取的字节较少，还会返回一个错误。若没有读取到字节，错误就只是EOF。如果一个EOF发生在读取了少于min个字节之后，ReadAtLeast就会返回ErrUnecpectedEOF。若min大于buf的长度，ReadAtLeast就会返回ErrShortBuffer。对于返回值，当且仅当err==nil时，才有n>=min。

**ReadFull函数**

~~~go
func ReadFull(r Reader, buf []byte)(n int, err error)
~~~

说明：

ReadFull精确地从r中将len(buf)个字节读取到buf中。它返回复制的字节数，如果读取的字节较少，还会返回一个错误。若没有读取到字节，错误就只是EOF。如果一个EOF发生在读取了一些但不是所有的字节后，ReadFull就会返回ErrUnexpectedEOF。对于返回值，当且仅当err\==nil时，才有n==len(buf)。

**WriteString函数**

~~~go
func WriteString(w Writer, s string)(n int, err error)
~~~

**MultiReader和MultiWriter函数**

~~~go
func MultiReader(readers ...Reader)Reader
func MultiWriter(writers ...Writer)Writer
~~~

MultiReader只是逻辑上将多个Reader组合起来，并不能通过调用一次Read方法获取所有Reader的内容。在所有的Reader内容都被读取完成后，Reader会返回EOF。

如果是MultiWriter，就会将一份内容复制到多个Writer中，同时输出。

**TeeReader函数**

~~~go
func TeeReader(r Reader, w Writer) Reader
~~~

该函数返回一个Reader，它将从r中读到的数据写入到w中。所有经由它处理的从r的读取都匹配于对应的对w的写入。它没有内部缓存，即写入必须在读取完成前完成。任何在写入时遇到的错误都将作为读取错误返回。

### ioutil-方便的IO操作函数集

**NopCloser函数**

包装一个io.Reader，返回一个io.ReadCloser，相应的Close方法啥也不做，只是返回一个nil。

**ReadAll函数**

一次性读取io.Reader中的数据。

~~~go
func ReadAll(r io.Reader)([]byte,error)
~~~

**ReadDir函数**

读取目录并返回排好序的文件和子目录名([]os.FileInfo)。

**ReadFile和WriteFile函数**

ReadFile读取整个文件的内容。

~~~go
func WriteFile(filename string, data []byte, perm os.FileMode) error
~~~

将data写入filename文件中，当文件不存在时，会创建一个（文件权限有perm指定）；否则会先清空文件内容。对于perm参数，一般指定为0666。

**TempDir和TempFile函数**

~~~go
func TempDir(dir, prefix string) (name string, err error) 
~~~

第一参数如果为空，表明在系统默认的临时目录中创建临时目录；第二个参数指定临时目录名的前缀，该函数返回临时目录的路径。

~~~go
func TempFile(dir, pattern string) (f *os.File, err error)
~~~

与TempDir类似。

注意：创建者创建的临时文件和临时目录要负责删除这些临时的目录和文件。

**Discard变量**

Discard对应的类型(type devNull int)实现了io.Writer接口，同时，为了优化io.Copy到Discard，避免不必要的工作，实现了io.ReaderFrom接口。

~~~go
func (devNull)Write(p []byte)(int,error){
    return len(p),nil
}
~~~

### fmt-格式化IO

**占位符**

| 占位符 |                     说明                      |
| :----: | :-------------------------------------------: |
|   %v   |               相应值的默认格式                |
|  %+v   |           打印结构体时，添加字段名            |
|  %#v   |              相应值的Go语法表示               |
|   %T   |           响应值的类型的Go语法表示            |
|   %%   |        字面上的百分号，并非值的占位符%        |
|   %t   |                单词true或false                |
|   %b   |                  二进制表示                   |
|   %c   |         响应的Unicode码点所表示的字符         |
|   %d   |                  十进制表示                   |
|   %o   |                  八进制表示                   |
|   %q   | 单/双引号围绕的字符字面值，由Go语法安全地转移 |
| %x  %X |                 十六进制表示                  |
|   %U   |              Unicode格式：U+1234              |
|   %s   |        输出字符串表示(string或[]byte)         |
|   %    |             十六进制表示，前缀0x              |
|   +    |              总打印数值的正负号               |
|   -    |        在右侧而非左侧填充空格，左对齐         |
|   #    |              备用格式，添加前缀               |

**规则**：

- 若一个操作数实现了error接口，Error方法就能将该对象装换为字符串，随后根据占位符的需要进行格式化
- 若一个操作数实现了String()string方法，该方法能将该对象转换为字符串，随后根据占位符的需要进行格式化

为了避免以下这类递归的情况：

~~~go
type X string
func (x X)String() string{return Sprintf("<%s>",x)}
~~~

需要在递归前转换该值：

~~~go
func (x X)String() string{return Sprintf("<%s>",string(x))}
~~~

一组类似的函数通过扫描已格式化的文本来产生值。Scan、Scanf和Scanln从os.Stdin中读取；Fscan、Fscanf和Fscanln从指定的io.Reader中读取；

Sscan、Sscanf和Sscanln从实参字符串中读取。Scanln、Fscanln和Sscanln在换行符处停止扫描，且需要条目紧随换行符之后，Scanf、Fscanf和Sscanf需要输入换行符来匹配格式中的换行符，其他函数则将换行符视为空格。

在所有的扫描参数中，若一个操作数实现了Scan方法（即它实现了Scanner接口），该操作数将使用该方法扫描其文本。此外，若已扫描的实参数少于提供的实参数，就会返回一个错误。

所有需要被扫描的额实参都必须是基本类型或Scanner接口的实现。

**Stringer接口**

~~~go
type Stringer interface{
    String() string
}
~~~

**Formatter接口**

~~~go
type Formatter interface{
    Format(f State, c rune)
}
~~~

**Formatter接口由带有定制的格式化器的值所实现。Format的实现可调用Sprintf或Fprintf(f)等函数来生成其输出。**

~~~go
type Person struct{
    Name 	string
    Age		int
    Sex		int
}
func (this *Person)Format(f fmt.State, c rune){
    if c=='L'{
        f.Write([]byte(this.String()))
        f.Write([]byte(" Person has three fields."))
    }else{
        //没有这句，会导致fmt.Printf("%s",p)啥也不输出
        f.Write([]byte(fmt.Sprintln(this.String())))
    }
}
~~~

注：

1. fmt.State是一个接口。由于Format方法是被fmt包调用的，它内部会实例化好一个fmt.State接口的实例。
2. 可以实现自定义占位符，同时fmt包中和类型相对应的预定义占位符会无效。因此例子中Format的实现加入了else子句。
3. 实现了Formatter接口，相应的String接口不起作用。但实现了Formatter接口的类型应该实现Stringer接口，这样方便在Format方法中调用String方法。
4. Format方法的第二个参数是占位符中%后的字母（有精度和宽度会被忽略，只保留字母）。

**GoStringer接口**

~~~go
type GoStringer interface{
    GoString() string
}
~~~

该接口定义了类型的Go语法格式。用于打印Print格式化占位符为%#v的值。一般不需要实现该接口。

**Scan序列函数**

对于`Scan/Fscan/Sscan`这组函数将连续由空格分隔的值存储为连续的实参（换行符也记为空格）。

对于`Scnaf/Fscanf/Sscanf`这组函数将连续由空格分隔的值存储为连续的实参，其格式有`format`决定，换行符处停止扫描。

对于`Scanln/Fscanln/Sscanln`表现和上一组一样，遇到“\n”停止。

一般地，使用`Scan/Scanf/Scanln`这组函数。

**Scanner和ScanState接口**

任何实现了Scan方法的对象都实现了Scanner接口，Scan方法会从输入读取数据并将处理结果存入接收端，接收端必须是有效的指针。Scan方法接收的第一个参数为ScanState接口类型。

**bufio-缓存IO**

**bufio.Reader结构包装了一个io.Reader对象，提供了缓存功能，同时实现了io.Reader接口。**

实例化：

~~~go
func NewReader(rd io.Reader)*Reader{
    //默认缓存大小:defaultBufSize = 4096
    return NEwReaderSize(rd,defaultBufSize)
}
~~~

