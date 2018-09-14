# class内存大小

-其中nonstatic data members 的总和大小
-任何由于alignment(译注)[^1]的需求而填补（padding）上去的空间（可能存在于members之间，也可能存在于集合体边界）
-加上为了支持virtual而由内部产生的任何额外负担（overhead）

[^1]: alignment就是讲数值调整到某数的倍数。在32位计算机中，通常alignment为4byte(32位)，以使bus的“运输量”达到最高效率

