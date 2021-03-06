# using声明和using 指示

- 一条using声明语句一次只引入命名空间的一个成员，可以出现在全局作用域，局部作用域，命名空间作用域以及类的作用域。*在类的作用域中，这样的声明语句只能指向基类成员*
- using指示：可以出现在全局作用域，局部作用域，命名空间作用域中，**但是不能出现在类的作用域中**。==using指示具有将命名空间成员提升到包含命名空间本身和using指示的最近作用域的能力。using指示一般被看做是出现在最近的外层作用域中==
- using声明语句声明的是一个名字，而非一个特定的函数。例如：

~~~c++
using NS::print(int);	//错误，不能指定形参列表
using NS::print;		//正确
~~~

- 一个using声明引入的函数将重载声明语句所属作用域中已有的其他同名函数。**如果一个using声明所在的作用域中出现一个函数与新引入的函数同名并且形参列表相同，则该using声明将报错**
- using指示：引入一个与已有函数形参列表完全相同的函数并不会产生错误。只要调用的时候指明调用的是命名空间中的函数版本还是当前作用域中的版本即可，否则会产生==二义性==。