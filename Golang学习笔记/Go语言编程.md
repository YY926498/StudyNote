# Go语言编程

## 入门

### hello,world

Go语言的代码通过包`package`组织，包类似于库或者模块。一个包由单个目录下的一个或多个`.go`源代码文件组成，目录定义包的作用。每个源文件都以一条`package`声明语句开始，表示该文件属于哪个包，紧跟着一系列导入`import`的包，之后是存储在这个文件里的程序语句。

`main`包比较特殊。它定义了一个独立可执行的程序，而不是一个库。在`main`里的`main`函数是整个程序执行时的入口。

`import`声明必须跟在文件的`package`声明之后。随后，则是组成程序的函数、变量、常亮、类型的声明语句（分别由关键字`func`、`var`、`const`和`type`定义）。

一个函数的声明由`func`关键字、函数名、参数列表、返回值列表以及包含在大括号里的函数体组成。

### 命令行参数

`os`包以跨平台的方式，可通过`os`包的`Args`变量获取。`os.Args`变量是一个字符串的切片。用`s[i]`访问单个元素，用`s[m:n]`获取子序列，区间索引时，Go也采用左闭右开形式，`s[m:n]`包含n-m个元素。

`os.Args`的第一个元素`os.Args[0]`是命令本身的名字，其他元素则是程序启动时传给它的参数。要获取其他所有的命令行参数可以用`os.Args[1:len(os.Args)]`或者`os.Args[1:]`。

如果程序里需要导入多个包，可以每个包写一份`import`声明，也可以用`()`括起来，比如：

~~~go
import(
    "fmt"
    "os"
)
~~~

注释语句以`//`开头。按照惯例，在每个包的包声明前添加注释，对于`main package`，注释包含一句或几句话，从整体角度对程序做个描述。

`var`声明定义变量。变量会在声明时直接初始化。如果变量没有显示初始化，则被隐式地初始化成空字符串。

`:=`是短变量声明的一部分，这是定义一个或多个变量并根据他们的初始值为这些变量赋予适当类型的语句。

`++`和`--`是语句，不像C系的其他语言是表达式。所以`j=i++`非法，而且`++`和`–-`都只能放在变量名后面，因此`–-i`非法

Go语句只有`for`循环这一种循环语句。`for`循环有多种形式，其中一种如下：

~~~go
for initialization; condition; post{
    //zero or more statements
}
~~~

`for`循环的三个部分不需括号包围。大括号强制要求，左大括号必须和`post`语句在同一行。

`initialization`语句是可选的，在循环开始前执行。`initlization`如果存在，必须是一条简单语句，即短变量声明、自增语句、赋值语句或函数调用。`condition`是一个布尔表达式，其值在每次循环迭代开始时计算。`post`语句在循环体执行结束后执行，之后再次对`condition`求值。

`for`循环的这三个部分每个都可以被省略，如果省略`initlization`和`post`，分号也可以被省略：

~~~go
//a traditional "while" loop
for condition{
    //...
}
~~~

如果连`condition`也省略，可以这样

~~~go
//a traditional infinite loop
for{
    //...
}
~~~

这样就会变成无限循环。可以采用`break`或者`return`语句。

`for`循环的另一种形式，在某种数据类型的区间上遍历

~~~go
for index, arg := range os.Args[1:]{
    fmt.Println("下标：",index,"\t",arg)
}
~~~

每次循环迭代，`range`产生一对值：索引以及该索引处的元素值。由于Go语言不允许使用无用的局部变量，这样会导致编译错误。因此如果不想要用索引，可以采用空标识符，即`__`（下划线）。空标识符可用于任何语法需要变量名但程序逻辑不需要的时候，类似于C++的`std::ignore`。

声明变量的形式有：

~~~go
s := ""
var s string
var s = ""
var s string = ""
~~~
第一种是短变量声明，最简洁，但只能用在函数内部，而不能用于包变量。第二种形式依赖于字符串的默认初始化零值机制，被初始化为“”。第三种形式用的少，除非同时声明多个变量。第四种形式显式地标明变量的类型，当变量类型与初值类型相同时，类型冗余，但如果两者类型不同，变量类型就必须了。实践中一般使用前两种形式的某个，初始值重要地话就显式地指定变量的类型，否则使用隐式初始化。

`+=`连接字符串，产生新字符串，并把它赋值给`s`。`s`原来的内容不再使用，将在适当时机对它进行垃圾回收。如果连接设计的数据量很大，这种方式代价高昂。一种简单且高效的解决方案是使用`strings`包的`join`函数：

~~~go
func main(){
    fmt.Println(strings.Join(os.Args[1:]," "))
}
~~~

另外，还可以直接输出切片：

~~~go
fmt.Println(os.Args[1:])
~~~

最后每个元素都放在一对方括号里。

### 查找重复的行

`if`语句类似`for`循环一样，两边不加括号。

~~~go
if condition{
    //...
}
~~~

`map`存储了键/值的集合，对集合元素，提供了常数时间的存，取或测试操作。键可以是任意类型，只要其键的值能用`==`运算符比较，值可以是任意类型。`map[string]int`定义的键是`string`，值是`int`。内建函数`make`具有很强的作用，例如：

~~~go
counts := make(map[string]int)
~~~

就是创建一个键是string，值是int的空map。Go中的map是哈希表，类似C++的unordered_map。同样，对map中不存在的元素使用`[]`，将对其赋其值类型的零值，对于`int`就是0。

`bufio`包使处理输入和输出方便又高效。`Scanner`类型是该包最有用的特性之一，它读取输入并将其拆成行或单词；通常是处理行形式的输入最简单的方法。

使用短变量声明创建`bufio.Scanner`类型的变量`input`。

~~~go
input := bufio.NewScanner(os.Stdin)
~~~

该变量从程序的标准输入中读取内容。每次调用`input.Scan()`,即读取下一行，并移除行末的换行符；读取的内容可以调用`input.Text()`得到。`Scan`函数在读到一行时返回`true`，不再有返回时，返回`false`。

`fmt.Printf`函数类似于C语言的`printf`，可以产生格式化输出。该函数的首个参数是个格式化字符串，指定后续参数被如何格式化。每个参数的格式取决于“转换字符”，形式为百分号后跟一个字母。Go程序员称之为动词。

|    %d    |                 十进制整数                 |
| :------: | :----------------------------------------: |
| %x,%o,%b |        十六进制，八进制，二进制整数        |
| %f,%g,%e |                   浮点数                   |
|    %t    |             布尔：true或false              |
|    %c    |            字符（unicode码点）             |
|    %s    |                   字符串                   |
|    %q    | 带双引号的字符串“abc”或者带单引号的字符‘c’ |
|    %v    |               变量的自然形式               |
|    %T    |                 变量的类型                 |
|    %%    |             字面上的百分号标志             |

默认情况下，`Printf`不会换行。按照惯例，以字母`f`结尾的格式化函数，如`log.Printf`和`fmt.Errorf`都采用`fmt.Printf`的格式化准则。而已`ln`结尾的格式化函数，则遵循`Println`的方式，以跟`%v`差不多的方式格式化参数，并在最后添加一个换行符。后缀`f`指`format`，`ln`指`line`。

`os.Open`函数返回两个值。第一个值是被打开的文件（`*os.File`），其后被`Scanner`读取。返回的第二个值是内置`error`类型的值。如果`err`等于内置值`nil`，那么文件被成功打开。读取文件，直到文件结束，然后调用`Close`关闭该文件，并释放占用的所有资源。相反，如果`err`的值不是`nil`，说明文件打开失败，这种情况下，错误值描述了所遇到的问题。

函数和包级别的变量可以任意顺序声明，并不影响其被调用。

`make`函数可以创建数据结构，比如`make(map[string]int)`创建一个`map`的引用。`map`作为参数传递给某函数时，该函数接受这个引用的一份拷贝，被调用函数对`map`底层数据结构的任何修改，调用者函数都可以通过持有的`map`的引用看到。类似于C++的引用传递。

`ReadFile`函数返回一个字节切片，必须把它切换为`string`，才能用`strings.Split`分割。

### GIF动画

常量是指在程序编译后始终都不会变化的值，一般出现在包级别的。但也可以声明定义在函数体内部，那么这种常量就只能在函数体内用。目前常量声明的值必须是一个数字值/字符串或者一个固定的boolean值。

`[]color.Color{...}`和`gif.GIF{...}`这两个表达式是复合声明。这是实例化Go语言里的符合类型的一种写法。前者生成的是一个`slice`切片，后者生成的是一个`struct`结构体。如下：

~~~go
//gif
type GIF struct {
	Image []*image.Paletted 
	Delay []int         
	LoopCount int
	Disposal []byte
	Config image.Config
	BackgroundIndex byte
}

anim := gif.GIF{LoopCount : 1}
~~~

上述是实例化一个GIF结构体，并且将其中的`LoopCount`成员初始化为1，其他成员会被设置各自类型默认的零值。

### 获取URL

主要使用`net/http`和`io/ioutil`包。其中`http.Get`函数是创建`HTTP`请求的函数，如果获取过程没有出错，那么就会在返回的结果中得到访问的请求结果。返回的Response指针的`Body`字段包括一个可读的服务器响应流。`ioutil.ReadAll`函数从`response`中读取到全部内容，将其结果保存到一个变量中。`response.Body.Close`函数关闭`response`的`Body`流，防止资源泄露。

### 并发获取多个URL

`goroutine`是一种函数的并发执行方式，而`channel`是用来在`goroutine`之间进行参数传递。`main`函数本身也运行在一个`goroutine`中，而`go function`则表示创建一个新的`goroutine`，并在这个新的`goroutine`中执行这个函数。

使用`make`函数创建一个传递`string`类型参数的`channel`：

~~~go
ch := make(chan string)
~~~

`ioutil.Discard`输出流相当于一个垃圾桶。使用`io.Copy`可将一个字节数组拷贝到一个输出流，并返回长度。

当一个`goroutine`尝试在一个`channel`上做`send`或者`receive`操作时，这个`goroutine`会阻塞在调用处，直到另一个`goroutine`从这个`channel`里接收或者写入值，这样两个`goroutine`才会继续执行`channel`操作之后的逻辑。

~~~go
ch := make(chan string)
ch <- string("nihao")//将数据写到channel
fmt.Println(<-ch)//从channel中读取数据
~~~

### Web服务

主要使用

~~~go
http.HandleFunc("/",func(w http.ResponseWriter, r *http.Request){
	//...
})
~~~

### 本章总结

`switch`：

~~~go
switch coinflip () {
    case "heads":
    	heads++
    case "tails":
    	tails++
    default:
    fmt.Println("landed on edge!")
}
~~~

Go语言不需要显式地在每一个`case`后写`break`，语言默认执行完`case`后的逻辑语句会自动退出。如果想相邻的几个`case`都执行同一逻辑，需要显式地写上一个`fallthrough`语句来覆盖这种默认行为。

另外Go语言的`switch`可以不带对象（`switch`不带操作对象时，默认用`true`值代替，然后将每个`case`的表达式和true值进行比较；可以直接罗列多种条件，像其他语言的`if else`一样，如下：

~~~go
func Signum(x int)int {
    switch {
        case x>0:
        	return +1
        default:
        	return 0
        case x<0:
        	return -1
    }
}
~~~

这种叫`无tag switch`；这和`switch true`等价。

类似于`for`和`if`控制语句一样，`switch`也可以紧跟一个简短的变量声明，一个自增表达式/赋值语句，或者一个函数调用。

**命名类型**：类型声明可以很方便地给一个特殊类型一个名字。因为`struct`类型声明非常长，可以给`struct`取一个名字。例如：

~~~go
type Point struct {
    X, Y int
}

var p Point
~~~

**指针**：Go语言提供了指针。指针时一种直接存储了变量的内存地址的数据类型。Go语言的指针没有C语言这么强。指针是可见的内存地址，`&`操作符可以返回一个变量的内存地址，并且`*`操作符可以获取指针指向的变量内容，但是在Go语言里没有指针运算，也就是不能像C语言里可以对指针进行加或减操作。

**方法和接口**：方法是和命名类型关联的一类函数。Go语言里比较特殊的是方法可以被关联到任意一种命名类型。接口是一种抽象类型，这种类型可以让我们以同样的方式来处理不同的固有类型，不用关心它们的具体实现，而只需要关注它们提供的方法。

**包(package)**：Go语言提供了很多包，并且这些包是可以扩展的。借助`godoc`工具可以直接在本地命令行阅读标准库的文档：

~~~go
go doc http.ListenAndServe
---------------------------------------------------------------------
package http // import "net/http"

func ListenAndServe(addr string, handler Handler) error
    ListenAndServe listens on the TCP network address addr and then calls Serve
    with handler to handle requests on incoming connections. Accepted
    connections are configured to enable TCP keep-alives.

    The handler is typically nil, in which case the DefaultServeMux is used.

    ListenAndServe always returns a non-nil error.
~~~

**注释**：在源文件开头写注释和在每个函数前写一个说明函数行为的注释是一个好习惯，可以被`godoc`这样的工具检测到。并且在执行命令时显示这些注释。

多行注释可以用`/*...*/`来包裹。

## 程序结构

### 命名

Go语言中的函数名，变量名，常量名，类型名，语句标号和包名等所有的命名，都遵循一个简单的命名规则：一个名字必须以一个字母（Unicode字母）或下划线开头，后面可以跟任意数量的字母，数字或下划线。大写字母和小写字母是不同的。

**关键字**：

~~~go
break		default			func	interface	select
case		defer			go		map			struct
chan		else			goto	package		switch
const		fallthrough		if		range		type
continue	for				import	return		var
~~~

预定义名字：

~~~go
内建常量：true	false	iota	nil

内建类型：int	int8	int16	int32	int64
		uint	uint8	uint16	uint32	uint64	uintptr
		float32	float64	complex128	complex64
		bool	byte	string	error

内建函数：make	len	cap	new	append	copy	close	delete
		complex	real	imag
		panic	recover
~~~

这些内部预定义的名字不是关键字，可以重新定义，但是要避免过度定义引起混乱。

如果一个名字是在函数内部定义，那么它就在函数内部有效。如果是在函数外部定义，那么将在当前包的所有文件中都可以访问。名字的开头字母的大小写决定了名字在包外的可见性。如果一个名字是大写字母开头的（必须是在函数外部定义的包级别的名字，包级函数名本身也是包级名字），那么它将是导出的，也就是说可以被外部的包访问，例如`fmt`包的`Printf`函数就是导出的，可以在`fmt`包外部访问。包本身的名字一般是用小写字母。

名字的长度没有逻辑限制，但是Go语言的风格是尽量使用短小的名字，对于局部变量尤其是这样。通常来说，如果一个名字的作用域比较大，声明周期比较长，那么用长的名字就会更有意义。

Go语言推荐使用驼峰式命名，当名字由几个单词组成，优先使用大小写分割，而不是优先使用下划线分割。但是对于缩略词，采用全部大写的形式，例如：`ASCII`或者`HTML`。

### 声明

声明语句定义了程序的各种实体对象以及部分或全部的属性。Go语言主要有四种类型的声明语句：`var`，`const`，`type`和`func`。分别对应变量，常量，类型和函数实体对象的声明。

在包一级声明语句声明的名字可在整个包对应的每个源文件中访问，而不是仅仅在其声明语句所在的源文件中访问。相比之下，局部声明的名字就只能在函数内部很小的范围被访问。

一个函数的声明由一个函数名字，参数列表（由函数的调用者提供参数变量的具体值），一个可选的返回值列表和包含函数体定义的函数体组成。如果函数没有返回值，那么返回值列表是省略的。执行函数从函数的第一个语句开始，一次顺序执行直到遇到return返回语句，如果没有返回语句则是执行到函数末尾，然后返回到函数调用者。

### 变量

`var`声明语句可以创建一个特定类型的变量，然后给变量附加一个名字，并且设置变量的初始值。变量声明的一般语法如下：

~~~go
var 变量名字  类型 = 表达式
~~~

其中`类型`或`= 表达式`两个部分可以省略其中的一个。如果省略的是类型信息，那么将根据初始化表达式来推导变量的类型信息。如果初始化表达式被省略，那么将用零值初始化该变量。数值类型变量对应的零值是0，布尔类型的变量对应的零值是false，字符串类型对应的零值是空字符串，接口或引用类型（包括`slice`，指针，`map`，`chan`和函数）变量对应的零值是`nil`。数组或结构体等聚合类型对应的零值是每个元素或字段都是对应该类型的零值。

 零值初始化机制可以确保每个声明的变量总是有一个良好定义的值，因此在Go语言中不存在未初始化的变量。这个特性可以简化很多代码，而且可以在没有增加额外工作的前提下确保边界条件下的合理行为。 