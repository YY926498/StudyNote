# 去除重复的字母

给定一个仅包含小写字母的字符串，去除字符串中重复的字母，使得每个字母只出现一次。需保证返回结果的字典序最小（要求不能打乱其他字符的相对位置）。

**示例 1:**

```c++
输入: "bcabc"
输出: "abc"
```

**示例 2:**

```c++
输入: "cbacdcbc"
输出: "acdb"
```

分析：首先遍历一遍字符串，将每个字符的个数记录下来，然后建立一个数组，表示哪些字符已经在结果字符串中，再遍历一遍字符串，每经过一个字符，就将当前字符的个数减1，表示已经经历过。如果当前字符比结果字符串的最后一位小，并且结果字符串的个数大于0，说明后面还有结果字符串的最后一位，因此，将最后一位弹出，循环这个工程，直到找到一个结果字符串的最后一位比当前字符小或者后面后面没有该字符时，停止，将当前字符加到结果字符串最后。

~~~c++
string removeDuplicateLetters(string s) {
        if(s.empty())
            return s;
        vector<bool> cExist(26,false);
        vector<int> cNum(26);
        string result;
        for(auto c:s)
        {
            ++cNum[c-'a'];
        }
        for(auto c:s)
        {
            --cNum[c-'a'];
            if(cExist[c-'a'])
                continue;            
            while(!result.empty() && result.back()>c && cNum[result.back()-'a']!=0)
            {
                cExist[result.back()-'a']=false;
                result.resize(result.size()-1);                
            }
            result+=c;
            cExist[c-'a']=true;
        }
        return result;
    }
~~~

