# 单一定义规则（ODR）

- `Non-inline`函数和成员函数，以及全局变量和`static`成员变量，在整个程序中只能定义一次。
- `Class`类型（包括`structs`和`unions`）以及`inline`函数，在每个编译单元中最多只能定义一次。如果跨越不同的编译单元，则其定义必须完全相同。

注：**编译单元**是指一个源码文件所涉及的所有文件；也就是说包括`#include`指令所包含的所有文件