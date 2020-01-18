# Go语言核心编程

## Ch6 反射

反射是指计算机程序在运行时可以访问、检测和修改本身状态或行为的一种能力。

### 基本概念

首先将实例传递给内部的空接口，实际上是将一个实例类型装换为接口可以表述的数据接口，反射基于这个转换后的数据结构来访问和操作实例的值和类型。

#### 基本数据结构和入口函数

##### `reflect.Type`

反射包里面有一个通用的描述类型公共信息的结构`rtype`，这个`rtype`类似接口内部实现时的runtime包里面的`_type`是同一个东西，只是因为包的隔离性分开定义而已。是描述类型的通用信息，同时为每一种基础类型封装了一个特定的结构。

~~~go
type rtype struct {
	size       uintptr
	ptrdata    uintptr  // number of bytes in the type that can contain pointers
	hash       uint32   // hash of type; avoids computation in hash tables
	tflag      tflag    // extra type information flags
	align      uint8    // alignment of variable with this type
	fieldAlign uint8    // alignment of struct field with this type
	kind       uint8    // enumeration for C
	alg        *typeAlg // algorithm table
	gcdata     *byte    // garbage collection data
	str        nameOff  // string form
	ptrToThis  typeOff  // type for pointer to this type, may be zero
}

// arrayType represents a fixed array type.
type arrayType struct {
	rtype
	elem  *rtype // array element type
	slice *rtype // slice type
	len   uintptr
}

// chanType represents a channel type.
type chanType struct {
	rtype
	elem *rtype  // channel element type
	dir  uintptr // channel direction (ChanDir)
}
...
~~~



### `reflect.TypeOf`

对于`reflect.TypeOf(a)`，传进去的实参a有两种类型，一种是接口变量，另一种是具体类型变量。如果a是具体类型变量，则`reflect.TypeOf()`返回的是具体的类型信息；如果a是一个接口变量，则函数的返回结果又分为两种：如果a绑定了具体实例类型的信息，则返回的是接口的动态类型。如果a没有绑定具体类型实例，则返回的是接口自身的静态类型信息。

注：对于接口的反射，目前只能针对接口类型为`Array`、`Chan`、`Map`、`Ptr`、`Slice`等进行反射操作。

### `reflect.Value`

~~~go
type Value struct {
    typ	 	*rtype
    ptr		unsafe.Pointer
    flag
}
~~~

`reflect.Value`总共有三个字段，一个是值的类型指针typ，另一个是指向值的指针ptr，最后一个是标记字段flag。