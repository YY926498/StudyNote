# N根绳子均分M根，求最大值

有N根绳子，第i根绳子长度为Li，现在需要M根等长的绳子，你可以对n根绳子进行任意裁剪（不能拼接），请你帮忙计算出这m根绳子最长的长度为多少。

输入描述：

~~~c
第一行包含2个正整数N,M，表示N根原始的绳子，和最终需要M根绳子数
第二行包含N个整数，第i个整数Li表示第i根绳子的长度
其中
1<=N,M<=100000
0<=Li<=100000000
~~~

输出描述：

~~~c
对每一个测试用例，输出一个数字，表示裁剪后最长的长度，保留两位小数
~~~

示例：

~~~
输入
3 4
3 5 4
输出
2.50
~~~

分析：采用二分法，先计算最大长度。即将所有绳子的长度相加，然后均分M份，得到最大长度。然后采用二分法，不断逼近实际的最大长度，由于浮点数用于计算二分法需要的时间较长，因此考虑扩大100倍，转换成整数进行计算

~~~c++
#include <iostream>
#include <vector>
using namespace std;
bool junfen(vector<int> const&src, int leng,int dst_num)
{
	int num = 0;
	for (auto i : src)
	{
		num += (i / leng);
	}
	if (num >= dst_num)
		return true;
	else 
		return false;
}
int main()
{
	int N, M;
	cin >> N >> M;
	vector<int> src(N);
	int sum = 0;
	for (auto& i : src)
	{
		cin >> i;
		i *= 100;
		sum += i;
	}
	int hi = sum / M;
	int lo = 0;
	while (hi>=lo)
	{
		if (junfen(src, hi, M) == true)
		{
			cout << 1.0*hi/100 << endl;
			break;
		}
		if (hi == lo + 1)
		{
			cout << 1.0*lo / 100 << endl;
			break;
		}
		int mid = (hi - lo) / 2 + lo;
		if (junfen(src, mid, M) == true)
			lo = mid;
		else
			hi = mid;
	}
	system("pause");
    return 0;
}
~~~

