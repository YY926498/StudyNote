# bind函数

==**bind函数**==是**functional**头文件中的一个标准库函数，可以看做一个通用的函数适配器调用bind函数的基本形式如下：

` auto newCallable = bind(callable,arg_list);`

其中` callable`是一个可调用的对象，` arg_list` 是一个逗号分隔的参数列表，对应给定的`callable`的参数。

- `arg_list `中的参数包括形如`_n`的名字，其中n是一个整数，这些参数是“占位符”，表示*newCallable*的参数，它们占据了传递给*newCallable*的参数的位置。`_n`是定义在`placeholders`的命名空间中

具体调用如下：

```c++
auto g = bind(f,a,b,_2,c,_1);
g(_1,_2);
```

映射为

```c++
f(a,b,_2,c,_1);
```



例如调用` g(x,y)`会调用` f(a,b,y,c,x)`





当进行非拷贝操作时，可使用标准库函数`ref`。

```c++
for_each(words.begin(),words.end(),
        bind(pring,ref(os),_1,' '));
```

函数`ref`返回一个对象，包含给定的引用，此对象是可以拷贝的。并且标准库中有一个`cref`函数，可以生成一个保存const引用的类。这两个函数均在**functional**头文件中

例：

```c++
#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<numeric>
#include<functional>
#include<stdlib.h>

using namespace std;
using namespace std::placeholders;

bool isShort(const string &s1, const string &s2)
{
	return s1.size( ) < s2.size( );
}
bool isSize(const string &s, string::size_type sz)
{
	return s.size( ) > sz;
}

int main(void)
{
	cout << "请输入10串字符串：" << endl;

	vector<string> vs;
	for (int i = 0; i != 10; i++)
	{
		string s;
		getline(cin, s);
		vs.emplace_back(s);
	}
	sort(vs.begin(), vs.end(), bind(isShort, _1, _2));//按照由小到大的顺序排列
	//sort(vs.begin(), vs.end(), bind(isShort, _2, _1));//按照由大到小的顺序排列
	cout << "重排后的string:" << endl;
	for (auto &i:vs)
	{
		cout << i << endl;
	}

	//auto wg = find_if(vs.begin( ), vs.end( ), [](const string &s) { return s.size( ) < 6; });
	auto wg = find_if(vs.begin(), vs.end(), bind(isSize,_1,6));
	cout << vs.end()-wg << endl;

	system("pause");
	return 0;
}
```
