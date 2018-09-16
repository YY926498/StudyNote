# vtab和vptr

一个类在定义虚函数时，会生成一个虚表（vtab)和一个虚指针(vptr)。在派生时，子类会继承基类，同时为基类再生成一个虚表和虚指针，如果自己也含有虚函数，会另外生成一个虚表和虚指针。因此一个类可能有多个虚表和虚指针。同时，如果一个子类有个虚函数override基类的虚函数，此时子类自己的虚表中没有该虚函数，这是因为子类生成的基类虚表包含该虚函数。

注：子类生成的基类的虚表和基类自己的虚表地址不相同，但同一个class生成的对象，共享同一虚表和虚指针。

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