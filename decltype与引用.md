# decltype与引用

- 如果`decltype`使用的表达式不是一个变量，则`decltype`返回表达式结果对应的类型，如果表达式的结果对象能作为一条赋值语句的左值，那么返回一个引用类型

  如下例

  ```c++
  #include<iostream>
  #include<stdlib.h>
  
  using namespace std;
  
  int main(void)
  {
  	int i = 0;
  	int *p = &i;
  	int &r = i;
  	decltype( i )i1;		      //i1为 int
  	decltype( ( i ) )i2=i;	  //i2为 int &
  	decltype( r )i3=i2;		    //i3为 int &
  	decltype( r + 1 )i4;	    //i4为 int
  	decltype( p )i5;		      //i5为 int *
  	decltype( *p )i6=i;		    //i6为 int &
  	system("pause");
  	return 0;
  }
  ```

- `r`是一个==**引用**==，因此`decltype(r)`的结果是一个引用类型。

- `r+0`是将r作为表达式的一部分，这个表达式的结果是一个==**具体值**==，而非一个引用

- 如果表达式的内容是==**解引用操作**==，则`decltype`将得到引用类型。这是因为解引用指针可以得到指针所指的对象，而且可以给这个对象赋值，因此`decltype(*p)`的结果类型是`int &`，而非`int`。

- `decltype`与`auto`的一个重要区别是：`decltype`的结果类型与表达式形式密切相关。对于`decltype`所用的表达式来说，如果变量名加上一对括号，则得到的类型与不加括号时会有所不同。

  切记：==`decltype((varisble))`（注意是双层括号）的结果永远是引用，而`decltype(variable)`结果只有当`ariable`本身就是一个引用时才是引用==

    

    

    

    

  
