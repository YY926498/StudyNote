# 底层const

在定义底层const时，指针所指向的变量不一定是常量，底层const代表的是不能通过指针修改变量的值，但是可以通过其他方式修改变量的值。例：

~~~c++
#include <iostream>
using namespace std;
int main()
{
	int a = 10;
	const int *p = &a;	
	a = 15;
    //++（*p）报错
	cout << *p << endl;
	system("pause");
    return 0;
}
~~~

输出结果为15。 