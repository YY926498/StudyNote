# Go语言标准库

## 输入输出

### io-基本的IO接口

io接口不一定是并行安全的。

在io包中最重要的两个接口：`Reader`和`Writer`接口。

**Reader接口**

~~~go
type Reader interface{
    Read(p []byte)(n int, err error)
}
~~~

