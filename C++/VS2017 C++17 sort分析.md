# VS2017 C++17 sort分析

源代码：

~~~c++
int a[10];
sort(a,a+10);
~~~

由于没有自定义谓词，根据重载规则进入：

~~~c++
template<class _RanIt> 
inline	void sort(const _RanIt _First, const _RanIt _Last)
{	// order [_First, _Last), using operator<
    _STD sort(_First, _Last, less<>());
}
//其中_STD 为::std::宏替换
~~~

~~~c++
template<class _RanIt,
	class _Pr> inline
	void sort(const _RanIt _First, const _RanIt _Last, _Pr _Pred)
	{	// order [_First, _Last), using _Pred
	_Adl_verify_range(_First, _Last);
	const auto _UFirst = _Get_unwrapped(_First);
	const auto _ULast = _Get_unwrapped(_Last);
	_Sort_unchecked(_UFirst, _ULast, _ULast - _UFirst, _Pass_fn(_Pred));
	}
~~~

上面的`_Adl_verify_range(_First, _Last);`主要验证迭代器`_First`和`_Last`是否合法。

接下来的两次调用`_Get_unwrapped`函数是为了获取迭代器，这个函数有多个重载。

接下来进入`_Sort_unchecked`函数。

~~~c++
template<class _RanIt,
	class _Pr> inline
	void _Sort_unchecked(_RanIt _First, _RanIt _Last, _Iter_diff_t<_RanIt> _Ideal, _Pr _Pred)
	{	// order [_First, _Last), using _Pred
	_Iter_diff_t<_RanIt> _Count;
        //其中_ISORT_MAX==32
	while (_ISORT_MAX < (_Count = _Last - _First) && 0 < _Ideal)
		{	// divide and conquer by quicksort
		auto _Mid = _Partition_by_median_guess_unchecked(_First, _Last, _Pred);
		// TRANSITION, VSO#433486
		_Ideal = (_Ideal >> 1) + (_Ideal >> 2);	// allow 1.5 log2(N) divisions

		if (_Mid.first - _First < _Last - _Mid.second)
			{	// loop on second half
			_Sort_unchecked(_First, _Mid.first, _Ideal, _Pred);
			_First = _Mid.second;
			}
		else
			{	// loop on first half
			_Sort_unchecked(_Mid.second, _Last, _Ideal, _Pred);
			_Last = _Mid.first;
			}
		}

	if (_ISORT_MAX < _Count)
		{	// heap sort if too many divisions
		_Make_heap_unchecked(_First, _Last, _Pred);
		_Sort_heap_unchecked(_First, _Last, _Pred);
		}
	else if (2 <= _Count)
		{
		_Insertion_sort_unchecked(_First, _Last, _Pred);	// small
		}
	}
~~~



首先判断迭代器包裹的长度。如果长度大于32，进入while循环。

其中`_Partition_by_median_guess_unchecked`函数作用是分类。然后划成三部分，继续调用`_Sort_unchecked`函数。

其中快排会根据`_Ideal`，决定是否继续递归。每次都是上一次的3/4。

如果长度大于32，会进行堆排，当小于32时，进行插入排序。