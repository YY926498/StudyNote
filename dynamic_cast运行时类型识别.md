# dynamic_cast运行时类型识别

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

从上面的实例可以看出，如果转换的原对象跟目的对象不同，引用会抛出bad_cast异常。并且当进行多态操作时，-==虚函数的默认实参由基类的默认实参决定==。