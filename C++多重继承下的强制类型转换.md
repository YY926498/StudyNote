# C++多重继承下的强制类型转换

考虑下列程序：

~~~c++
#include <iostream>
using namespace std;
class A
{
public:
	virtual void fa()
	{
		cout << "this is A fa" << endl;
	}
	int a;
};
class B
{
public:
	virtual void fb()
	{
		cout << "this is B fb" << endl;
	}
	virtual void fb1()
	{
		cout << "this is B fb1" << endl;
	}
	int b;
};
class C :public A, public B
{
public:
	int c;
};
int main(void)
{
	C* pc = new C();
	A* pa = (A*)pc;
	B* pb = (B*)pc;

	
	pb->fb();
	pb->fb1();

	B* pb1 = dynamic_cast<B*>(pa);
	pb1->fb();
	pb1->fb1();
	
	B* pb2 = (B*)pa;
	cout << std::hex << "pa addr:" << pa << "\npb addr:" << pb << endl;
	cout << "pc addr:" << pc << "\npb1 addr:" << pb1 << endl;
	cout << "pb2 addr:" << pb2 << endl;
	cout << "(B*)pa addr:" << (B*)pa << endl;
	cout << "(C*)pb1 addr:" << (C*)pb1 << endl;
	cout << "(C*)pb2 addr:" << (C*)pb2 << endl;
	pb2->fb();
	pb2->fb1();
	return 0;
}
~~~





~~~c++
this is B fb
this is B fb1
this is B fb
this is B fb1
pa addr:0016EE20
pb addr:0016EE28
pc addr:0016EE20
pb1 addr:0016EE28
pb2 addr:0016EE20
(B*)pa addr:0016EE20
(C*)pb1 addr:0016EE20
(C*)pb2 addr:0016EE18
this is A fa
异常
~~~

按照内存布局，`C`类中，`A`类排在最前面，然后是B类，然后是`C`类自身数据。

- 通过派生类指针转换而来的基类指针，会自动计算偏移量，如`pb`。因此，在调用虚函数时，不会出现问题。
- 通过某个基类指针进行动态类型转换，也会自动计算偏移量，如`pb1`。
- 通过某个基类指针强制类型转换到另一个基类指针，此时不会计算偏移量，如`pb2`，这时pb2指向的虚函数表是类A的虚函数表，因此`pb2->fb()`实际指的是类A中的虚函数表的第一项，由于类A中虚函数表只有一项，因此`pb2->fb1()`指向的是一个空地址，因此报错

