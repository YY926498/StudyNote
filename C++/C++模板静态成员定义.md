# C++模板静态成员定义

```c++
template <typename T> class Test{
public:
    static std::string info;
};
```

如果有个模板如上，对于info的定义，下列哪种方式是正确的：

```c++
template <> string Test<int>::info("123");
template <typename T> string Test<T>::info("123");
template <typename T> string Test<T>::info;
template <> string Test<int>::info; 
template <> string Test<int>::info();
template <typename T> string Test<T>::info();
```

问题解答：

前三种正确，后三种错误。

第一种形式称之为特化定义，其作用是为模板某一特化提供其静态成员的定义，在我们例子中，它仅仅为Test<int>类的静态成员info提供定义。并且调用单参数构造函数初始化。

第 二种形式类似于普通类的静态成员的定义方式，其作用是隐式地在该编译单元为模板的所有特化提供其静态成员的定义，在我们例子中，在首次使用 Test<int>,Test<float>,Test<char>...会隐式提供静态成员的定义，并且调用单参 数构造函数初始化。

第三种形式和第二种形式一致，唯一不同就是采用默认构造函数初始化。

其次，说明一下三个错误的答案。

第一种形式，很多人都会认为是对的，认为它采用默认构造函数初始化。但编译器会对特化定义进行特殊处理，编译认为它是一个声明而非定义。至于为什么如此，应该询问一下制定标准的人。我认为可能实现这样的语法可能比较困难并且这个语法也比较鸡肋。

第二种形式，这不成了声明一个函数啦。

第三种形式，同第二种。



更多

```c++
//a.cpp
template <typename T> string Test<T>::info("4321");
可以使用Test<int>::info
//b.cpp
template <typename T> string Test<T>::info("1234");
也可以使用Test<int>::info
```

上述两个定义可以再不同的编译单元中共存。

```c++
//a.cpp
template <> string Test<int>::info("123");
//b.cpp
template <> string Test<int>::info("123");
```

而特化定义，上述方式无法通过编译。

```c++
//a.cpp
template <> string Test<int>::info("123");

//b.cpp
template <typename T> string Test<T>::info("123");

一旦使用Test<int>::info无法通编译 
```

上述方式无法通过编译。

 

一般为了避免无法编译，应当尽量减少使用，如下方式的定义

```c++
template <typename T> string Test<T>::info;
```

只有在你首次需要使用时在实现文件中给出如下特化定义即可，其他文件只要包含头文件就能使用。

```c++
template <> string Test<int>::info("123");
```

