# cin和getline搭配使用

当使用getline时，如果上面使用了cin语句，那么可能输入缓冲区中含有回车或空格或tab，因此，在getline前面需要使用cin.ignore()来清楚缓冲区。

---

```c++
#include<iostream>
#include<string>

using namespace std;

int main(void)
{
	string s1,s2;
	int b;
	while (cin>>b)
	{
		if (b == 0)
			break;
		cout << "str1:";
		cin.ignore();
		getline(cin, s1);
		cout << s << endl;
		cout << "str2:";
		getline(cin, s2);
		cout << s;
		cout << "\n请输入数字:";
	}
	system("pause");
	return 0;
}
```

如果没有cin.ignore()，那么s1=“\n”, s2=“输入的字符”