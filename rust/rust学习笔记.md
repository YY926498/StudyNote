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

