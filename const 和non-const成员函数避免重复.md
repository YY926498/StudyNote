# const 和non-const成员函数避免重复

如果成员函数有个const区别，代表成员函数被重载。

由于两类函数有部分功能重复，因此可以定义一个功能，另外一个函数调用另一个重载的成员函数。

实例：

~~~c++
class X
{
public:
    /*前面的const代表返回值是const,这是因为在调用该函数时可能会出现问题，例如：
    if(x[1]=2),本来的意思可能是判断x[1]是否等于2，但因为少打一个字符，就造成错误，如果不加const限定符，编译器不会报错，因此，需要加入const。
    后面的const代表该函数由const对象调用，在函数体内，一般不改变对象成员。*/
    const char& operator[](size_t pos) const
    {
        ...
        return text[pos];    
    }
    char & operator[](size_t pos)
    {
        return const_cast<char &>(static_cast<const X&>(*this)[pos]);
    }
private:
    char *text;
}
~~~

再`char & operator[](size_t pos)`中，使用`static_cast`先将对象转成const类型，避免无穷递归。转成const类型后，调用`const char& operator[](size_t pos) const`函数，然后将返回的const类型通过`const_cast`转换符，删去const属性。

为什么不先定义non-const函数，再用const函数调用non-const函数？

**这是因为const成员函数承诺绝不改变其对象的逻辑状态，如果在const成员函数内调用non-const成员函数，就会冒这种风险。**