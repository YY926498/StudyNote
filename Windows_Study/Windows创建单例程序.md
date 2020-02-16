# Windows创建单例程序

创建单例程序时，可以利用命名内核对象在第一次创建后，再次创建，GetLastError会返回一个“已存在”错误码。因此可以利用这个在保证同一程序只能被打开一次，但是内核对象的名字，可能被其他程序创建，因此有如下步骤：

- 创建边界描述符。因此需要一个字符串标识符来命名一个范围，专有命名空间将在这个范围中定义，这个名称作为以下函数的第一个参数进行传递：

~~~c
HANDLE CreateBoundaryDescriptor(PCTSTR pszName, DWORD dwFlag);
~~~

其中第二个参数暂时没有什么作用，可以传递一个0。返回值是一个指针，指向一个用户模式的结构，结构中包含了边界的定义。因此，不能把返回的句柄值传给CloseHandle，相反，应该传给DeleteBoundaryDescriptor。

- 调用以下函数，讲一个特权用户组的SID与边界描述符关联起来：

~~~c
BOOL AddSIDToBoundaryDescriptor(HANDLE* phBoundaryDescriptor,PSID pRequiredSid);
~~~

- 调用以下函数来创建专有命名空间时，边界描述符伪句柄作为第二个参数传给该函数：

~~~c
HANDLE CreatePrivateNamespace(PSECURITY_ATTRIBUTES psa, PVOID pvBoundaryDescriptor,PCTSTR pszAliasPrefix);
~~~

调用下列函数打开现有的专有空间：

~~~c
HANDLE OpenPrivateNamespace(PVOID pvBoundaryDescriptor,PCTSTR pszAliasPrefix);
~~~

注意，CreatePrivateNamespace和OpenPrivateNamespace返回的HANDLE并不是内核对象句柄，可以调用ClosePrivateNamespace来关闭它们返回的这种伪句柄：

~~~c
BOOLEAN ClosePrivateNamespace(HANDLE hNamespace,DWORD dwFlags);
~~~

如果还有内核对象正在使用，命名空间一定不能关闭。如果在内部还有内核对象时关闭命名空间，其他人就可以在同一个边界中，在重新创建的一个相同的命名空间中，并在其中创建一个同名的内核对象，使Dos攻击再次成为可能。