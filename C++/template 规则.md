# template 规则

template arguments(模板实参)的一个一般性约束条件是：必须能够在编译期或链接期求值。（只在执行时才能求值的运算式（例如某局部变量的地址）不能作为nontype template arguments使用）。

关于nontype template arguments还有几条规则：

- derived-to-base转换不被考虑：
- 成员变量不被考虑
- 不能使用array内某个元素的地址

~~~c++
template<typename T,T nontype_pram>
class C;

class Base{
    public:
    int i;
}base;
class Derived:public Base{
}derived_obj;
C<Base*,&derived_obj> *err1;//error:[derived-to-base转换]不被考虑
C<int&,base.i>* err2;//error:成员变量不被考虑
int a[10];
C<int*, &a[0]>* err3;//error:不能使用array内某个元素的地址
~~~

1. `template template argument`必须是这样一个`class template`:其参数完全匹配待替换的`template template parameter`的参数。这是因为`template template argument`的`default template argument`(默认模板实参)会被编译器忽略，除非对应的`template template parameter`有默认参数。
2. 一个由function template 产生的函数和一个常规函数，无论如何不会被编译器视为等价，即使类型和名称完全相同，这对class members造成两个重要结果：

~~~c++
1. 由member function template 产生的函数不会覆盖虚函数
2. 由constructor template 产生的构造函数不会被当做default copy构造函数（同理，assignment template 也类似）
~~~

3. 