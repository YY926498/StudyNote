# vector元素数目修改

如果直接修改vector<>::end()不能达到元素数目修改的目的，可以使用resize()，达到缩小或扩大的目的。

测试：

~~~c++
#include<iostream>
#include<vector>

using namespace std;

int main(void)
{
	vector<int> v1(10, 9);
	v1.end() = v1.begin() + 3;
	for (auto& j : v1)
		cout << j << " ";
	cout << endl;
	cout << v1.size() << endl;

	v1.resize(3);
	for (auto& j : v1)
		cout << j << " ";
	cout << endl;
	cout << v1.size() << endl;
	system("pause");
	return 0;
}
~~~

结果：

~~~c++
9 9 9 9 9 9 9 9 9 9
10
9 9 9
3
~~~

