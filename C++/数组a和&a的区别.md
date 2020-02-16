# 数组`a`和`&a`的区别

~~~c++
#include <iostream>
using namespace std;

int main(void)
{
	int a[5] = { 1,2,3,4,5 };
	intptr_t *ptr = (intptr_t *)(&a + 1);
	printf("%d, %d\n", *(a + 1), *(ptr - 1));
	system("pause");
	return 0;
}
~~~

分析：

![1546245924841](C:\Users\YY\AppData\Roaming\Typora\typora-user-images\1546245924841.png)

- `a`在这里表示的是数组首元素地址即`a[0]`的地址，值为`0x00b3fd8c`

- `&a`代表的是数组首地址，其值为`0x00b3fd8c`

- `a+1`代表的是`0x00b3fd8c+1*sizeof(int)`，等于`0x00b3fd90`

- `&a+1`的值是`（0x00b3fd8c+1*sizeof(a)=0x00b3fda0)`

因此结果是

`2，5`