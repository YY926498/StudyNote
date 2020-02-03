# Go语言核心编程

## Ch6 反射

反射是指计算机程序在运行时可以访问、检测和修改本身状态或行为的一种能力。

### 基本概念

首先将实例传递给内部的空接口，实际上是将一个实例类型装换为接口可以表述的数据接口，反射基于这个转换后的数据结构来访问和操作实例的值和类型。

#### 基本数据结构和入口函数

`reflect.Type`

反射包里面有一个通用的描述类型公共信息的结构`rtype`，这个`rtype`类似接口内部实现时的runtime包里面的`_type`是同一个东西，只是因为包的隔离性分开定义而已。是描述类型的通用信息，同时为每一种基础类型封装了一个特定的结构。

~~~go
type rtype struct {
	size       uintptr
	ptrdata    uintptr  // number of bytes in the type that can contain pointers
	hash       uint32   // hash of type; avoids computation in hash tables
	tflag      tflag    // extra type information flags
	align      uint8    // alignment of variable with this type
	fieldAlign uint8    // alignment of struct field with this type
	kind       uint8    // enumeration for C
	alg        *typeAlg // algorithm table
	gcdata     *byte    // garbage collection data
	str        nameOff  // string form
	ptrToThis  typeOff  // type for pointer to this type, may be zero
}

// arrayType represents a fixed array type.
type arrayType struct {
	rtype
	elem  *rtype // array element type
	slice *rtype // slice type
	len   uintptr
}

// chanType represents a channel type.
type chanType struct {
	rtype
	elem *rtype  // channel element type
	dir  uintptr // channel direction (ChanDir)
}
...
~~~

`rtype`实现了接口`reflect.Type`。

为什么反射接口返回的是一个Type接口类型，而不是直接返回`rtype`？

1.  因为类型信息是一个只读的信息，不可能动态地修改类型的相关信息，不太安全。
2.  因为不同的类型，类型定义也不一样，使用接口这一抽象数据结构能够进行统一的抽象。`reflect.TypeOf()`函数返回一个Type的接口变量，通过接口抽象出来的方法访问具体的类型信息。

`Type`接口的主要方法：

1.  所有类型通用的方法

~~~go
Name() 	string//返回包含包名的类型的名字，对于未命名类型返回的是空
Kind()	Kind//返回该类型的底层基础类型

Implements(u Type) bool//确定当前类型是否实现了u接口类型

AssignableTo(u Type) bool //判断当前类型的实例是否能赋值给type为u的类型变量

ConvertibleTo(u Type) bool//判断当前类型是否能够转换为u类型变量

Comparable() bool //判断当前类型是否支持比较

NumMethod() int//返回一个类型的方法的个数

Method(int) Method //通过索引值访问方法，索引值必须属于[0,NumMethod()),否则引发panic

MethodByName(string)(Method,bool)//通过方法名获取Method

PkgPath() string //返回类型的包路径，如果类型是预声明或未命名类型，则返回空字符串

Size() uintptr //返回存放该类型的实例需要多大的字节空间
~~~

2.  不同基础类型的专有方法

如果不是某种特定的类型却调用了该类型的方法，则会引发panic。如果不确定类型，则要先调用Kind()方法确定类型后再调用类型的专有方法。

~~~go
//Int*, Uint*, Float*, Complex*:Bits
//Array:Elem, Len
//Chan:ChanDir, Elem
//Func:In, NumIn, Out, NumOut,IsVariadic
//Map:Key, Elem
//Ptr:Elem
//Slice:Elem
//Struct:Field,FieldByIndex,FieldByName, FieldByFunc,NumField

Elem() Type//返回类型的元素类型

Bits()int//返回数值类型内存占用的位数

//Struct专用的方法
NumField()int//返回字段数目

Field(i int)StructField//通过整数索引获取struct字段

FieldByIndex(index []int)StructField//获取嵌入字段的struct字段

FieldByName(name string)StructField//通过名字查找获取struct字段

//func专有方法
IsVariadic()bool//函数是否为不定参数
NumIn()int	//输入参数个数
NumOut() int	//输出参数个数
In(i int)Type	//返回第i个输入参数的类型
Out(i int)Type	//返回第i个输出参数的类型

//map类型专有的方法
Key()Type
~~~



对于`reflect.TypeOf(a)`，传进去的实参a有两种类型，一种是接口变量，另一种是具体类型变量。如果a是具体类型变量，则`reflect.TypeOf()`返回的是具体的类型信息；如果a是一个接口变量，则函数的返回结果又分为两种：如果a绑定了具体实例类型的信息，则返回的是接口的动态类型。如果a没有绑定具体类型实例，则返回的是接口自身的静态类型信息。

注：对于接口的反射，目前只能针对接口类型为`Array`、`Chan`、`Map`、`Ptr`、`Slice`等进行反射操作。

 `reflect.Value`

~~~go
type Value struct {
    typ	 	*rtype
    ptr		unsafe.Pointer
    flag
}
~~~

`reflect.Value`总共有三个字段，一个是值的类型指针`typ`，另一个是指向值的指针`ptr`，最后一个是标记字段`flag`。

#### 基础类型

`Type`接口有一个`Kind()`方法，返回的是一个整数枚举值，不同的值代表不同的类型。这种类型是一个抽象的概念，暂且称为“基础类型”，比如所有的结构都归结为一种基础类型`struct`，所有的函数都归结为一种基础类型`func`。

**底层类型和基础类型**

区别：基础类型是抽象的类型划分，底层类型是针对每一个具体的类型来定义的。

### 反射规则

#### 反射API

1.  从实例到Value

~~~go
func ValueOf(i interface{}) Value
~~~

2.  从实例到Type

~~~go
func TypeOf(i interface{}) Type
~~~

3.  从Type到Value：由于Type里面只有类型信息，所以直接从一个Type接口变量里面是无法获得实例的Value的，但可以通过该Type构建一个新实例的Value。

~~~go
func New(typ Type) Value//New返回的是一个Value，该Value的type为PtrTo(typ)，即Value的Type是指定typ的指针类型

func Zero(typ Type) Value//Zero返回的是一个typ类型的零值，注意返回的Value不能寻址，值不可变
~~~

如果知道一个类型的值的底层存放地址，则还有一个函数是可以根据type和该地址值恢复出Value的：

~~~go
func NewAt(typ Type,p unsafe.Pointer) Value
~~~

4.  从Value到Type：从反射对象Value到Type可以直接调用Value的方法，因为Value内部存放了Type类型的指针：

~~~go
func (v Value) Type() Type
~~~

5.  从Value到实例：Value本身就包含类型和值信息，reflect提供了丰富的方法来实现从Value到实例的转换：

~~~go
func (v Value)Interface()(i interface)//将Value转换为空接口，该空接口内部存放具体类型实例，可以使用接口类型查询还原为具体的类型

//Value自身也提供丰富的方法，直接将Value转换为简单类型实例，如果类型不匹配，则直接引起panic
func (v Value) Bool() bool
func (v Value) Float() float64
func (v Value) Int() int64
func (v Value) Uint() uint64
~~~

6.  从Value到指针到值：从一个指针类型的Value获得值类型Value有两种方法：

~~~go
//如果v类型是接口，则Elem()返回接口绑定的实例的Value，如果v类型是指针，则返回指针值的Value，否则引发panic
func (v Value) Elem()Value

//如果v是指针，则返回指针值的Value，否则返回v自身，该函数不会引起panic
func Indirect(v Value) Value
~~~

7.  Type指针和值的相互转换

-   指针类型Type到值类型Type

~~~go
//t必须是Array、Chan、Map、Ptr、Slice，否则会引发panic
//Elem返回的是其内部元素的type
t.Elem() Type
~~~

-   值类型Type到指针类型Type

~~~go
//PtrTo返回的是指向t的指针型Type
func PtrTo(t Type)Type
~~~

8.  Value值的可修改性

~~~go
func (v Value) CanSet() bool //通过CanSet判断是否可修改

func (v Value) Set(v Value) //通过Set进行修改
~~~

由于都是值传递，因此必须传递指针、切片等底层含有指针的类型。

#### 反射三定律

-   反射可以从接口值得到反射对象。
-   反射可以从反射对象获得接口值。
-   若要修改一个反射对象，则其值必须可以修改。

### `inject`库

正常情况下，对函数或方法的调用是调用方的主动直接行为，调用方清楚地知道被调的函数名是什么，参数有哪些类型，直接主动地调用；包括对象的初始化也是显式地直接初始化。

**控制反转**：将这种主动行为变成间接行为，主调方不是直接调用函数或对象，而是借助框架代码进行间接的调用和初始化。控制反转可以解耦调用方和被调方。

一般情况下，使用库的程序是程序主动地调用库的功能，但使用框架的程序常常由框架驱动整个程序，在框架下写的业务代码是被框架驱动的，这种模式就是“控制反转“。

“依赖注入”是实现“控制反转”的一种方法，如果说“控制反转”是一种设计思想，那么“依赖注入”就是这种思想的一种实现，通过注入的参数或实例的方法实现控制反转。一般情况下，等价。

#### `inject`实践

`inject`实现了对结构体和函数的依赖注入。

#### `inject`原理分析

**入口函数`New`**：`inject.New()`函数构建了一个具体类型`injector`实例作为内部注入引擎，返回的是一个`Injector`类型的接口。

`Injector`的方法归类为两类：

-   对参数注入进行初始化，将结构类型的字段的注入和函数的参数注入统一成一套方法实现
-   专用注入实现，分别是生成结构对象和调用函数方法

~~~go
type Injector interface {
    //抽象生成注入结构实例的接口
    Applicator
    
    //抽象函数调用的接口
    Invoker
    
    //抽象注入参数的接口
    TyepMapper
    
    //实现一个注入实例链，下游的能覆盖上游的类型
    SetParent(Injector)
}
~~~

`TypeMapper`接口实现对注入参数操作的汇总，包括设置和查找相关的类型和值的方法。注意：无论函数的实参，还是结构的字段，在`inject`内部，都存放在`map[reflect.Type]reflect.Value`类型的`map`里面。

~~~go
type TypeMapper interface {
    //如下三个方法是设置参数
    Map(interface{}) TypeMapper
    MapTo(interface{}, interface{}) TypeMapper
    Set(reflect.Type, reflect.Value) TypeMapper
    
    //查找参数
    Get(reflect.Type) reflect.Value
}
~~~

`Invoker`接口中`Invoke`方法是对被注入实参函数的调用：

~~~go
type Invoker interface{
    Invoke(interface{})([]reflect.Value,error)
}
~~~

`Applicator`接口中`Apply`方法实现对结构的注入：

~~~go
type Applicator interface {
    Apply(interface{}) error
}
~~~

### 反射的优缺点

**优点**：

-   通用
-   灵活

**缺点**：

-   脆弱
-   晦涩
-   性能损失

**最佳实践**：

-   在库或框架内部使用反射，而不是把反射接口暴露给调用者，复杂性留在内部，简单性放到接口
-   框架代码才考虑使用反射，一般的业务代码没有必要抽象到反射的层次，这种过度设计会带来复杂度的提升，使得代码难以维护
-   除非没有其他方法，否则不要使用反射

## Ch7 语言陷阱

### 多值赋值和短变量声明

#### 多值赋值

可以一次性声明多个变量，并可以在声明时赋值，而且可以省略类型，但必须遵守一定的规则要求。

-   相同类型的变量可以在末尾带上类型

~~~go
var x, y int
var x, y int = 1, 2
~~~

-   如果不带类型，则可以直接进行类型推断

~~~go
var x, y = 1, 2
~~~

-   不同类型的变量声明和隐式初始化可以使用如下语法

~~~go
var (
    x int
    y string
)
~~~

-   多值赋值语句中每个变量后面不能都带上类型

**多值赋值的两种格式**

-   右边是一个返回多值的表达式，可以是返回多值的函数调用，也可以是`range`对`map`或者`slice`等函数的操作，还可以是类型断言：

~~~go
x, y = f()

for k, v:= range map{
    
}
v, ok := i.(xxx)
~~~

-   赋值的左边操作数和右边的单一返回值的表达式个数一样，逐个从左向右依次对左边的操作数赋值。例如：

~~~go
x, y, z = a, b, c
~~~

**多值赋值语义**

-   对左侧操作数中的表达式、索引值进行计算和确定，首先确定左侧的操作数的地址；然后对右侧的赋值表达式进行计算，如果发现右侧的表达式计算引用了左侧的变量，则创建临时变量进行值拷贝，最后完成计算
-   从左到右的顺序依次赋值

#### 短变量的声明和赋值

短变量声明和赋值是指在Go函数或类型方法内部使用`:=`声明并初始化变量，支持多值赋值，格式如下：

~~~go
a := va
a, b := va, vb
~~~

短变量的声明和赋值的语法要约：

-   使用`:=`操作符，变量的定义和初始化同时完成
-   变量名后不要跟任何类型名，Go编译器完全靠右边的值进行推导
-   支持多值短变量声明赋值
-   只能用在函数或类型方法的内部

在多值短变量的声明和赋值时，至少有一个变量是新创建的局部变量，其他的变量可以复用以前的变量，不是新创建的变量执行的仅仅是赋值。

赋值操作符`=`和`:=`的区别：

-   `=`不会声明并创建变量，而是在当前赋值语句所在的作用域由内向外逐层去搜寻变量，如果没有搜索到相同变量名，则报编译错误
-   `:=`必须出现在函数或类型方法内部
-   `:=`至少要创建一个局部变量并初始化

### `range`复用临时变量

-   for range下的迭代变量是共用的
-   main函数所在的goroutine和后续启动的goroutine存在竞争关系

因此，在for循环下调用并发时要复制迭代变量后再使用，不要直接引用for迭代变量

### `defer`陷阱

**`defer`和函数返回值**

defer中如果引用了函数的返回值，则因引用形式不同会导致不同的结果。

-   函数调用方负责开辟栈空间，包括形参和返回值的空间
-   有名的函数返回值相当于函数的局部变量

~~~go
package main

func f1()(r int){
    defer func(){
        r++
    }()
    return 0
}
func f2()(r int){
    t := 5
    defer func(){
        t=t+5
    }()
    return t
}
func f3()(r int){
    defer func(r int){
        r=r+5
    }(r)
    return 1
}
func main(){
    fmt.Println("f1=",f1())//f1=1
    fmt.Println("f2=",f2())//f2=5
    fmt.Println("f3=",f3())//f3=1
}
~~~

分析f1：

-   r是函数的有名返回值，分配在栈上，其地址又被称为返回值所在栈区。首先r被初始化为0
-   `return 0`会复制0到返回值栈区，返回值r被赋值为0
-   执行`defer`语句，由于匿名函数对返回值r是闭包引用，所以`r++`执行后，函数返回值被修改为1
-   `defer`语句执行完`RET`返回，此时函数的返回值仍然为1

分析f2:

-   返回值r被初始化为0
-   引入局部变量t，并被初始化为5
-   赋值t的值5到返回值r所在的栈区
-   `defer`语句后面的匿名函数是对局部变量t的闭包引用，t的值被设置为10

分析f3:

-   返回r被初始化为0
-   复制1到函数返回值r所在的栈区
-   执行`defer`，`defer`后匿名函数使用的是传参数调用，在注册defer函数时将函数返回值r作为实参传进去，由于函数调用的是值拷贝，所以`defer`函数执行后只是形参值变为5，对实参没有任何影响
-   函数返回，此时函数返回值栈区上的值是1

综上，对于带defer的函数返回值整体上有三个步骤

-   执行return的值拷贝，将return语句返回的值复制到函数返回值栈区（如果只有一个return，不带任何变量或值，则此步骤不做任何动作
-   执行defer语句，多个defer按照FILO顺序执行
-   执行调整RET指令

另外一种解决方法是：在定义函数时使用不带返回值名的格式，此时，defer就不能直接引用返回值的栈区，也就避免了返回值被修改的问题。

### 切片困惑

#### 数组

数组的类型不仅包括其元素类型，也包括其大小，`[2]int`和`[5]int`是两个完全不同的数组类型。

创建数组：

-   声明时通过字面量进行初始化

-   直接声明，不显示地进行初始化

    ~~~go
    a := [3]int {1, 2, 3}
    b := [...]int{1,2,10:10}
    var c [3]int
    ~~~

数组名无论作为函数实参，还是作为struct嵌入字段，或者数组之间的直接赋值，都是值拷贝。

#### 切片

切片创建：

-   通过数组创建：array[b:e]创建一个包括e-b个元素的切片，第一个元素是array[b]，最后一个元素是array[e-1]
-   make：通过内置的make函数创建，make([]T, len, cap)中的T是切片元素类型，len是长度，cap是底层数组的容量，其中cap是可选参数
-   直接声明：可以直接声明一个切片，也可以在声明切片的过程中使用字面量进行初始化，直接声明但不进行初始化的切片其值为nil。例如：

~~~go
var a	[]int	//a is nil
var a	[]int = []int{1,2,3,4}
~~~

另外，`make([]T,0)`创建的是空切片，不等于nil。其中底层指针指向的0。

**多个切片引用同一个底层数组引发的混乱**：切片可以有数组创建，一个底层数组可以创建多个切片，这些切片共享底层数组，使用append扩展切片过程中可能修改底层数组中的元素，间接地影响其他切片的值，也可能发生数组复制重建，共用底层数组的切片，由于其行为不明确，不推荐使用。

#### 函数名的意义

函数名和匿名函数字面量的值的含义：

-   类型信息，表明其数据类型是函数类型
-   函数名代表函数的执行代码的起始位置
-   可以通过函数名进行函数调用。函数调用格式为`func_name(param_list)`。在底层执行层面包含以下4部分内容：
    -   准备好参数
    -   修改PC值，跳转到函数代码起始位置开始执行
    -   复制值到函数的返回值栈区
    -   通过RET返回到函数调用的下一条指令处继续执行

#### 引用语义

这里的引用不是C++的对变量的引用，而是闭包对外部变量的引用。可以认为是建立了一个和外部变量同名的“引用”，该引用和外部变量指向相同的地址。

### 习惯用法

#### 干净和强迫症

-   编译器不能通过未使用的局部变量（包括未使用的标签）
-   “import”未使用的包同样不能通过编译，但可以匿名导入
-   所有的控制结构、函数和方法定义的“{”放到行尾，而不能另起一行
-   提供go fmt工具格式化代码，使所有的代码风格保持统一。

#### comma, ok表达式

-   获取map值

    获取map中不存在的值不会发生异常，而是会返回值类型的零值，如果想确定map中是否存在key值，则可以使用map值的comma,ok语法，示例如下：

    ~~~go
    m := make([string]string)
    
    v, ok := m["some"]
    
    if !ok {
        
    } else {
        
    }
    ~~~

-   读取chan的值

读取已经关闭的通道，不会阻塞，也不会引起panic。而是一直返回该通道的零值。判断关闭的方法有两种：

1.  读取通道的comma, ok表达式，如果通道已经关闭，则ok的返回值是false
2.  通过range循环迭代

~~~go
c := make(chan int)
go func(){
    c <- 1
    c <- 2
    close(c)
}()
for {
    //使用comma, ok判断通道是否关闭
    v, ok := <- c
    if ok {
        
    }
}
//使用range更加简洁
for v:=range c {
    
}
~~~

-   类型断言

~~~go
//判断接口body绑定的实例是否实现了另一个接口类型io.ReadCloser
rc, ok := body.(io.ReadCloser)
~~~

#### 简写模式

Go语言里很多重复引用或声明可以用`()`进行简写。

1.  import多个包

~~~go
import (
	"io"
    "fmt"
)
~~~

2.  多个变量声明

包中多个相关全局变量声明时，建议使用`()`进行合并声明：

~~~go
var (
	a int = 5
    b string = "abc"
)
~~~

#### 包中的函数或方法设计

在包内部实现两个“同名“的函数或方法：一个首字母大写，用于导出API供外部调用，一个首字母小写，用于实现具体逻辑。一般首字母大写的函数调用首字母小写的函数，同时包装一些功能；首字母小写的函数负责更多的底层细节。

#### 多值返回函数

多值返回函数里如果有error或bool类型的返回值，则应该讲error或bool作为最后一个返回值。





