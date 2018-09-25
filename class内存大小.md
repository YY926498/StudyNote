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

空类占一个字节。这是由于类的实例化就是为每个实例在内存中分配一块地址，每个类在内存中都有唯一的标识，因此空类被实例化，编译器会隐含地为其添加一个字节，作为区分。

如果一个类的数据成员为空，但是有虚函数，编译器会为其隐式分配一个虚指针，指向虚函数表，该表的第一个表示类的类型信息，第二个开始代表虚函数。因此该类的大小为4字节。

如果一个类中有static数据成员，该成员不计算到类的大小中，这是因为static成员属于全局数据段，所有该类的对象共享。

普通类的大小仅与数据成员有关。

如果一个类派生于空类，空类所占字节变为0，然后计算该派生类所占字节，所占字节会进行对齐。如字节和为5~7会对齐为8。

对于位域，一个位域变量最大为32，即4个字节。如果有多个位域变量，它们的和不大于32，那个会对齐为32位。

在多重继承的时候，会将每个基类都对齐后，然后计算总和。

虚函数类的继承，派生类的大小=派生类的自身成员大小+基类数据成员大小+虚指针大小。

注对于64位系统，数值可能会变化。

[^1]: alignment就是讲数值调整到某数的倍数。在32位计算机中，通常alignment为4byte(32位)，以使bus的“运输量”达到最高效率

