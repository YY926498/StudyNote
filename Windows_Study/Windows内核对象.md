# Windows内核对象

常见的内核对象有：访问令牌、事件对象、文件对象、文件映射对象、I/O完成端口对象、作业对象、邮件槽对象、互斥量对象、管道对象、进程对象、信号槽对象、线程对象、可等待的计时器对象以及线程池工厂对象等。

 进程间传递内核对象的方法：继承，命名对象、DuplicateHandle。

其中DuplicateHandle可以将目标进程ID设为自身，用于复制一个内核对象的句柄，同时修改其访问权。