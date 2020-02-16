# Excel表列名称

给定一个正整数，返回它在 Excel 表中相对应的列名称。

~~~c++
	1 -> A
    2 -> B
    3 -> C
    ...
    26 -> Z
    27 -> AA
    28 -> AB 
    ...
~~~

示例：

~~~c++
输入: 701
输出: "ZY"
~~~

这道题虽然很简单，就是10进制转26进制，但是需要注意的是数是从‘A’开始的，当数小于27的时候，1就是‘A’，如果大于27后，有可能0也是‘A’。

如果这么想就有可能写出下列程序：

~~~c++
string convertToTitle(int n) 
{
    string res;
    if(n<27)
    {
        char c='A'-1+n%27;
        res+=c;
        return res;
    }
    else
    {
        while(n)
        {
            char c='A'+n%27;
            res=c+res;
            n/=26;
        }
    }
    return res;
}
~~~

上列程序是及其错误的，没有认识到这道题的陷阱。这道题就是10进制转26进制，但是注意，在实际的26进制中是0~25,然后再进位。如果用示例来推导结论，就会发现‘A’可能对应多个值，而且‘Z’是26.因此，可以考虑每次算之前，将其减一，这样A-Z就是0-25。所以有下列程序：

~~~c++
string convertToTitle(int n) {
    string res;
    while(n>0)
    {
        --n;
        char c='A'+n%26;
        res=c+res;
        n/=26;
    }
    return res;
}
~~~

这道题有点绕，需要好好想想。