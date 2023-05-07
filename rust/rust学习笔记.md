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