# vtab和vptr

一个类在定义虚函数时，会生成一个虚表（vtab)和一个虚指针(vptr)。在派生时，会继承该虚表，但是虚表的首地址和虚指针的地址都会变化，同时在虚表中会隐藏自身的虚函数。如果一个类多重继承时，多个基类含有虚表，此时，子类中会有多个虚表和虚指针。

注：子类生成的基类的虚表和基类自己的虚表地址不相同，但同一个class生成的对象，共享同一虚表和虚指针。

==实例1：==

~~~c++
#include<iostream>
using namespace std;

class X
{
	virtual int foo()
	{
		return 1;
	}
	void dfg() {}
	virtual void f(){}
};
class Y :virtual public X
{
	virtual int foo() { return 0; }
	virtual void df(){}
};
class z :virtual public X
{
	virtual void sd() {};
};
int main(void)
{
	X x;
	X x1;
	Y y1;
	Y y2;
	z z1;
	system("pause");
	return 0;
}
~~~

结果：

|      | 名称      | 值                                                           | 类型     |
| ---- | --------- | ------------------------------------------------------------ | -------- |
| ◢    | x         | {...}                                                        | X        |
|      | ◢ __vfptr | 0x00dd7b34 {Project1.exe!void(* X::`vftable'[3])()} {0x00dd1212 {Project1.exe!X::foo(void)}, 0x00dd1186 {Project1.exe!X::f(void)}} | void * * |
|      | [0]       | 0x00dd1212 {Project1.exe!X::foo(void)}                       | void *   |
|      | [1]       | 0x00dd1186 {Project1.exe!X::f(void)}                         | void *   |

|      | 名称      | 值                                                           | 类型     |
| ---- | --------- | ------------------------------------------------------------ | -------- |
| ◢    | x1        | {...}                                                        | X        |
|      | ◢ __vfptr | 0x00dd7b34 {Project1.exe!void(* X::`vftable'[3])()} {0x00dd1212 {Project1.exe!X::foo(void)}, 0x00dd1186 {Project1.exe!X::f(void)}} | void * * |
|      | [0]       | 0x00dd1212 {Project1.exe!X::foo(void)}                       | void *   |
|      | [1]       | 0x00dd1186 {Project1.exe!X::f(void)}                         | void *   |

|      | 名称      | 值                                                           | 类型     |
| ---- | --------- | ------------------------------------------------------------ | -------- |
| ◢    | y1        | {...}                                                        | Y        |
|      | ◢ X       | {...}                                                        | X        |
|      | ◢ __vfptr | 0x00dd7b50 {Project1.exe!void(* Y::`vftable'[3])()} {0x00dd131b {Project1.exe!Y::foo(void)}, 0x00dd1186 {Project1.exe!X::f(void)}} | void * * |
|      | [0]       | 0x00dd131b {Project1.exe!Y::foo(void)}                       | void *   |
|      | [1]       | 0x00dd1186 {Project1.exe!X::f(void)}                         | void *   |
|      | ◢ __vfptr | 0x00dd7b44 {Project1.exe!void(* Y::`vftable'[2])()} {0x00dd1208 {Project1.exe!Y::df(void)}} | void * * |
|      | [0]       | 0x00dd1208 {Project1.exe!Y::df(void)}                        | void *   |

|      | 名称      | 值                                                           | 类型     |
| ---- | --------- | ------------------------------------------------------------ | -------- |
| ◢    | y2        | {...}                                                        | Y        |
|      | ◢ X       | {...}                                                        | X        |
|      | ◢ __vfptr | 0x00dd7b50 {Project1.exe!void(* Y::`vftable'[3])()} {0x00dd131b {Project1.exe!Y::foo(void)}, 0x00dd1186 {Project1.exe!X::f(void)}} | void * * |
|      | [0]       | 0x00dd131b {Project1.exe!Y::foo(void)}                       | void *   |
|      | [1]       | 0x00dd1186 {Project1.exe!X::f(void)}                         | void *   |
|      | ◢ __vfptr | 0x00dd7b44 {Project1.exe!void(* Y::`vftable'[2])()} {0x00dd1208 {Project1.exe!Y::df(void)}} | void * * |
|      | [0]       | 0x00dd1208 {Project1.exe!Y::df(void)}                        | void *   |

|      | 名称      | 值                                                           | 类型     |
| ---- | --------- | ------------------------------------------------------------ | -------- |
| ◢    | z1        | {...}                                                        | z        |
|      | ◢ X       | {...}                                                        | X        |
|      | ◢ __vfptr | 0x00dd7b78 {Project1.exe!void(* z::`vftable'[3])()} {0x00dd1212 {Project1.exe!X::foo(void)}, 0x00dd1186 {Project1.exe!X::f(void)}} | void * * |
|      | [0]       | 0x00dd1212 {Project1.exe!X::foo(void)}                       | void *   |
|      | [1]       | 0x00dd1186 {Project1.exe!X::f(void)}                         | void *   |
|      | ◢ __vfptr | 0x00dd7b6c {Project1.exe!void(* z::`vftable'[2])()} {0x00dd1352 {Project1.exe!z::sd(void)}} | void * * |
|      | [0]       | 0x00dd1352 {Project1.exe!z::sd(void)}                        | void *   |

==实例2==

~~~c++
#include<iostream>
using namespace std;

class X
{
public:
	virtual void foo() {}
};
class X1 :public X
{
public:
	virtual void dff() {}
};
class Y
{
public:
	virtual void fg() {}
};
class Z :public X, public Y
{
public:
	virtual void df() {}
};
int main(void)
{
	X x;
	X1 x1;
	x1.dff();
	Y y;
	Z z;
	Z z1;
	system("pause");
	return 0;
}
~~~

|      | 名称      | 值                                                           | 类型     |
| ---- | --------- | ------------------------------------------------------------ | -------- |
| ◢    | x         | {...}                                                        | X        |
|      | ◢ __vfptr | 0x01189b34 {Project1.exe!void(* X::`vftable'[2])()} {0x01181262 {Project1.exe!X::foo(void)}} | void * * |
|      | [0]       | 0x01181262 {Project1.exe!X::foo(void)}                       | void *   |
| ◢    | x1        | {...}                                                        | X1       |
|      | ◢ X       | {...}                                                        | X        |
|      | ◢ __vfptr | 0x01189b40 {Project1.exe!void(* X1::`vftable'[3])()} {0x01181262 {Project1.exe!X::foo(void)}} | void * * |
|      | [0]       | 0x01181262 {Project1.exe!X::foo(void)}                       | void *   |
| ◢    | y         | {...}                                                        | Y        |
|      | ◢ __vfptr | 0x01189b54 {Project1.exe!void(* Y::`vftable'[2])()} {0x01181032 {Project1.exe!Y::fg(void)}} | void * * |
|      | [0]       | 0x01181032 {Project1.exe!Y::fg(void)}                        | void *   |
| ◢    | z         | {...}                                                        | Z        |
|      | ◢ X       | {...}                                                        | X        |
|      | ◢ __vfptr | 0x01189b60 {Project1.exe!void(* Z::`vftable'[3])()} {0x01181262 {Project1.exe!X::foo(void)}} | void * * |
|      | [0]       | 0x01181262 {Project1.exe!X::foo(void)}                       | void *   |
|      | ◢ Y       | {...}                                                        | Y        |
|      | ◢ __vfptr | 0x01189b70 {Project1.exe!void(* Z::`vftable'[2])()} {0x01181032 {Project1.exe!Y::fg(void)}} | void * * |
|      | [0]       | 0x01181032 {Project1.exe!Y::fg(void)}                        | void *   |
| ◢    | z1        | {...}                                                        | Z        |
|      | ◢ X       | {...}                                                        | X        |
|      | ◢ __vfptr | 0x01189b60 {Project1.exe!void(* Z::`vftable'[3])()} {0x01181262 {Project1.exe!X::foo(void)}} | void * * |
|      | [0]       | 0x01181262 {Project1.exe!X::foo(void)}                       | void *   |
|      | ◢ Y       | {...}                                                        | Y        |
|      | ◢ __vfptr | 0x01189b70 {Project1.exe!void(* Z::`vftable'[2])()} {0x01181032 {Project1.exe!Y::fg(void)}} | void * * |
|      | [0]       | 0x01181032 {Project1.exe!Y::fg(void)}                        | void *   |