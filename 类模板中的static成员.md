# 类模板中的static成员

在类模板Foo中，定义static成员，那么每个Foo的实例都有其自己的static成员实例。即对任意给定类型X，都有一个Foo<X> ::ctr(其中ctr为Foo中的static成员)。所有Foo<X>类型的对象共享相同的ctr对象。

---

例如：

~~~c++
#include<iostream>
using namespace std;
template <typename T>
class Foo {
public:
	static size_t count() {
		return ctr;
	}
	void addctr() {
		ctr++;
	}
	void subctr() {
		ctr--;
	}
private:
	static size_t ctr;
	// 下面可以有其他成员接口
};
template <typename T>
size_t Foo<T>::ctr = 0;    //定义并初始化 ctr
int main(void)
{
	Foo<int> fi1;
	fi1.addctr(); 
	Foo<int> fi2;
	cout << fi2.count() << endl;
	system("pause");
	return 0;
}
~~~

输出结果：

~~~c++
1
~~~

说明由于fi1对ctr成员执行自增，fi2中的ctr成员指向的也是fi1中的ctr成员。