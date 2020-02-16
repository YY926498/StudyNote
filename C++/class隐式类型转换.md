# class隐式类型转换

如果一个类的单变量构造函数未声明explicit，那么就可能会有发生隐式类型转换的风险。另外class中有种member function起到类隐式类型转换函数，格式如下：

~~~c++
operator type(){}
~~~

其中，type为目标转换类型，如int, double等等。如果定义该函数，那么在程序中就有可能发生隐式类型转换，如果在程序编写时没有注意这些，就可能导致程序错误。

实例：

~~~c++
#include<iostream>

using namespace std;

class X
{
public:
	X(int a) :x(a) {}
	operator int() 
	{ 
		return x; 
	}
private: 
	int x;
};
int main(void)
{
	X x1(2);
	X x2 = 3;
	cout << x1 << endl;
	system("pause");
	return 0;
}
~~~

输出：

~~~c++
2
~~~

根据`X x2=3;`这条程序，x2中的构造函数被调用，此时x2.x=3；

`cout<<x1<<endl;`由于class X中未定义`<<`运算符，所以将x1进行类型转换，编程int型输出。

