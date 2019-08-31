# 求sqrt

第一种方法：二分查找

~~~c++
double sqrt(double x)
{
    double lo=0;
    double hi=x;
    double mid=0;
    double last=0;
    do
    {
        last=mid;
        mid=lo+(hi-lo)/2;
        if(mid*mid>x)
            hi=mid;
        else
            lo=mid;
    }while(abs(last-mid)>eps);
    return mid;
}
~~~

利用二分查找的思想，找到根。

第二种方法：利用牛顿法。

由于要求根，因此构造函数：$$f(x) = {x^2} - a$$。根据牛顿法的思想，假设初始横坐标为$a$ 。因此，初始点为$(a,f(a))$。曲线$f(x)$在该点的导数为$f'(x)=2a$。

由此可以推导出曲线上$(x_{n-1},f(x_{n-1}))$的切线为：

$y=f(x_{n-1})+f'(x_{n-1})(x-x_{n-1})$

令$y=0$。求出该切线与$x$轴的交点，得到$x_n$。

$x_n=(x_{n-1}+a/x_{n-1})/2$

不断迭代。程序如下：

~~~c++
double sqrt(double x)
{
    double last;
    double res=x;
    do
    {
        last=res;
        res=(res+x/res)/2;
    }while(abs(last-res)>eps);
    return res;
}
~~~

第三种方法：

利用magic number。程序如下：

~~~c++
float InvSqrt (float x) 
{
    float xhalf = 0.5f*x;
    int i = *(int*)&x;
    i = 0x5f3759df - (i>>1);
    x = *(float*)&i;
    x = x*(1.5f - xhalf*x*x);
    return x;
}
~~~

上面程序求$1/sqrt(x)$。求出结果后，在将值乘以x，便可以得到$sqrt(x)$。

