# LeetCode176场周赛总结

## 统计有序矩阵中的负数

给你一个 m * n 的矩阵 grid，矩阵中的元素无论是按行还是按列，都以非递增顺序排列。 

请你统计并返回 grid 中 负数 的数目。

~~~c++
示例 1：
	输入：grid = [[4,3,2,-1],[3,2,1,-1],[1,1,-1,-2],[-1,-1,-2,-3]]
    输出：8
    解释：矩阵中共有 8 个负数。
示例 2：
	输入：grid = [[3,2],[1,0]]
    输出：0
示例 3：
	输入：grid = [[1,-1],[-1,-1]]
    输出：3
示例 4：
	输入：grid = [[-1]]
	输出：1
提示：
m == grid.length
n == grid[i].length
1 <= m, n <= 100
-100 <= grid[i][j] <= 100
~~~

这道题很简单，直接遍历二维数组即可。

~~~c++
int countNegatives(vector<vector<int>>& grid) {
	int res = 0;
    for(auto& row : grid)
        for(auto col : row)
            if(col < 0)
                ++res;
    return res;
}
~~~

## 最后K个数的乘积

请你实现一个「数字乘积类」`ProductOfNumbers`，要求支持下述两种方法：

1.` add(int num)`

-   将数字 `num` 添加到当前数字列表的最后面。

2.` getProduct(int k)`

-   返回当前数字列表中，最后 `k` 个数字的乘积。
-   你可以假设当前列表中始终 **至少** 包含 `k` 个数字。

题目数据保证：任何时候，任一连续数字序列的乘积都在 32-bit 整数范围内，不会溢出。

~~~tex
输入：
["ProductOfNumbers","add","add","add","add","add","getProduct","getProduct","getProduct","add","getProduct"]
[[],[3],[0],[2],[5],[4],[2],[3],[4],[8],[2]]

输出：
[null,null,null,null,null,null,20,40,0,null,32]

解释：
ProductOfNumbers productOfNumbers = new ProductOfNumbers();
productOfNumbers.add(3);        // [3]
productOfNumbers.add(0);        // [3,0]
productOfNumbers.add(2);        // [3,0,2]
productOfNumbers.add(5);        // [3,0,2,5]
productOfNumbers.add(4);        // [3,0,2,5,4]
productOfNumbers.getProduct(2); // 返回 20 。最后 2 个数字的乘积是 5 * 4 = 20
productOfNumbers.getProduct(3); // 返回 40 。最后 3 个数字的乘积是 2 * 5 * 4 = 40
productOfNumbers.getProduct(4); // 返回  0 。最后 4 个数字的乘积是 0 * 2 * 5 * 4 = 0
productOfNumbers.add(8);        // [3,0,2,5,4,8]
productOfNumbers.getProduct(2); // 返回 32 。最后 2 个数字的乘积是 4 * 8 = 32 

提示
add 和 getProduct 两种操作加起来总共不会超过 40000 次。
0 <= num <= 100
1 <= k <= 40000
~~~

这道题首先考虑使用前缀和，但是需要注意的是其中有0的存在，如果最后K个数中有0，乘积为0。因此，使用前缀和时，需要对0特殊处理，将0所在的位置换成1，并且不乘以前面的积。另外记录最后一个0出现的位置。

~~~c++
class ProductOfNumbers {
public:
    ProductOfNumbers():seq(),zero(-1) {
        
    }
    
    void add(int num) {
        if(num == 0)
        {
            zero = seq.size();
            seq.emplace_back(1);
            return;
        }
        if(seq.empty())
            seq.emplace_back(num);
        else
            seq.emplace_back(seq.back()*num);
    }
    
    int getProduct(int k) {
        if(((int)seq.size()-k)<=zero)//注意size函数返回的是size_t，必须要强转
            return 0;
        if(seq.size()==k)
            return seq.back();
        return seq.back()/seq[seq.size()-1-k];
    }
    vector<long long> seq;
    int zero;
};
~~~

## 最多可以参加的会议数目

给你一个数组 `events`，其中 `events[i] = [startDayi, endDayi]` ，表示会议 `i` 开始于 `startDayi` ，结束于 `endDayi` 。

你可以在满足 `startDayi <= d <= endDayi` 中的任意一天 `d` 参加会议 `i` 。注意，一天只能参加一个会议。

请你返回你可以参加的 **最大** 会议数目。

~~~tex
示例1
输入：events = [[1,2],[2,3],[3,4]]
输出：3
解释：你可以参加所有的三个会议。
安排会议的一种方案如上图。
第 1 天参加第一个会议。
第 2 天参加第二个会议。
第 3 天参加第三个会议。
示例2
输入：events = [[1,4],[4,4],[2,2],[3,4],[1,1]]
输出：4
提示
1 <= events.length <= 10^5
events[i].length == 2
1 <= events[i][0] <= events[i][1] <= 10^5
~~~

这道题最开始想用动态规划写，但是没推出转移方程。后来经过别人提示，使用贪心或者暴力都能过。首先对会议进行排序，排序的规则是按照结束的越早越优先，如果结束时间相同，则按照开始时间升序排序。这道题卡了太久，等到做出来已经结束了:cry:

~~~c++
int maxEvents(vector<vector<int>>& events) {
	sort(events.begin(), events.end(), [](const vector<int>& lhs, const vector<int>&rhs) {
		if (lhs.back() < rhs.back())
			return true;
		if (lhs.back() > rhs.back())
			return false;
		return lhs.front() <= rhs.front();
	});//在vs里使用谓词需要考虑各种情况，但是在LeetCode里有种情况没定义，居然没有错
	vector<bool> flag(events.back().back() + 1, false);
	int pos = 0;
	int res = 0;
	for (int i = 0; i < events.size(); ++i)
	{
		for (int j = events[i].front(); j <= events[i].back(); ++j)
		{
			if (flag[j] == false)//直接贪心，只要可以参加就直接报名
			{
				++res;
				flag[j] = true;
				break;
			}
		}
	}

	return res;
}
~~~

## 多次求和构造目标数组

给你一个整数数组 `target` 。一开始，你有一个数组 `A` ，它的所有元素均为 1 ，你可以执行以下操作：

-   令 `x` 为你数组里所有元素的和
-   选择满足 `0 <= i < target.size` 的任意下标 `i` ，并让 `A` 数组里下标为 `i` 处的值为 `x` 。
-   你可以重复该过程任意次

如果能从 `A` 开始构造出目标数组 `target` ，请你返回 True ，否则返回 False 。

~~~tex
示例 1：
输入：target = [9,3,5]
输出：true
解释：从 [1, 1, 1] 开始
[1, 1, 1], 和为 3 ，选择下标 1
[1, 3, 1], 和为 5， 选择下标 2
[1, 3, 5], 和为 9， 选择下标 0
[9, 3, 5] 完成
示例 2：
输入：target = [1,1,1,2]
输出：false
解释：不可能从 [1,1,1,1] 出发构造目标数组。
示例 3：
输入：target = [8,5]
输出：true
提示：
N == target.length
1 <= target.length <= 5 * 10^4
1 <= target[i] <= 10^9
~~~

本来想找出规律，但是经过别人提示可以由后往前推。是否能由target推出全1数组。

~~~go
bool isPossible(vector<int>& target) {
	long long sum = 0;//使用long long是因为在leetcode数据集里会有溢出的情况，因此使用该类型
	priority_queue<int, vector<int>,std::less<int>> que;
	for (auto i : target)
	{
		sum += i;
		que.push(i);
	}
	while (que.top() != 1)
	{
		long long top = que.top();
		que.pop();
		long long x = sum - top;
		long long  y = top - x;
		if (y <= 0)
			return false;
		que.push(y);
		sum = top;
	}
	if (que.top() == 1)
		return true;
	return false;
}
~~~

