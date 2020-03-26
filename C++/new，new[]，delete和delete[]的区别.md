# new，new[]，delete和delete[]的区别

调用new和new[a]都会开辟空间，不同的是new之后在空间上调用一次构造函数，而new[a]会在开辟的空间上调用a次构造函数。

同理，delete和delete都会释放new/new[]开辟的空间，但是delete只会调用一次析构函数，delete[]会调用多次析构函数。

~~~c++
int* p=new int[10];
delete p;
delete[] p;
~~~

上面两种析构方式都没有问题。但是如果p是指向含有深拷贝的自定义类的数组时，此时delete会造成内存泄漏。