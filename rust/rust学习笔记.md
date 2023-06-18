# Rust学习笔记

## Rust基础介绍

### cargo

新建rust项目：

```rust
cargo new hollo_world
```

rust项目运行指令：

```rust
cargo run [--release]
```

rust编译指令：

```rust
cargo build [--release]
```

由于项目变大之后，执行cargo build或者cargo run会变得很慢，可以使用`cargo check`指令快速检查代码能否编译通过，节省大量的编译时间。

### Cargo.toml和Cargo.lock

`Cargo.toml`和`Cargo.lock`是cargo的核心文件，所有活动都基于这两个。

`Cargo.toml`是`cargo`的项目数据描述文件。存储项目的所有元配置信息。

`Cargo.lock`是`cargo`工具根据同一项目的toml文件生成的项目依赖详细清单。如果项目是可运行程序时，就上传`Cargo.lock`，如果只是依赖库项目，就需要添加到`.gitignore`中。

### 定义项目依赖

使用`cargo`时，主要通过`Cargo.toml`的依赖段落来描述项目的各种依赖项：

-   基于Rust官方仓库`creates.io`，通过版本说明来描述
-   基于项目源代码的git仓库地址，通过URL来描述
-   基于本地项目的绝对路径或者相对路径，通过类Unix模式的路径来描述

这三种形式的具体写法如下：

```rust
[dependencies]
rand = "0.3"
hammer = {version = "0.5.0"}
color = {git = "https://github.com/bjz/color-rs"}
geometry = {path = "creates/geometry"}
```

### 函数

定义一个函数，打印多种语言的`hello, world`

```rust
fn greet_world() {
    let chinese = "世界，你好";
    let english = "hello, world";

    let regions = [chinese,english];
    for region in regions.iter(){
        println!("{}",&region);
    }
}
```

其中，`println`后面的`!`运算符是**宏**操作符，另外`println`使用`{}`来作为占位符，Rust底层会自动识别输出数据的类型。

最后，Rust的集合类型不能直接进行循环，需要编程迭代器才能用于迭代循环

## Rust基本概念

### 变量绑定与解构

#### 变量命名

##### 变量绑定

rust中，将一个值赋值给一个变量，称为**变量绑定**，例如：

```rust
let a = "hello world"
```

这里称为变量绑定涉及到rust最核心的原则-所有权。即所有内存对象都是有主任的，而且一般情况下完全属于它的主人。绑定就是把这个对象绑定给一个变量，让这个变量成为它的主人。

##### 变量可变性

rust的变量默认情况下是不可变的。可以通过`mut`关键字让变量变为可变的。如果变量a不可变，一旦为它绑定值，就不能再修改a。例如:

```rust
fn main() {
    let x = 5;
    println!("{}", x);
    x = 6;
}
```

执行`cargo run`命令后，就会提示`x=6`无法对不可变变量赋值。如果要定义一个可变变量，也很简单，在变量名前加一个`mut`。如下：

```rust
fn main() {
    let mut  x = 5;
    println!("{}", x);
    x = 6;
    println!("{}", x);
}
```

### 使用下划线开头忽略未使用的变量

如果创建一个变量却不会在任何地方使用，Rust会给出一个警告，但是有时刚开始编写项目的时候，需要Rust不要警告未使用的变量，此时可以用下划线作为变量名的开头。例如:

```rust

fn unused_variablefn(){
    let _x = 5;
}
```

### 变量解构

let表达式不仅仅用于变量的绑定，还能进行复杂变量的解构；从一个相对复杂的变量中，匹配出该变量的一部分内容:

```rust
fn unbind(){
    let(a ,mut b) = (true,false);
    println!("a = {:?},  b = {:?}",a,b);

    b=true;
    println!("b = {:?}",b)
}
```

#### 解构式赋值

在rust新版本中，可以在赋值语句的左式中使用元组、切片和结构体模式：

```rust
struct SingleVar {
    c:i32
}

fn express_bind(){
    let (a,b,c);

    [a,..,b,_] = [1,2,3,4,5,6];
    println!("a = {:?}, b = {:?}",a,b );
    SingleVar{c} = SingleVar{c:12};  
    println!("c = {:?}",c);
}
```

另外在测试中发现，结构体赋值时，赋值的变量名必须与结构体里的field名字保持一致。如果不一致，则会报错：

```rust
struct SingleVar {
    c:i32
}

// 比如变量名为d，但是结构体里的field名字是c，则会提示结构体里没有对应的field
fn express_bind(){
    let d;
    SingleVar{d} = SingleVar{c:12};
   |               ^
   |               |
   |               struct `SingleVar` does not have this field
   |               help: a field with a similar name exists: `c`
}
```

另外，如果结构体里有多个变量，也可以只对某个变量赋值：

```rust
struct SingleVar {
    c:i32,
    d:i32
}

fn express_bind(){
    let (c,d);

    SingleVar{c,..} = SingleVar{c:12,d:1};
    println!("c = {:?}",c);

    SingleVar{d,..} = SingleVar{c:11,d:21};
    println!("d = {:?}",d);
}

// output：
c = 12
d = 21
```

目前：使用`+=`的赋值语句还不支持解构式赋值。

### 变量与常量之间的差异

变量与常量之间存在一些差异：常量不允许使用`mut`。**常量不仅仅默认不可变，而且自始至终不可变**。常量使用`const`关键字而不是`let`关键字来声明，并且值的类型必须标注。

Rust的常量命名约定是全部字母都使用大写，并使用下划线分隔单词，另外对数字字面量可插入下划线以提高可读性。

```rust
const MAX_POINTS : u32 = 100_000;
```

### 变量遮蔽

Rust允许声明相同的变量名，在后面声明的变量会遮蔽前面声明的：

```rust
fn variable_shadowing(){
    let a = 123;
    println!("a = {:?}",a);

{
    let a = a+1;
    println!("a = {:?}",a);
}

    let a = a+1;
    println!("a = {:?}",a);
}

// output 
a = 123
a = 124
a = 124
```

这里可以看出，rust的花括号跟其他语言保持一致，里面重新定义变量a，不会影响到花括号外的变量a。这里后续的let生成了完全不同的新变量，多个变量只是名称相同，但是会设计内存对象的再分配，而`mut`声明的变量，后续修改时，只是修改同一个内存地址上的值，不会发生内存对象的再分配，性能更好。

变量遮蔽的用处在于：如果在某个作用域内无须再使用之前的变量，就可以重复的使用变量名字，不用去想更多的名字。并且变量遮蔽时，允许前后同名变量的类型不相同。

## 基本类型

#### 类型推导与标注

Rust可以根据变量的值和上下文中的使用方式来自动推导出变量的类型。但是在某些情况下，需要手动标注类型。

```rust
fn type_assert() {
    let guess = "42".parse().expect("not a numble!");
    println!("guess = {:?}", guess);
}

// output
help: consider giving `guess` an explicit type
  |
6 |     let guess: /* Type */ = "42".parse().expect("not a numble!");
```

解决方法是：

```rust
fn type_assert() {
    let guess = "42".parse::<i32>();
    println!("guess = {:?}", guess);
}
// output
guess = Ok(42)


fn type_assert1() {
    let guess:i32 = "42".parse().expect("not a number");
    println!("guess = {:?}", guess);
}
// output
guess = 42
```

### 数值类型

#### 整数类型

**整型溢出**：debug模式编译时，Rust会检查整型溢出。如果检查到整型溢出是，会在编译时panic。如果使用`--release`模式编译时，Rust不会检查溢出，此时按照补码循环溢出。

如果要显式处理可能的溢出，可以使用标准库针对原始数字类型提供的方法：

`wrapping_*`方法在所有模式下都按照补码循环溢出的规则处理，例如：`wrapping_add`

`checked_*`方法在发生溢出时，返回`None`值

`overflowing_*`方法返回该值和一个指示是否存在溢出的布尔值

`saturating_*`方法使值达到最小值或最大值

```rust
fn int_overflow(){
    let a : u8 = 250;

    let b = a.wrapping_add(10);
    println!("wrapping_add b = {:?}",b);

    let b = a.checked_add(10);
    println!("checked_add b = {:?}",b);

    let b = a.overflowing_add(10);
    println!("overflowing_add b = {:?}",b);

    let b = a.saturating_add(10);
    println!("saturating_add b = {:?}",b);
}

// output
wrapping_add b = 4
checked_add b = None
overflowing_add b = (4, true)
saturating_add b = 255
```

#### 浮点类型

**浮点数陷阱**

1.  浮点数往往是想要数字的近似表示
2.  浮点数在某些特性上是反直觉的：比如：浮点数的比较运算实现的是`std::cmp::PartialEq`特征，没有实现`std::cmp::Eq`特征。

Rust的`HashMap`数据结构要求`K`的类型必须实现了`std::cmp::Eq`特征，这表示浮点数无法作为`HashMap`的`key`来存储键值对，但是Rust的整数类型、字符串类型、布尔类型都实现了该特征，因此可以作为`HashMap`的`Key`。

为了避免上面的两个陷阱，需要遵守以下准则：

-   避免在浮点数上测试相等性
-   当结果在数学上可能存在未定义时，需要格外小心

**NaN**：Rust的浮点数类型使用`NaN`来处理数学上未定义的结果。所有跟`NaN`交互的操作，都会返回一个`NaN`，而且`NaN`不能用来比较。

#### 序列

Rust提供简单的方式，用来生成连续的数值，例如`1..5`生成1到4的连续数字；`1..=5`生成从1到5的连续数字，常常用于循环中:

```rust
for i in 1..=5 {
    println!("{}",i)
}
```

序列只允许用于数字或字符类型，原因是：可以连续，同时编译器在编译期可以检查该序列是否为空。字符和数字值是Rust中仅有的可以用于判断是否为空的类型。

### 字符、布尔、单元类型

#### 字符类型（char）

Rust的字符是Unicode值。

#### 布尔（bool）

Rust中的布尔类型有两个可能的值：true或者false。布尔值占用内存的大小为1个字节。

#### 单元类型

单元类型就是`()`。例如：`fn main()`函数的返回值就是单元类型`()`。`main`函数不是无返回值。在Rust中无返回值的函数是`发散函数`，即无法收敛的函数。单元类型不占用任何内存。

### 语句和表达式

语句会执行一些操作但是不会返回一个值，表达式在求值后返回一个值。

#### 语句

````rust
let a = 8;
let b :Vec<f64> = Vec::new();
let (a,c) = ("hi",false);
````

以上都是语句，完成一个具体的操作，但是没有返回值。

由于let是语句，不能将let语句赋值给其他值，如下形式是错误的：

```rust
let b = (let a=8);
```

#### 表达式

表达式会进行求值，然后返回一个值。表达式可以成为语句的一部分，例如：`let y = 6`中，`6`就是一个表达式，在求值后返回一个值`6`。

调用一个函数是表达式，因为会返回一个值，调用宏也是表达式，用花括号包裹最终返回一个值的语句块也是表达式，总之，能返回值，就是表达式。

```rust
fn main() {
    let y = {
        let x = 3;
        x+1
    }
    
    println!("{}",y)
}
```

上面用一个语句块表达式将值赋给y变量。该语句块是表达式的原因是：最后一行是表达式，返回来`x+1`的值，注意`x+1`不能以分号结尾，否则会从表达式变成语句，**表达式不能包含分号**。一旦在表达式后加上分号，就会编程一条语句，再也不会返回一个值。如果表达式不返回任何值，会隐式地返回一个`()`。

### 函数

#### 函数返回

在Rust中函数就是表达式。函数的返回值就是函数体最后一条表达式的返回值，也可以使用`return`提前返回。

永不返回的发散函数：当用`!`做函数返回类型的时候，表示该函数永不返回。这种语法往往用作会导致程序崩溃的函数：

```rust
fn dead_end() -> ! {
    panic!("程序崩溃")
}

fn forever() -> ! {
    loop {
        // ...
    }
}
```

## 所有权和借用

### 所有权

```
1. Rust中的每一个值都被一个变量所拥有，该变量被称为值的所有者
2. 一个值同时只能被一个变量所拥有，或者说一个值只能拥有一个所有者
3. 当所有者（变量）离开作用域时，这个值将被丢弃
```

#### 变量绑定背后的数据交互

##### 转移所有权

```rust
let x = 5;
let y = x;
```

代码逻辑：将`5`绑定到变量`x`，接着拷贝`x`的值赋给`y`，最终`x`和`y`都等于5。

接下来看一段代码：

```rust
fn string_copy()->(){
    let s1= String::from("hello");

    let s2 = s1;
    println!("s1 = {}, s2 = {}",s1,s2);
}
```

这里由于`String`类型是一个复杂类型，由存储在栈中的堆指针、字符串长度、字符串容量共同组成，其中堆指针是最重要的，指向了真实存储字符串内容的堆内存。

总之`String`类型指向了一个堆上的空间，存储着真实数据，下面分析`let s2 = s1;`:

-   拷贝`String`和存储在堆上的字节数组：如果该语句是拷贝所有数据(深拷贝)，那么无论是`String`本身还是底层的堆上数据，都会被全部拷贝，这对于性能而言会造成非常大的影响。
-   只拷贝`String`本身：这样拷贝非常快，但是带来了新的问题，由于所有权规定`一个值只允许有一个所有者`，而现在这个值有了两个所有者：`s1`和`s2`。

当变量离开作用域后，Rust会自动调用`drop`函数并清理变量的堆内存。不过由于两个`String`变量指向了同一个位置。这就造成一个问题：当`s1`和`s2`离开作用域后，都会尝试释放相同的内存，会产生**二次释放(double free)**的错误，可能会导致潜在的安全漏洞。

因此，Rust规定:当`s1`赋予`s2`后，Rust认为`s1`不再有效，无需在`s1`离开作用域后`drop`任何东西，这就是把所有权从`s1`转移给`s2`，`s1`在被赋予`s2`后就马上失效。

再来看一段代码：

```rust
fn str_copy()->(){
    let s1= "hello";

    let s2 = s1;
    println!("s1 = {}, s2 = {}",s1,s2);
}
```

这段代码里，`s1`只引用了存储在二进制中的字符串`hello`，并没有持有所有权。因此当`let s2 = s1;`时，仅仅对该引用进行了拷贝，此时`s1`和`s2`都引用了同一个字符串。

#### 克隆（深拷贝）

Rust永远不会自动创建数据的“深拷贝”。因此任何自动的复制都不是深拷贝。

如果确实需要深度复制`String`中堆上的数据，而不仅仅是栈上的数据，可以使用`clone`的方法。

#### 拷贝（浅拷贝）

浅拷贝只发生在栈上，因此性能很高。

例如：

```rust
let x = 5;
let y = x;

println!("{},{}",x,y)
```

这段代码是没有问题的。原因是整体这种基本类型在编译时是已知大小的，会被存储在栈上，所以拷贝其实际的值是快速的。这里没有深浅拷贝的区别。

Rust有一个叫做`Copy`的特征，可以用在类型整型这样在栈中存储的类型。如果一个类型拥有`Copy`特征，一个旧的变量在被赋值给其他变量后仍然可用。

**任何基本类型的组合可以`Copy`，不需要分配内存或某种形式资源的类型是可以`Copy`的**。

例如：

-   所有整数类型
-   布尔类型
-   所有浮点数类型
-   字符类型
-   元组：当且仅当其包含的类型也都是`Copy`的时候
-   不可变引用`&T`。可变引用`&mut T`是不可以`Copy`的

#### 函数传值与返回

将值传递给函数，一样会发生**移动**或者**复制**，就跟`let`语句一样。

### 引用与借用

借用(Borrowing)：获取变量的应用。

#### 引用与解引用

常规引用是一个指针类型，指向了对象存储的内存地址。例如：

```rust
fn borrowing(){
    let x = 5;
    let y = &x;
    println!("x = {:?}, y = {:?}",x,*y)
}
```

这里变量`x`存放了一个`i32`值5，`y`是`x的一个应用。`

#### 不可变引用

```rust
fn immutable_ref() {
    let s1 = "hello".to_string();
    let len = calc_length(&s1);

    println!("s1 = {}, len = {}", s1, len)
}

fn calc_length(s: &String) -> usize {
    s.len()
}
```

这里`&`符号表示引用，允许使用值，但是不获取所有权。

通过`&s1`语法，创建一个指向`s1`的引用，但是并不拥有它。因为并不拥有这个值，当应用离开作用域后，其指向的值也不会被丢弃。

同理，函数`calc_length`使用`&`来表明参数`s`的类型是一个引用。

#### 可变引用

```rust
fn mutable_ref() {
    let mut s = "hello".to_string();
    insert_str(&mut s);
    println!("mutable_ref {:?}", s);
}

fn insert_str(s: &mut String) {
    s.push_str(", world");
}
```

首先，声明`s`是一个可变类型，其次创建一个可变的引用`&mut s`和接受可变引用参数`s: &mut String`的函数。

#### 可变引用同时只能存在一个

同一个作用域，特定数据只能有一个可变引用。

#### 可变引用与不可变引用不能同时存在

```
引用的作用域s从创建开始，一直持续到它最后一次使用的地方，这个跟变量的作用域有所不同，变量的作用域从创建持续到某一个花括号}
```

#### NLL

引用作用域这种编译器优化行为，在Rust中称为Non-Lexical LifeTimes(NLL)，专门用于找到某个引用在作用域结束前就不再被使用的代码位置。

#### 悬垂引用

悬垂引用也被叫做悬垂指针，意思是指针指向某个值后，这个值被释放掉了，而指针仍然存在，其指向的内存可能不存在任何值或已经被其他变量重新使用。Rust编译器保证数据不会在引用结束前被释放，要想释放数据，必须先停止其引用的使用。

#### 借用规则总结

同一时刻，只能拥有一个可变引用，或者任意多个不可变引用。

## 复合类型

### 字符串与切片

#### 字符串

#### 切片

对于字符串而言，切片就是对`String`类型中的某一部分的引用，例如：

```rust
fn slice_study(){
    let s = "hello, world".to_string();
    let hello = &s[0..5];

    println!("s = {}, hello = {}",s,hello);
}
```

`hello`没有引用整个`String s`，而是引用了`s`的一部分内容，通过`[0..5]`的方式来指定。这就是创建切片的语法，使用方括号包括的一个序列：**[开始索引..终止索引]**，其中开始索引时切片中的第一个元素的索引位置，而终止索引时最后一个元素后面的索引位置，也就是一个右半开区间。在切片数据结构内部会保存开始的位置和切片的长度，其中长度是通过终止索引-开始索引的方式计算得来的。

在使用Rust的`..`range序列语法时，如果要从索引0开始，可以使用如下方式：

```rust
let hello = &s[0..5];
let hello = &s[..5];
```

如果切片想要包含`String`的最后一个字节，可以使用如下方式：

```rust
let len = s.len();
let world = &s[7..len]
let world = &s[7..]
```

同时截取完整的`String`切片：

```rust
let len = s.len();
let str = &s[0..len]
let str = &s[..]
```

```
在对字符串使用切片语法时需要格外注意，切片的索引必须落在字符之间的边界位置，也就是UTF-8字符的边界。如果只取某个字符的前面字节，没有落在边界处，程序会直接崩溃退出。
```

字符串切片的类型是`&str`