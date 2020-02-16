# class类的大小（补）

#### 在win32平台下，微软的编译器在默认情况下采用如下的对齐规则，任何基本数据类型T的对齐模数就是T的大小，即sizeof(T),比如对于double类型（8字节），就要求该类型数据的地址总是8的倍数，而char类型数据（1字节）则可以从任何一个地址开始

- 一个普通类的大小等于该类的每个非静态数据成员（nonstatic data member）对齐后的总和。但如果class中有位域后，应该将所有位域变量加起来后再对齐。如例中的class A。
- 一个子类普通继承基类，并且基类跟子类均没有虚函数和static成员，那么子类的大小等于基类的非静态数据成员之和加子类非静态数据成员之和。如例中的class A1，class A2。
- 如果一个类含有虚函数，则该类的大小等于每个非静态数据成员之和，如class B。
- 如果一个类普通继承基类，并且基类和子类均有虚函数，此时子类的大小等于基类的非静态数据成员加子类的非静态数据成员加虚指针，两个类公用同一虚指针。如class B1, class B2, class B3。
- 如果一个普通类多重继承若干个普通类，如果基类中含有虚函数，并且自身也有虚函数，那么按照从左到右的顺序，如果有个基类有虚函数表，那么子类的虚函数放在左侧第一个含有虚函数表的基类的虚函数表后面。如果还有其他基类还有虚函数表，那么虚指针仍然保留。如class B4。
- 如果类内含有静态数据成员，该成员不计入class大小。这是因为静态数据成员类似于全局变量。每个对象的内存布局不包括static成员。如class C1, class C2, class C3。
- 如果一个类内有若干个数据成员，每个数据成员被对齐成最大的一个，并且包含虚指针和虚基类的偏移指针。并且子类的对齐不影响基类的对齐。如class D1, class D2。
- 一个类如果虚继承基类，那么该类的大小等于子类的非静态数据成员+虚指针+虚基类的偏移指针+基类的大小。其中子类的非静态数据成员，虚指针，虚基类的偏移指针都会受子类最大变量的大小而对齐。如果一个类同时虚继承多个基类，那么虚基类的偏移指针只会计算一次，如E1~E7。

实例：平台为win10的VS2017，使用x86

~~~c++
#include<iostream>
using namespace std;
typedef unsigned int bit;
class A
{
	int a;
	char c;
	bit mo : 2;
	bit df : 1;
};
class A1 :public A
{
	int a1;
	char c1;
};
class A2 :public A1
{
	int a2;
	char c2;
};
class B
{
	virtual void foo(){}
};
class B1 :public B
{
	virtual void foo1(){}
};
class B2 :public B1
{
	virtual void foo2(){}
};
class B3
{
	virtual void foo3(){}
};
class B4 :public B2, public B3
{
	virtual void foo4(){}
};
class C1
{
	static void f(){}
	static int n;
};
class C2 :public C1
{
	static void f1(){}
	static double d;
};
class C3 :public virtual  C2
{
	static void f2(){}
	static char c;
};
//在win32平台下，微软的编译器在默认情况下采用如下的对齐规则，任何基本数据类型
//T的对齐模数就是T的大小，即sizeof(T).比如对于double类型（8字节），就要求
//该类型数据的地址总是8的倍数。而char类型数据（1字节）则可以从任何一个地址开始
//因此D1的大小仅为1字节
//而D2
class D1
{
	char c;
};
class D2 :public virtual D1
{
	int fg;
	double h;
	void f(){}
};
class E1
{
	virtual void f() {	}
	char c;
};
class E2 :public virtual E1
{
	virtual void fg(){}
	char d;
};
class E3
{
	virtual void df(){}
	char d;
};
class E4 :public E3
{
	char d;
};
class E5 :public E2, public E4
{
	virtual void f4(){}
	char d;
};
class E6 :public virtual E2, public virtual E4
{
	virtual void f4(){}
	char d;
};
class E7 :public virtual E2, public virtual E4
{
	virtual void f4() {}
	double d;
};
int main(void)
{
	cout << "sizeof(A)=" << sizeof(A) << endl;
	cout << "sizeof(A1)=" << sizeof(A1) << endl;
	cout << "sizeof(A2)=" << sizeof(A2) << endl;
	cout << "sizeof(B)=" << sizeof(B) << endl;
	cout << "sizeof(B1)=" << sizeof(B1) << endl;
	cout << "sizeof(B2)=" << sizeof(B2) << endl;
	cout << "sizeof(B3)=" << sizeof(B3) << endl;
	cout << "sizeof(B4)=" << sizeof(B4) << endl;
	cout << "sizeof(C1)=" << sizeof(C1) << endl;
	cout << "sizeof(C2)=" << sizeof(C2) << endl;
	cout << "sizeof(C3)=" << sizeof(C3) << endl;
	cout << "sizeof(D1)=" << sizeof(D1) << endl;
	cout << "sizeof(D2)=" << sizeof(D2) << endl;
	cout << "sizeof(E1)=" << sizeof(E1) << endl;
	cout << "sizeof(E2)=" << sizeof(E2) << endl;
	cout << "sizeof(E3)=" << sizeof(E3) << endl;
	cout << "sizeof(E4)=" << sizeof(E4) << endl;
	cout << "sizeof(E5)=" << sizeof(E5) << endl;
	cout << "sizeof(E6)=" << sizeof(E6) << endl;
	cout << "sizeof(E7)=" << sizeof(E7) << endl;
	system("pause");
	return 0;
}
~~~

结果

~~~c++
sizeof(A)=12
sizeof(A1)=20
sizeof(A2)=28
sizeof(B)=4
sizeof(B1)=4
sizeof(B2)=4
sizeof(B3)=4
sizeof(B4)=8
sizeof(C1)=1
sizeof(C2)=1
sizeof(C3)=4
sizeof(D1)=1
sizeof(D2)=25
sizeof(E1)=8
sizeof(E2)=20
sizeof(E3)=8
sizeof(E4)=12
sizeof(E5)=36
sizeof(E6)=44
sizeof(E7)=56
~~~

