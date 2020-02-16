# vector转string

利用copy函数，即可以完成转换

~~~c++
vector<double> vd{ 1.1,2.3,5.6 };
	vd.resize(10);
	copy_backward(vd.begin(), vd.begin() + 3, vd.end());
	
	copy(vd.begin(), vd.end(), ostream_iterator<double>(cout,"\n"));
	
	copy_if(vd.begin(), vd.end(), ostream_iterator<double>(cout, "\n"), [](int a) {return a != 0; });

~~~

结果：

~~~c++
1.1
2.3
5.6
0
0
0
0
1.1
2.3
5.6
1.1
2.3
5.6
1.1
2.3
5.6
请按任意键继续. . .
~~~

