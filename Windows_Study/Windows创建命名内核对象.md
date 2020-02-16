# Windows创建命名内核对象

程序如下“

~~~c++
#include <Windows.h>
#include <winnt.h>
#include <iostream>
#include <strsafe.h>
int main(void)
{
	HANDLE hMutexProcessA = CreateMutex(NULL, FALSE, TEXT("MUTEX"));
	DWORD errflag = GetLastError();
	std::cout << "flag:" << errflag << std::endl;
	return 0;
}
~~~

第一个进程创建成功，但是第二个进程在创建时，会报错：

​		$err,hr	ERROR_ALREADY_EXISTS : 当文件已存在时，无法创建该文件。 	unsigned int

这是因为第一个进程创建成功，并且内核空间中有了“MUTEX”的互斥量。因此第二个进程再次创建“MUTEX”的互斥量时，就表示文件已存在，说明第二个进程确实能访问。在调用Create*之后，马上调用GetLastError，判断自己刚才是创建了一个新的内核对象，还是仅仅是打开了一个现有的。如果是打开一个现有的内核对象，那么该进程设置的内核对象的属性将会被忽略，内核对象保持原有属性。

但是创建命名内核对象时，同一名字的内核对象的类型必须匹配，否则被拒绝访问。

也可以使用Open*函数