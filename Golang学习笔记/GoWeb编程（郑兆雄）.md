# Go与Web应用

## HTTP简介

HTTP是一种无状态、由文本构成的请求-响应协议，这种协议使用的是客户端-服务端计算模型。

CGI是一个简单的接口，允许Web服务器与一个独立运行于Web服务器进程之外的进程进行对接。通过CGI与服务器进行对接的程序通常被称之为CGI程序，这种程序可以使用任何编程语言编写。

## HTTP请求

跟所有其他HTTP报文一样，都由一系列文本行组成，这些文本行会按照以下顺序进行排列：

-   请求行
-   零个或任意多个请求首部
-   一个空行
-   可选的报文主体

### 幂等的请求方法

如果一个HTTP方法在使用相同的数据进行第二次调用的时候，不会对服务器的状态造成任何改变，那么这个方法就是幂等的。



# ChitChat

## 请求的接收与处理

工作流程如下：

-   客户端将请求发送到服务器的一个URL上
-   服务器的多路复用器将接收到的请求重定向到正确的处理器，然后由该处理器对请求进行处理
-   处理器处理请求并执行必要的动作
-   处理器调用模板引擎，生成相应的HTML并将其返回给客户端。

### 多路复用器

**最小惊讶原则**：也称最小意外原则，是设计包括软件在内的一切事物的一条通用规则，指的是我们在进行设计的时候，应该做那些合乎常理的事情，使事物的行为总是显而易见、始终如一并且合乎情理。

~~~go
package main

import (
	"fmt"
	"log"
	"net/http"
)

func main() {
	http.HandleFunc("/",root)
	http.HandleFunc("/hello",hello)
	err := http.ListenAndServe(":8080",nil)
	if err!= nil{
		log.Fatal(err)
	}
}

func root(w http.ResponseWriter,req *http.Request)  {
	fmt.Fprint(w,"root")
}
func hello  (w http.ResponseWriter, req *http.Request){
	fmt.Fprint(w,"hello")
}
~~~

如果服务器遇到`/hello/root`，这种链接时，实际上会使用root去处理对`/hello/root`的请求。产生这种行为的原因在于程序在绑定hello时使用的URL是`/hello`而不是`/hello/`。如果被绑定的URL不是以`/`结尾，那么即使请求的URL只有前缀部分与被绑定URL相同，`ServeMux`也会认定这两个URL是匹配的。

因此，如果与hello处理器绑定的URL是`/hello/`而不是`/hello`，那么当浏览器请求`/hello/there`的时候，服务器在找不到与之完全匹配的处理器时，就会退而求其次，开始寻找能够与`/hello/`匹配的处理器，并最终找到`hello`处理器。



# 处理请求

## 请求和响应

### Request结构

Request结构表示一个由客户端发送的HTTP请求报文。主要结构：

-   URL字段
-   Header字段
-   Body字段
-   Form字段、PostForm字段和MultipartForm字段

在使用浏览器发送URL时，会将URL中的片段部分剔除掉-因为服务器接收到的都是不包含片段部分的URL，所以程序无法通过Fragment字段去获取URL的片段部分。但是仍然存在是因为有些请求来自其他的发送工具。

### 请求首部

一个Header类型的实例就是一个映射，这个映射的键为字符串，而键的值是由任意多个字符串组成的切片。

~~~go
h := r.Header
h := r.Header["Accept-Encoding"]
h := r.Header.Get("Accept-Encoding")
~~~

直接引用Header将得到一个字符串切片，而在Header上调用Get方法将返回字符串形式的首部值，其中多个首部值将使用逗号分隔。

### 请求主体

请求和响应的主体都由`Request`结构的`Body`字段表示，这个字段是一个`io.ReadCloser`接口，该接口即包含了`Reader`接口，也包含了`Closer`接口。

## Go与HTML表单

`<form>`标签可以包围文本行、文本框、单选按钮、复选框以及文件上传等多种HTML表单元素，而用户则可以把想要传送给服务器的数据输入到这些元素里面。用户在表单中输入的数据会以键值对的形式记录在请求的主体中，然后以HTTP POST请求的形式发送至服务器。

HTML表单的内容类型决定了POST请求在发送键值对时将使用何种格式，其中HTML表单的内容类型是由表单的`enctype`属性指定的：

~~~html
<form action="/process" method="post" enctype="application/x-www-form-urlencoded">
    <input type="text" name="first_name" />
    <input type="text" name="last_name" />
    <input type="submit" />
</form>
~~~

`enctype`属性的默认值是`application/x-www-form-urlencoded`。浏览器至少需要支持`application/x-www-form-urlencoded`和`multipart/form-data`两种编码方式。另外html5还支持`text/plain`编码方式。

如果表单传送的是简单的文本数据，那么使用URL编码格式更好，因为这种编码更为简单、高效，并且所需的计算量比另一种的更少。但是，如果表单需要传送大量数据（如上传文件）那么使用multipart/form-data编码格式会更好一些。另外还可以通过base64编码，以文本形式传送二进制数据。另外也可以通过GET请求发送表单。因为GET请求不包含请求主体，所以在GET方法传递表单时，表单数据将以键值对的形式包含在请求的URL里面，而不是通过主体传递。

### Form字段

使用Request结构的方法获取表单数据的一般步骤是：

-   调用`ParseForm`方法或者`ParseMultipartForm`方法，对请求进行语法分析
-   根据步骤1调用的方法，访问相应的Form字段、`PostForm`字段或`MultipartForm`字段

### `PostForm`字段

注意如果某个键同时出现在URL和表单中，表单值总是在URL值前面。如果一个键同时有表单键值对和URL键值对，但是只想要获取表单键值对而不是URL键值对，可以访问Request结构的`PostForm`字段，这个字段只会包含键的表单值，而不包含任何同名键的URL值。其中`PostForm`字段只支持`application/x-www-form-urlencoded`编码，如果表单中的字段不是该编码，需要通过`MultipartForm`字段来获取`multipart/form-data`编码。

### `MultipartForm`字段

调用`ParseMultipartForm`方法后，`MultipartForm`字段只包含表单键值对而不包含URL键值对，并且`MultipartForm`字段的值也不再是一个映射，而是一个包含了两个映射的结构，其中第一个映射的键为字符串，值为字符串组成的切片，而第二个映射用来记录用户上传的文件。

另外Request的`FormValue`方法允许直接访问与给定键相关联的值，就像访问`Form`字段中的键值对一样，唯一的区别是`FormValue`方法在需要时会自动调用`ParseForm`方法或者`ParseMultipartForm`方法。

`FormValue`方法及时在给定键拥有多个值的情况下，只会从Form结构中取出给定键的第一个值，所以如果想要获取给定键包含的所有制，那么就需要直接访问Form结构。

除了访问的是`PostForm`字段而不是`Form`字段之外，`PostFormValue`方法的作用跟上面介绍的`FormValue`方法的作用基本相同。`PostFormValue`方法只会返回表单键值对而不返回URL键值对。

`FormValue`返回对应键的值的第一个元素，`PostFormValue`返回对应键的表单的第一个元素。

### 文件

`multipart/form-data`编码通常用于实现文件上传功能，这种功能需要用到file类型的input标签。此时先用`ParseMultipartForm`进行语法分析，接着使用`MultipartForm`的file字段取出文件头，然后调用文件头的Open方法来打开文件。

另外还有`Formfile`方法，可以快速地获取被上传的文件：`FormFile`方法在被调用时将返回给定键的第一个值，因此在客户端只上传了一个文件的情况下，使用起来非常方便。

### 处理带有JSON主体的POST请求

使用`ParseForm`方法是无法从Angular客户端发送的JSON请求中获取JSON数据。这是因为编码形式不同。Go的`ParseForm`方法只会对表单数据进行语法分析，不会接受其他编码。

## `ResponseWriter`

`ServeHTTP`接受`ResponseWriter`接口和一个指向`Request`结构的指针作为参数。接受`Request`结构指针的原因是为了让服务器能够察觉到处理器对`Request`结构的修改，必须传引用而不是传值的方式传递Request结构，另一方面以传值的方式接受`ResponseWriter`，实际上是`ResponseWriter`是response这个非导出结构的接口，而`ResponseWriter`在使用response结构是，传递的也是指向response结构的指针，`ResponseWriter`是以传引用而不是传值的方式在使用response结构。即`ServeHTTP`函数的两个函数传递的都是引用而不是值。

`ResponseWriter`接口的3个方法

~~~go
type ResponseWriter interface {	
	Header() Header	
	Write([]byte) (int, error)
	WriteHeader(statusCode int)
}
~~~

**对`ResponseWriter`进行写入**

Write方法接受一个字节数组作为参数，并将数组中的字节写入HTTP响应的主体中。如果用户在使用Write方法执行写入操作的时候，没有为首部设置响应的内容类型，那么响应的内容类型将通过检测被写入的前512字节决定。

`WriteHeader`方法不能用于设置响应的首部（`Header`方法才是做这件事的）：`WriteHeader`方法接受一个代表HTTP响应状态码的整数作为参数，并将这个整数用作`HTTP`响应的返回状态码；在调用这个方法之后，用户可以继续对`ResponseWriter`进行写入，但是不能对响应的首部做任何写入操作。如果用户在调用`Write`方法之前没有执行`WriteHeader`方法，那么程序默认会使用`200 OK`作为响应的状态码。

因此，`WriteHeader`方法必须在`Write`方法之前调用。

调用Header方法可以取得一个由首部组成的映射，修改这个映射就可以修改首部，修改后的首部将被包含在HTTP响应中，并随着一同发送至客户端。

需要注意：`WriteHeader`方法在执行完毕之后就不允许再对首部进行写入，所以用户必须先写入首部，然后或再写入状态码。

## cookie

cookie是一种存储在客户端的、体积较小的信息，这些信息最初都是由服务器通过HTTP响应报文发送的。每当客户端向服务器发送一个HTTP请求时，cookie都会随着请求被一同发送至服务器。cookie的设计本意是要克服HTTP的无状态性，虽然cookie不是完成这一目标的唯一方法，但是是最常用也最流行的方法之一。

### Go与cookie

~~~go
type Cookie struct{
		Name:       "",
		Value:      "",
		Path:       "",
		Domain:     "",
		Expires:    time.Time{},
		RawExpires: "",
		MaxAge:     0,
		Secure:     false,
		HttpOnly:   false,
		SameSite:   0,
		Raw:        "",
		Unparsed:   nil,
	}
~~~

没有设置`Expires`字段的`cookie`通常被称为会话`cookie`或者临时`cookie`，这种`cookie`在浏览器关闭的时候会自动被移除。相对而言，设置了`Expire`字段的`cookie`通常被称为持久`cookie`，这种`cookie`会一直存在，直到指定的时期来临或被手动删除为止。

`Expire`字段和`MaxAge`字段都可以用于设置`cookie`的过期时间，其中`Expire`字段用于明确地指定`cookie`应该在什么时间过期，而`MaxAge`字段则指明了`cookie`在被浏览器创建出来之后能够存活多少秒。为了让`cookie`在所有浏览器上都能够正常地运作，一个实际的方法是只使用`Expire`，或者同时使用`Expire`和`MaxAge`。

### 将cookie发送至浏览器

`Cookie`结构的`String`方法可以返回一个经过序列化处理的`cookie`，其中`Set-Cookie`响应首部的值就是由这些序列化的cookie组成的。

~~~go
w.Header().Set("Set-Cookie", c2.String())//不能连续使用多次Set，此时会覆盖之前的
w.Header().Add("Set-Cookie", c1.String())
http.SetCookie(w, &c1)//内部使用Add，不会覆盖
~~~

### 从浏览器获取cookie

~~~go
h := r.Header["Cookie"]
r.Cookie("cookiename")
r.Cookies()
~~~

### 使用cookie实现闪现消息

先设置cookie，并且将消息用base64编码存放在value里，然后将之前的cookie的内容取出并解码，再设置同名cookie，设置过期时间为负数，这样就能够立马删除原cookie，然后将内容写到w中。



# 内容展示

## 模板引擎

-   无逻辑模板引擎：将模板中指定的占位符替换成相应的动态数据。这种模板引擎只进行字符串替换，而不执行任何逻辑处理。无逻辑模板引擎的目的是完全分离程序的表现和逻辑，并将所有计算方面的工作都交给处理器完成。
-   嵌入逻辑的模板引擎：将编程语言代码嵌入模板当中，并在模板引擎渲染模板时，由模板引擎执行这些代码并进行相应的字符串替换工作。因为拥有在模板里面嵌入逻辑的能力，所以这类模板引擎能够变得非常强大，但与此同时，这种能力也会导致逻辑分散遍布在不同的处理器之间，使代码变得难以维护。

## Go的模板引擎

流程：处理器首先调用模板引擎，接着以模板文件列表的方式向模板引擎传入一个或多个模板，然后再传入模板需要用到的动态数据；模板引擎在接收到这些参数之后会生成出相应的HTML，并将这些文件写入到`ResponseWriter`里面，然后由`ResponseWriter`将HTTP响应返回给客户端。

用户可以拥有任意多个模板文件，并且这些模板文件可以使用任意后缀名，但他们的类型必须是可读的文本格式。

注意，模板中被两个大括号包围的点`.`是一个动作，它指示模板引擎在执行模板时，使用一个值去替换这个动作本身。

使用Go的Web模板引擎需要两个步骤：

-   对文本格式的模板源进行语法分析，创建一个经过语法分析的模板结构，其中模板源既可以是一个字符串，也可以是模板文件中包含的内容
-   执行经过语法分析的模板，将`ResponseWriter`和模板所需的动态数据传递给模板引擎，被调用的模板引擎会把经过语法分析的模板和传入的数据结合起来，生成出最终的HTML，并将这些HTML传递给`ResponseWriter`。

### 对模板进行语法分析

`ParseFile`是一个独立的函数，可以对模板文件进行语法分析，并创建出一个经过语法分析的模板结构以供`Execute`方法执行。该函数接受的参数数量是可变的，但是，无论接受多少文件名作为输入，都只返回一个模板。

另一种方式是使用`ParseGlob`函数，会对匹配给定模式的所有文件进行语法分析。

~~~go
t,_:=template.ParseGlob("*.html")
~~~

`Must`函数可以包裹一个函数，被包裹的函数会返回一个指向模板的指针和一个错误，如果这个错误不是nil，那么`Must`函数将产生一个panic。

### 执行模板

~~~go
t,_ := template.ParseFiles("t1.html","t2.html")
t.Execute(w,"Hello World!")//此时只执行第一个模板
t.ExecuteTemplate(w,"t2.html","Hello World!")//此时可以指定模板执行
~~~

## 动作

模板中的动作主要有：

-   条件动作
-   迭代动作
-   设置动作
-   包含动作
-   点`.`

其中点`.`是最为重要的一个动作，代表传递给模板的数据，其他动作和函数基本上都会对这个动作进行处理，以此达到格式化和内容展示的目的。

### 条件动作

~~~go
{{if arg}}
	some context
{{end}}

{{if arg}}
	some context
{{else}}
	some context
{{end}}
~~~

`arg`都是传递给条件动作的参数，可以是一个字符串常量、一个变量、一个返回单个值的函数或方法等。

### 迭代动作

迭代动作可以对数组、切片、映射或者通道进行迭代，而在迭代的循环的内部，点`.`则会被设置为当前被迭代的元素

~~~go
{{range array}}
	Dot is set to the element {{.}}
{{end}}
~~~

迭代循环中的`{{.}}`代表的是当前被迭代的切片元素。另一种写法如下：

~~~go
{{range .}}
	some content
{{else}}
	some content
{{end}}
~~~

模板里面介于`{{else}}`和`{{end}}`之间的内容将在点`.`为`nil`时显示。

### 设置动作

设置动作允许用户在指定的范围之内为点`.`设置值。

~~~go
{{with arg}}
	Dot is set to arg
{{end}}
~~~

介于`{{with arg}}`和{{end}}之间的点将被设置为参数`arg`的值。

位于`{{with "world"}}`之前的点会因为处理器传入的值而被设置为原值，而位于`{{with "wirld"}}`和`{{end}}`之间的点则会被设置为`world`；但是，在语句`{{end}}`执行完毕之后，点的值又会被重新设置成原值。

跟迭代动作一样，设置动作也拥有一个能够提供备选方案的变种：

~~~go
{{with arg}}
	Dot is set to arg
{{else}}
	Fallback is arg is empty
{{end}}
~~~

其中`arg`为空字符串、0、false等时，执行else分支。执行新模板不需要对处理器或者服务器进行任何修改，也不需要重启服务器，只要刷新一个浏览器。

### 包含动作

包含动作允许用户在一个模板里面包含另一个模板，从而构建出嵌套模板。包含动作的格式为`{{template "name"}}`，其中`name`参数为被包含模板的名字。

在执行嵌套模板时，必须对涉及的所有模板文件都进行语法分析。

调用`Parse**`时，第一个模板将成为主模板，当用户对模板集合调用`Execute`方法时，主模板将执行。

如果要进行参数传递，可以使用`{{template "name" arg}}`，其中`arg`就是传递给嵌套模板的数据。

## 参数、变量和管道

一个参数就是模板中的一个值。可以是布尔值、整数、字符串等字面量，也可以是结构、结构中的一个字段或者数组中的一个键。还可以是一个变量、一个方法（这个方法必须只返回一个值，或者只返回一个值和一个错误）或者一个函数。最后，参数也可以是一个点`.`，用于表示处理器向模板引擎传递的数据。

除了参数之外，还可以在动作中设置变量。变量以`$`开头

~~~go
{{$key := value}}

{{range $key, $value := .}}
The Key is {{$key}} and the value is {{$value}}
{{end}}
~~~

此例中，点`.`是一个映射，而动作range在迭代这个映射的时候，会将变量`$key`和`$value`分别初始化为当前被迭代映射元素的键和值。

模板中的管道是多个有序串联起来的参数、函数和方法，工作方式的语法跟UNIX的管道相似：

~~~go
{{p1 | p2 | p3}}
~~~

这里p1、p2和p3可以是参数或者函数。管道允许用户将一个参数的输出传递给下一个参数，而各个参数之间用`|`分割。

## 函数

Go模板引擎函数都是受限制的：尽管这些函数可以接受任意多个参数作为输入，但只能返回一个值，或者返回一个值和一个错误。

为了创建一个自定义模板函数，需要：

1.  创建一个名为`FuncMap`的映射，并将映射的键设置为函数的名字，而映射的值则设置为实际定义的函数
2.  将`FuncMap`与模板进行绑定

~~~go
t := template.New("tmpl.html").Funcs(funcMap)
t = template.Must(t.ParseFiles("./模板_函数/tmpl.html"))
~~~

其中`Funcs`方法必须在`ParseFiles`之前被调用。在调用`ParseFiles`函数时，如果用户没有为模板文件中的模板定义名字，那么函数将使用模板文件的名字作为模板的名字。与此同时，在调用New函数创建新模板的时候，用户必须传入一个模板名字，如果用于给定的模板名字跟前面分析模板时通过文件名提取的模板名字不相同，那么程序将返回一个错误。

在调用函数时，除了使用管道，还可以像调用普通函数一样，将`.`作为参数传递给函数。

使用管道比直接调用函数要强大和灵活。可以组合多个函数，相比直接调用更加简洁。

## 上下文感知

~~~go
原文本
{{.}}
<a href="/{{.}}">
<a href="/?q={{.}}">
<a onclick="{{.}}">
~~~

上下文感知特性主要用于实现自动的防御编程。防止某些明显并且低级的编程错误。

### 防御XSS攻击

这种攻击是由于服务器将攻击者存储的数据原原本本地显式给其他用户所致。预防XSS攻击的常见方法是在显式或者存储用户传入的数据之前，对数据进行转义。实际上，如果有需要，用户是可以完全不使用上下文感知特性的。

### 不对HTML进行转义

只要把不想转义的内容传给`template.HTML`函数，模板引擎就不会对其进行转义。

## 嵌套模板

布局：指的是Web设计中可以重复应用在多个页面上的固定模式。

~~~~go
{{define "name"}}
	...
{{end}}
~~~~

在同一个模板文件里面定义多个不同的模板之外，还可以在不同的模板文件里面定义同名的模板。但是对于同一个模板指针，只能有一个名字。

## 通过块动作定义默认模板

~~~go
{{block "name" .}}
 Dot is set to arg
{{end}}
~~~

块动作允许用户定义一个模板并且立即使用。其中`arg`的第一个参数为模板名，如果没有加载对应模板，就使用`{{blok}}{{end}}`中间的模板。

块动作能够高效地定义一个模板。如果模板引擎没有找到哦啊可用的模板，那么就会使用块动作中定义的模板。



# 存储数据

## 内存存储

## 文件存储

### 读取和写入CSV文件

~~~go
reader := csv.NewReader(io.Reader)
reader.FieldsPerRecord = -1//设置正数：读取器在读取时每条记录里面读取出的字段数量必须一致，设置0：将读取的第一条记录的字段数量用作FieldsPerRecord的值，为负数，即使缺少某些字段，读取进程不会被中断。
~~~

### gob包

## Go与SQL



# Go Web服务

## Web服务简介

基于SOAP的Web服务和基于REST的Web服务的一个区别在于：前者是功能驱动的，后者是数据驱动的。SOAP的XML报文非常冗长，对Web服务进行更新，必须修改WSDL，REST本身是一种设计理念，通常回避基于SOAP的Web服务简单。

## 基于SOAP的Web服务简介

SOAP是一种协议，用于交换定义在XML里面的结构化数据，能够跨越不同的网络协议并在不同的编程模型中使用。

## 基于REST的Web服务简介

REST具象状态传输是一种设计理念，用于设计那些通过标准的几个动作来操作资源，并以此来进行相互交流的程序。

PUT和POST的主要区别在于：在使用PUT时需要准确地知道哪一项资源将会被替换，而使用POST只会创建出一项新资源以及一个新URL。换句话说，POST用于创建一项全新的资源，而PUT用于替换一项已经存在的资源。PUT方法是幂等的，同一个调用重复执行多次，给定的URL上面都只会有一项资源被创建出来。相反，POST不是幂等的，每调用一次POST，就会创建一项新资源以及一个新URL。

另外PATCH方法可以用于对一项资源进行部分更新。

~~~http
REST请求示例：
GET /comment.123 HTTP/1.1  
~~~

这个GET请求没有与之相关联的主体。

由于REST只允许用户使用指定的几个HTTP方法操纵资源，不允许用户对资源执行任意动作。但是要绕过这个问题，有两个常用的方法：

-   把过程具体化，或者把动作转换成名词，然后将其用作资源（具体化：将抽象的概念转换为实际的数据模型或对象。动词：操纵资源的动作）
-   将动作用作资源的属性

### 将动作转换为资源

比如激活用户账号的请求：

~~~go
POST /user/456/activation HTTP/1.1
{"date":"2015-05-15T13:05:02Z"}
~~~

这段代码将创建一个被激活的资源，以此来表示用户的激活状态。另一个好处是可以为被激活的资源添加额外的属性。

### 将动作转换为资源的属性

如果用户的激活与否可以通过用户的账号的一个状态确定，那么只需将激活动作用作资源的属性，然后通过HTTP的PATCH方法对该资源进行部分更新即可：

~~~go
PATCH /user/456 HTTP/1.1
{"active":"true"}
~~~

## 通过Go分析和创建XML

### 分析XML

1.  创建一些用于XML数据的结构
2.  使用`xml.Unmarshal`将XML数据解封到结构里面

结构标签是一些跟在字段后面，使用字符串表示的键值对：它的键是一个不能包含空格、引号`“`或者冒号`:`的字符串，而值则是一个被双引号`“`包围的字符串。在处理XML时，结构标签的键总是为xml。

出于创建映射的需要，xml包要求被映射的结构以及结构包含的所有字段都必须是公开的。即名称必须以答谢的英文字母开头。

XML结构标签的使用规则：

1.  通过创建一个名字为`XMLName`、类型为`xml.Name`的字段，可以将xml元素的名字存储在这个字段里面（在一般情况下，结构的名字就是元素的名字）。
2.  通过创建一个与XML元素属性同名的字段，并使用`xml:"<name>,attr"`作为该字段的结构标签，可以将元素的`<name>`属性的值存储到这个字段里面。
3.  通过创建一个与XML元素标签同名的字段，并使用`xml:",chardata"`作为该字段的结构标签，可以将XML元素的字符数据存储到这个字段里面。
4.  通过定义一个任意名字的字段，并使用`xml:",innerxml"`作为该字段的结构标签，可以将XML元素的原始XML存储到这个字段里面。
5.  没有模式标志（如`,attr`、`,chardata`或者`,innerxml`）的结构字段将与同名的XML元素匹配。
6.  使用`xml:"a>b>c"`这样的结构标签可以在不指定树状结构的情况下直接获取指定的XML元素，其中a和b作为中间元素，而c则是想要获取的节点元素。

如果需要从一个很大的XML的表单中提取部分数据，首先创建一个`Decoder`，通过调用`NewDecoder`并向其传递一个`io.Reader`来完成。然后使用Token方法来获取XML流中的下一个`token`：此时，`token`实际上是一个表示XML元素的接口。然后使用for循环不断取出token。如果所有token都被取出，Token方法将返回一个表示文件数据或数据流已经被读取完毕的`io.EOF`结构作为结果，并将返回值中的err变量设为nil。

对token进行检查，判断是否为`StartElement`，即判断该token是否为XML元素的起始标签。如果程序面对的是流式XML数据或者体积非常庞大的XML文件，解码是提取数据的可行方法。

### 创建XML

创建结构并向里面填充数据

创建用于存储数据的XML文件

根据给定的XML文件，创建出相应的编码器

把结构编码至文件

## 通过Go分析和创建JSON

### 分析JSON

与分析xml类似。

1.  创建一些用于包含JSON数据的结构

2.  通过`json.Unmarshal`函数，把JSON数据解封到结构里面。

## 创建Go Web服务



# 应用测试

## Go与测试

主要搭配`testing`库和`httptest`库使用。

## 使用Go进行单元测试

单元测试是一种为验证单元的正确性而设置的自动化测试，一个单元就是程序中的一个模块化部分。程序中的一个部分能否独立地进行测试，是评判这个部分能否被归纳为“单元”的一个重要指标。

-   测试文件与被测试的源码文件位于同一个包内

-   调用被测试的函数

-   检查结果是否和预期的一样，如果不一样就显示出一条出错信息

-   暂时跳过对编码函数的测试

测试时，函数应该以`Testxxx`作为名字，并且接受一个指向`testing.T`结构的指针作为参数，如果被测试函数的输出结果未如预期时，用户可以使用这个结构来产生相应的failing以及error。

`testing.T`结构拥有几个常用的函数：

-   `Log`– 将给定的文本记录到错误日志里面，与`fmt.Println`类似
-   `Logf` – 根据给定的格式，将给定的文本记录到错误日志里面，与`fmt.Printf`类似
-   `Fail` – 将测试函数标记为“已失败”，但允许测试函数继续执行
-   `FailNow` – 将测试函数标记为“已失败”并停止执行测试函数

除以上4个函数外，`testing.T`还提供一些便利函数，由上面4个函数组合而成：

|           |  `Log`  |  `Logf`  |
| :-------: | :-----: | :------: |
|  `Fail`   | `Error` | `Errorf` |
| `FailNow` | `Fatal` | `Fatalf` |



### 跳过测试用例

在进行测试驱动开发(TDD)的时候，通常会让测试用例持续地失败，知道函数被真正地实现为止；但是，为了避免测试用例在函数尚未实现之前一直打印烦人的失败信息，用户可以使用`testing.T`结构提供的Skip函数，暂时跳过指定的测试用例。此外，如果某个测试用例的执行时间非常长，也可以在实施完整性检查的时候，使用`Skip`函数跳过该测试用例。

除了可以直接跳过整个测试用例，还可以通过`go test`命令传入短暂标志`-short`，并在测试用例中使用某些条件逻辑来跳过测试中的指定部分。注意，这种做法跟在`go test`命令中通过选项来选择性地执行指定的测试不一样：选择性执行只会执行指定的测试，并跳过其他的所有测试，而`-short`标志则会根据用户编写测试代码的方式，跳过测试中的指定部分或者跳过整个测试用例。

~~~go
func TestLongRunningTest(t *testing.T){
    if testing.Short() {
        t.Skip("Skipping long-running test in short mode")
    }
    time.Sleep(10*time.Second)
}
~~~

### 以并行方式运行测试

`go test -parallel n`该命令是同时并行n个测试用例。在测试函数中添加`t.Parallel()`函数。