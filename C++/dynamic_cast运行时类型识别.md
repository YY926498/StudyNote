# RTTI运行时类型识别

通过RTTI，能够通过基类的指针或引用来检索其所指对象的实际类型。C++通过下面两个操作符提供RTTI。

- `typeid`：返回指针或引用所指对象的实际类型
- `dynamic_cast`：将基类类型的指针或引用安全的转换为派生类的指针或引用

对于带虚函数的类，在运行时执行`RTTI`操作符，返回其动态类型信息；对于其他类型，在编译时执行`RTTI`，返回其静态类型信息。

当具有基类的指针或引用，但需要执行派生类操作时，需要动态的强制类型转换。这种机制的使用容易出错，最好以虚函数机制代替。

dynamic_cast的用法：

~~~c++
dynamic_cast<type*> (e)
dynamic_cast<type&> (e)
dynamic_cast<type&&>(e)    
~~~

指针类型的dynamic_cast

~~~c++
if(Derived *dp=dynamic_cast<Derived*>(bp))
{
}
else
{
}
~~~

引用类型的dynamic_cast

~~~c++
void f(const Base &b)
{
    try{
        const Drived &d=dynamic_cast<const Derived&>(b);
    }
    catch(bad_cast)
    {
        
    }
}
~~~

关于dynamic_cast为什么指针和引用的使用形式不同：由于引用必须指向有地址的对象，不存在所谓的空引用，因此引用的错误类型报告与指针不同，需要使用try_catch来处理类型转换失败的情况

**样例**

~~~c++
#include<iostream>
#include<string>

using namespace std;

class A
{
public:
	explicit A(int i) 
	{
		b = i;
		a = b + 1;
	}
	virtual void count(int i = 21)
	{
		cout << i << endl;
	}
private:
	int a;
	int b;
};
class B : public  A
{
public:
	explicit B(int i, int j) :A(i), c(j) {	}
	void count(int i = 12)
	{
		cout << i << endl;
	}
private:
	int c;
};
void num(A *a)
{
	
	if (B *b = dynamic_cast<B*>(a))
	{
		(*b).count();
	}
}
void num(A &a)
{
	try
	{
		B &b = dynamic_cast<B&>(a);
		b.count();
	}
	catch (bad_cast)
	{
		cout << "error" << endl;
	}
}
int main(void)
{
	A a(12);
	B b(1, 2);
	A *p = &b;
	a.count();
	b.count();
	p->count();
	num(p);
	num(*p);
	num(a);
	system("pause");
	return 0;
}
~~~

结果：

~~~c++
21
12
21
12
12
error
~~~

从上面的实例可以看出，如果转换的原对象跟目的对象不同，引用会抛出bad_cast异常。并且当进行多态操作时，==虚函数的默认实参由基类的默认实参决定==。

###  `typeid`操作符

`typeid`能够获取一个表达式的类型：`typeid(e)`。

如果操作数不是类类型或者是没有虚函数的类，则获取其静态类型；如果操作数是定义了虚函数的类类型，则计算运行时类型。

`typeid`最常见的用途是比较两个表达式的类型，或者将表达式的类型与特定类型相比较。

~~~c+
Base *bp;
Derived *dp;
// compare type at run time of two objects
if (typeid(*bp) == typeid(*dp))
{
    // bp and dp point to objects of the same type
}
// test whether run time type is a specific type
if (typeid(*bp) == typeid(Derived))
{
    // bp actually points a Derived
}
~~~

注意：如果是`typeid(bp)`，则是对指针进行测试，这会返回指针`（bp）`的静态编译时类型`（Base *）`。

如果指针p的值是0，,并且指针所指的类型是带虚函数的类型，则`typeid(*p)`抛出一个`bad_typeid`异常。