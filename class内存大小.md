# class内存大小

- 其中nonstatic data members 的总和大小
- 任何由于alignment(译注)[^1]的需求而填补（padding）上去的空间（可能存在于members之间，也可能存在于集合体边界）
- 加上为了支持virtual而由内部产生的任何额外负担（overhead）

实例：

~~~c++
#include<iostream>
using namespace std;
typedef unsigned int bit;
class Z
{

};
class X:public Z
{
protected:
	int x;
};

class Y :public X
{
protected:
	bit mk : 31;
	bit km : 1;
	char y;
};
class Y1 :public X
{
protected:
	bit mk : 31;
	bit km : 2;
	char y;
};

int main(void)
{
	cout << "empty class=" << sizeof(Z) << endl;
	cout << "X=" << sizeof(X) << endl;
	cout << "Y=" << sizeof(Y) << endl;
	cout << "Y1=" << sizeof(Y1) << endl;
	cout << "char=" << sizeof(char) << endl;
	cout << "int=" << sizeof(int) << endl;
	system("pause");
	return 0;
}
~~~

结果：

~~~c++
empty class=1
X=4
Y=12
Y1=16
char=1
int=4
~~~

分析上面的实例：可知：

------------------

空类占一个字节

如果一个类派生于空类，空类所占字节变为0，然后计算该派生类所占字节，所占字节会进行对齐。如字节和为5~7会对齐为8。

对于位域，一个位域变量最大为32，即4个字节。如果有多个位域变量，它们的和不大于32，那个会对齐为32位。

在多重继承的时候，会将每个基类都对齐后，然后计算总和。

[^1]: alignment就是讲数值调整到某数的倍数。在32位计算机中，通常alignment为4byte(32位)，以使bus的“运输量”达到最高效率

