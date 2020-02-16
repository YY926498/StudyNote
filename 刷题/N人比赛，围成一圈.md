# N人比赛，围成一圈

题目描述：

~~~c
xiangxiang有n个人参加编程比赛，比赛结束后每个人都得到一个分数；现在所有人排成一圈（第1个和第n个相邻）领取奖品，要求：
1.如果某人的分数比某边的人高，那么奖品数量也要求比该边的人高
2.每人至少得到一个奖品
问：最少准备多少个奖品？
~~~

输入描述：

~~~c
第一行是整数n，表示测试样例个数
每个测试样例的第一行是一个正整数n，表示参加比赛的人数（0<n<100000）
第二行是n个正整数a[i](0<a[i]<100000)，表示的从第1个人到第n个人的分数
~~~

输出描述

~~~c
对每个测试样例，输出至少应该准备的最少奖品
~~~

示例：

~~~c
4
2
1 2
4
1 2 3 3
8
5 4 3 2 1 4 3 2 
8
5 4 3 2 1 2 3 4    
~~~

输出：

~~~c
3
8
21
24
~~~

采用暴力方法，将所有数据存放在环形链表中，避免首尾不连续。然后找出极小值，加入此时找到两个连续的极小值A、B。其中B为A的后续。计算区间[A,B)所需奖品数量时，借助指针lo和hi，分别指向A的后值和B的前值

- 如果只有一个人，只需要1个奖品
- 如果有两个人，两个成绩相等，只需要2个，不等需要3个

- 两个极小值之间有一个山峰，任何一个指针走到山峰处，就要停下来，等待另一个指针到达山峰
- 如果当前指针指向的值比之前走过的大，那么该指针的最大值加1，否则不变
- 每次计算[A,B)这个区间的奖品数量，这样可以避免重复计算

- 如果A=B：说明此时已经是最后一次计算，计算完后需要退出
- 计算区间[A,B)时，有三种情况，需要分别计算
- - lo先到达山峰，并不动至少一次
  - hi先到达山峰，并不动至少一次
  - 同时到达或有一个先到达，但另一个也在相邻位置
- 每次从极小值出发时，都认为极小值处所需奖品数为1，然后遇到比之前大的，就加1，这样唯一需要考虑的就是山峰处的值
- 计算区间的和
- - 同时到达：取两者的最大值
  - 有一个先到达，另一个在相邻位置：如果左边的值比右边的值大，但是左边的最大值小于右边的最大值 或者右边的值比左边的值大，但是右边的最大值小于左边的最大值，这两者情况下，认为山峰所需的奖品数被低估，需要加上两个指针所处的值的差值，并加1
  - 如果两个都到达山峰，但是有一个指针先到，将整个区间的和减去先到指针的最大值，然后加上该指针的最大值和另一个指针的最大值中的最大值



程序:

~~~c++

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

struct node
{
	node() :pre(nullptr), suf(nullptr), val(0) {}
	node(int t) :pre(nullptr), suf(nullptr), val(t) {}
	node(node* p, node* s, int t) :pre(p), suf(s), val(t) {}
	node* pre;
	node* suf;
	int val;
};
int main()
{
	int n;
	cin >> n;
	while (n-- > 0)
	{
		int num;
		cin >> num;
		vector<node*> src(num);
		for (int i = 0; i < num; ++i)
		{
			int t;
			cin >> t;
			node* p = new node(t);
			src[i] = p;
		}
		for (int i = 0; i < num; ++i)
		{
			if (i == 0)
			{
				src[i]->pre = src[num - 1];
				src[i]->suf = src[i + 1];
			}
			else if (i == num - 1)
			{
				src[i]->pre = src[i-1];
				src[i]->suf = src[0];
			}
			else
			{
				src[i]->pre = src[i - 1];
				src[i]->suf = src[i + 1];
			}
		}
		if (src.size() == 1)
			cout << 1 << endl;
		else if (src.size() == 2)
		{
			if (src[0]->val != src[1]->val)
				cout << 3 << endl;
			else 
				cout << 2 << endl;
		}
		else
		{
			node* qishi = src[0];
			node* cur=qishi->pre;
			node* next=qishi;
			while (qishi != cur)//寻找第一个极小值
			{
				if (qishi->val > qishi->pre->val || qishi->val > qishi->suf->val)
					qishi = qishi->suf;
				else
					break;
			}
			cur = qishi;
			next = qishi->suf;
			int res = 0;
			while (next != qishi)
			{
				while (next != qishi)//寻找下一个极小值
				{
					if (next->val > next->pre->val || next->val > next->suf->val)
						next = next->suf;
					else
						break;
				}
				if (next == cur->suf)//如果两个极小值相邻，直接加1
				{
					++res;
					cur = next;
					next = next->suf;
					continue;
				}
				node* lo = cur->suf;
				node* hi = next->pre;
				int z_max = 1;
				int z_sum = 1;//从左侧极小值出发
				int y_max = 1;
				int y_sum = 0;//从右侧极小值的前续出发
				int zflag = 0;//1：lo指针先到达山峰
				int yflag = 0;//1：hi指针先到达山峰
				while (lo != hi && lo != hi->suf)
				{
					if (zflag == 0 && lo->val > lo->pre->val)
					{
						++z_max;
						z_sum += z_max;
					}
					if (yflag == 0 && hi->val > hi->suf->val)
					{
						++y_max;
						y_sum += y_max;
					}		
                    //如果一个指针到达山峰，另一个相邻，退出
					if (zflag==0 && yflag==0 && lo->suf == hi)
						break;
					if (lo->val <= lo->suf->val)
						lo = lo->suf;
					else
						zflag = 1;
					if (hi->val <= hi->pre->val)
						hi = hi->pre;
					else
						yflag = 1;
				}
				res += z_sum + y_sum;
				if (lo == hi)
				{
                    //同时到达，但是山峰值未加，需要加上
					if (zflag == 0 && yflag == 0)
					{
						if (lo->val > lo->pre->val)
							++z_max;
						if (hi->val > hi->suf->val)
							++y_max;
						res += max(z_max, y_max);
					}
					else//某个指针先到达山峰，需要减去该指针最大值并加上两侧指针的最大值
					{
						if (zflag == 1)
						{
							res = res - z_max + max(z_max, 1 + y_max);
						}
						else
						{
							res = res - y_max + max(1 + z_max, y_max);
						}
					}
				}
				else//一个指针先到达山峰，另一个相邻位置
				{
					if ((lo->val > hi->val && z_max >= y_max)||(lo->val<hi->val && y_max>=z_max))
						res += 1 + abs(z_max - y_max);
				}
				if (next == qishi)
					break;
				else
				{
					cur = next;
					next = next->suf;
				}
				
			}
			cout << res << endl;
		}
		for (int i = 0; i < num; ++i)
			delete src[i];
	}
}
~~~

