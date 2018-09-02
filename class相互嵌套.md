# class相互嵌套

如果`class`进行相互调用时，如果有`class A`和`class B`，在`class A`中调用`class B`的成员时，应当在`class A`定义前面声明`class B`，并且调用函数应写在`class`外面。

例如：

```c++
class StrBlobPtr;
class StrBlob
{
public:
	friend class StrBlobPtr;
	...
	StrBlobPtr begin( );
	StrBlobPtr end( );
private:
	shared_ptr<vector<string>> data;
};
class StrBlobPtr
{
public:
	friend class StrBlob;
private:
	weak_ptr<vector<string>>wptr;
	size_t curr;
};
StrBlobPtr StrBlob::begin() 
{ 
	return StrBlobPtr(*this); 
}
StrBlobPtr StrBlob::end()
{
	auto ret = StrBlobPtr(*this, data->size());
	return ret;
}
```

其中`begin()`函数和`end()`函数属于`StrBlob`类，但是需要调用`StrBlobPtr`类，因此在`class StrBlob`前面声明`class StrBlobPtr`，并且在`class`外面定义`begin()`函数和`end()`函数。如果在`class StrBlob`中定义函数，编译器刚开始时不会发现错误，在运行时就会报错，提示未声明`class StrBlobPtr`。