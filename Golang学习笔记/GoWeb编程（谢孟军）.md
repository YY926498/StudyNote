# Web基础

Web服务器的工作原理简单归纳为：

-   客户机通过TCP/IP协议建立到服务器的TCP连接
-   客户端向服务端发送HTTP协议请求包，请求服务器里的资源文档
-   服务器向客户机发送HTTP协议应答包，如果请求的资源包含有动态语言的内容，那么服务器会调用动态语言的解释引擎负责处理“动态内容”，并将处理得到的数据返回给客户端
-   客户机与服务器断开。由客户端解释HTML文档，在客户端屏幕上渲染图形结果

**DNS解析过程**

1.  在浏览器中输入www.qq.com域名，操作系统会先检查自己本地的hosts文件是否有这个网址的映射关系，如果有，就先调用这个IP地址映射，完成域名解析
2.  如果hosts里没有这个域名的映射，则查找本地DNS解析器缓存，是否有这个网址映射关系，如果有，直接返回，完成域名解析
3.  如果hosts与本地DNS解析器缓存都没有相应的网址映射关系，首先会找TCP/IP参数中设置的首选DNS服务器。在此称呼为本地DNS服务器，此服务器收到查询时，如果要查询的域名，包含在本地配置区域资源中，则返回解析结果给客户机，完成域名解析，此解析具有权威性
4.  如果要查询的域名，不由本地DNS服务器区域解析，但该服务器已缓存了此网址映射关系，则调用这个IP地址映射，完成域名解析，此解析不具有权威性
5.  如果本地DNS服务器本地区域文件与缓存解析都失效，则根据本地DNS服务器的设置进行查询，如果未用转发模式，本地DNS就把请求发至“根DNS服务器”，“根DNS服务器”收到请求后会判断这个域名`.com`是谁来授权管理，并会返回一个负责该顶级域名服务器的一个IP。本地DNS服务器收到IP信息后，就会联系负责`.com`域的这台服务器。这他`.com`域的服务器收到请求后，如果自己无法解析，它就会找一个管理`.com`域的下一级DNS服务器地址`qq.com`给本地DNS服务器。当本地服务器收到这个地址后，就会找`qq.com`域服务器，重复上面的动作，进行查询，直至找到`www.qq.com`主机
6.  如果用的是转发模式，此DNS服务器就会把请求转发至上一级DNS服务器，由上一级服务器进行解析，或找根DNS或把转请求转至上上级，以此循环。不管是本地DNS服务器用的是转发还是根提示，最后都是把结果返回给本地DNS服务器，由此DNS服务器再返回给客户机

**递归查询过程**：是查询的递交者，而迭代查询过程是查询的递交者不变

浏览器最后发起的请求是基于IP来和服务器做信息交互的。

**HTTP协议详解**

HTTP协议是无状态的和Connection: keep-alive的区别

无状态是指协议对于事务处理没有记忆能力，服务器不知道客户端是什么状态。从另一方面来说，打开一个服务器上的网页和之前打开的这个服务器上的网页之间没有区别。

HTTP是一个无状态的面向连接的协议，无状态不代表HTTP不能保持TCP连接，更不能代表HTTP使用的是UDP协议。

## Go的Web工作

web工作的几个概念：

**Request**：用户请求的信息，用来解析用户的请求信息，包括post、get、cookie和url等信息

**Response**：服务器需要反馈给客户端的信息

**Conn**：用户的每次请求链接

**Hnadler**：处理请求和生成返回信息的处理逻辑

http包执行流程：

1.  创建Listen Socket，监听指定的端口，等待客户端请求到来
2.  Listen Socket接受客户端的请求，得到Client Socket，接下来通过Client Socket与客户端通信
3.  处理客户端的请求，首先从Client Socket读取HTTP请求的协议头，如果是POST方法，还可能要读取客户端提交的数据，然后交给相应的handler处理请求，handler处理完毕以后准备好客户端需要的数据，通过Client Socket写给客户端

## Go的http包详解

核心功能：Conn和ServeMux

~~~go
type ServeMux struct{
    mu 	sync.RWMutex		//锁，由于请求涉及到并发处理，因此这里需要一个锁机制
    m	map[string]muxEntry	//路由规则，一个string对应一个mux实体
}
type muxEntry struct {
	h       Handler			//路由表达式对应的handler
	pattern string			//模式匹配
}
type Handler interface {
	ServeHTTP(ResponseWriter, *Request)
}
~~~

如果函数没有定义ServeHTTP这个接口，就会依赖HandlerFunc：

~~~go
type HandlerFunc func(ResponseWriter, *Request)
func (f HandlerFunc) ServeHTTP(w ResponseWriter, r *Request){
    f(w,r)
}
~~~

路由器里面会存储好相应的路由规则，在接收到请求之后调用mux.handler(r).ServeHTTP(w,r)。其中mux.handler(r)是根据用户请求的URL和路由器里面存储的map去匹配，当匹配到之后返回存储的handler，调用这个handler的ServeHTTP接口就可以执行相应的函数。



# 表单

表单以包含表单元素的区域。表单元素是允许用户在表单中（比如：文本域、下拉列表、单选框、复选框等）输入信息的元素。表单使用表单标签`<form>`定义。

~~~html
<form>
    ...
    input 元素
    ...
</form>
~~~

表单的一个很大的功能是能够上传文件。

## 处理表单的输入

开发Web的一个原则是：不能信任用户输入的任何信息，所以验证和过滤用户的输入信息就非常重要。

**必填字段**

想要确保从一个表单元素中得到一个值，可以通过len函数获取字符串的长度。`r.Form`对不同类型的表单元素的留空有不同的处理，对于空文本框、空文本区域以及文件上传。元素的值为空值，而如果是未选中的复选框和单选按钮，则根本不会在`r.Form`中产生相应的条目，因此需要通过`r.Form.Get()`来获取值，因为如果字段不存在，通过该方式获取的是空值。但是通过`r.Form.Get()`只能获取单个值，如果是map的值，必须通过上面的方式来获取。

**数字**

如果要确保一个表单输入框中获取的只是数字：

~~~go
getint,err := strconv.Atoi(r.Form.Get("age"))
if err!= nil{
    
}
//还有一种是正则表达式：
if m,_ := regexp.MatchString(`^[0-9]+$`,r.Form.Get("age"));!m{
    
}
~~~

**中文**

如果想通过表单元素获取一个用户的中文名字，但是又为了保证获取的是正确的中文，需要进行验证，而不是用户随便的一些输入。对于中文，目前只能使用正则方式来验证：

~~~go
if m,_:=regexp.MatchString(`^[\\x{4e00}-\\x{9fa5}]+$`,r.Form.Get("realname)); !m{
    
}
~~~

**英文**

如果期望通过表单元素来获取一个英文值：

~~~go
if m,_=regexp.MatchString(`^[a-zA-Z]+$`,r.Form.Get("engname"));!m{
    
}
~~~

**电子邮件地址**

~~~go
if m,_ := regexp.MatchString(`^([\w\.\_]{2,10})@(\w{1,}).([a-z]{2,4})$`,r.Form.Get('email'));!m{
    
}
~~~

**手机号码**

~~~go
if m,_:=regexp.MatchString(`^(1[3|4|5|8][0-9]\d{4,8})$`,r.Form.Get("Mobile"));!m{
    
}
~~~

**下拉菜单**

如果要判断表单里面`<select>`元素生成的下拉菜单中是否有被选中的项目：

~~~html
<select name = "fruit">
    <option value = "apple">apple</option>
    <option value = "pear">pear</option>
    <option value = "banane">banane</option>
</select>
~~~

那么就可以这样来验证：

~~~go
slice := []string{"apple","pear","banane"}
for _,v:= range slice{
    if v==r.Form.Get("fruit"){
        return true
    }
}
return false
~~~

**单选按钮**

如果要判断一个radio按钮是否被选中：

~~~html
<input type = "radio" name = "gender" value = "1">男
<input type = "radio" name = "gender" value = "2">女
~~~

那么可以通过类似下拉菜单的做法：

~~~go
slice := []int{1,2}
for _,v:=range slice{
    if v==r.Form.Get("gender"){
        return true
    }
}
return false
~~~

**复选框**

有一项选择兴趣的复选框，想确定用户选中的和提供的用户选择是同一个类型的数据：

~~~go
<input type = "checkbox" name = "interest" value = "football">足球
<input type = "checkbox" name = "interest" value = "basketball">篮球
<input type = "checkbox" name = "interest" value = "tennis">网球
~~~

对于复选框的验证和单选有点不一样，因为接收到的数据是一个slice：

~~~go
slice := []string{"football","basketball","tennis"}
a:=Slice_diff(r.Form["interest"],slice)
if a==nil{
    return true
}
return false
~~~

**日期和时间**

如果要确定用户填的日期和时间是否有效，Go里面提供了一个time的处理包，可以把用户输入年月日转换为相应的时间，然后进行逻辑判断：

~~~go
t := time.Date(2009,time.November,10,23,0,0,0,time.UTC)
fmt.Printf("Go launched at %s\n",t.Local)
~~~

获取time之后就可以进行很多时间函数的操作。

**身份证号码**

如果要验证输入是否是身份证，可以用正则进行方便的验证，但是有15位和18位，都需要验证：

~~~go
//验证15位身份证，15位的全是数字
if m,_ := regexp.MatchString(`^(\d{15})$`,r.Form.Get("usercard"));!m{
    return false
}
return true
//验证18位身份证，前17位是数字，最后一位可能是数字或者字符X
if m,_:=regexp.MatchString(`^(\d{17})([0-9]|X)$`,r.Form.Get("usercard"));!m{
    return false
}
return true
~~~

## 预防跨站脚本

动态站点会受到一种名为“跨站脚本攻击”`XSS`的威胁，而静态站点完全不受影响。对XSS的防护可以结合一下两种方法：

1.  验证所有输入数据，有效监测攻击
2.  对所有输出数据进行适当的处理，防止任何已成功注入的脚本在浏览器端运行

Go里可以借助`html/template`里面的几个函数帮助转义：

~~~go
func HTMLEscape(w io.Writer, b []byte)	//把b转义之后写到w
func HTMLEscapeString(s string)string	//转义s之后返回结果字符串
func HTMLEscaper(args ...interface{})string	//支持多个参数一起转义，返回结果字符串
~~~

Go里的`html/template`包默认过滤html标签，但有时候想输出`<script>alert()</script>`这种看起来正常的信息，可以使用`text/template`包。

~~~go
import (
	"log"
	"os"
	"text/template"
)

func main() {
	t, err := template.New("foo").Parse(`{{define "T"}}Hello,{{.}}!{{end}}`)
	if err != nil {
		log.Fatal(err)
	}
	err = t.ExecuteTemplate(os.Stdout, "T", "<script>alert('you have been pwned')</script>")
}
//输出
Hello,<script>alert('you have been pwned')</script>!
~~~

也可以使用`html/template`：

~~~go
package main

import (
	"html/template"
	"log"
	"os"
)

func main() {
	t, err := template.New("foo").Parse(`{{define "T"}}Hello,{{.}}!{{end}}`)
	if err != nil {
		log.Fatal(err)
	}
	err = t.ExecuteTemplate(os.Stdout, "T", template.HTML("<script>alert('you have been pwned')</script>"))
}
//输出
Hello,<script>alert('you have been pwned')</script>!
~~~

如果不转义就会成下列：

~~~go
import (
	"html/template"
	"log"
	"os"
)

func main() {
	t, err := template.New("foo").Parse(`{{define "T"}}Hello,{{.}}!{{end}}`)
	if err != nil {
		log.Fatal(err)
	}
	err = t.ExecuteTemplate(os.Stdout, "T", "<script>alert('you have been pwned')</script>")
}
//输出
Hello,&lt;script&gt;alert(&#39;you have been pwned&#39;)&lt;/script&gt;!
~~~

## 防止多次递交表单

解决方案是在表单中添加一个带有唯一值的隐藏字段。在验证表单时，先检查带有该唯一值的表单是否已经递交过了，如果是，拒绝再次递交；如果不是，则处理表单进行逻辑处理。另外，如果采用了AJAX模式递交表单的话，当表单递交后，通过javascript来禁用表单的递交按钮。

## 处理文件上传

要使表单能够上传文件，首先第一步要添加form的enctype属性，enctype属性有如下三种情况：

~~~html
application/x-www-form-urlencoded	//表示在发送前编码所有字符，默认
multipart/form-data	//不对字符编码。在使用包含文件上传控件的表单时，必须使用该值
text/plain			//空格转换为"+"号，但不对特殊字符编码
~~~

处理文件上传需要调用`r.ParseMultipartForm`,里面的参数表示maxMemory，调用ParseMultipartForm之后，上传的文件存储在maxMemory大小的内存里面，如果文件大小超过了maxMemory，那么剩下的部分将存储在系统的临时文件中。可以通过`r.FormFile`获取上面的文件句柄，然后实例中使用io.Copy来存储文件。

获取其他非文件字段信息的时候不需要调用`r.ParseForm`，因为在需要的时候Go自动回去调用。而且`ParseMultipartForm`调用一次之后，后面再次调用不会再有效果。

上传文件的主要步骤：

1.  表单中增加`enctype = “multipart/form-data”`
2.  服务端调用`r.ParseMultipartForm`，把上传的文件存储在内存和临时文件中
3.  使用`r.FormFile`获取文件句柄，然后对文件进行存储等处理。

**客户端上传文件**

~~~go
func postFile(filename string, targetUrl string) error {
	bodyBuf := &bytes.Buffer{}
	bodyWriter := multipart.NewWriter(bodyBuf)
	//很关键的一个步骤
	fileWriter, err := bodyWriter.CreateFormFile("uploadfile", filename)
	if err != nil {
		fmt.Println("error writing to buffer:", err)
		return err
	}

	//打开文件句柄操作
	fh, err := os.OpenFile(filename, os.O_RDONLY, 0666)
	if err != nil {
		fmt.Println("error opening file:", err)
		return err
	}

	//iocopy
	_, err = io.Copy(fileWriter, fh)
	if err != nil {
		fmt.Println("error copying file:", err)
		return err
	}

	contentType := bodyWriter.FormDataContentType()
	bodyWriter.Close()
	resp, err := http.Post(targetUrl, contentType, bodyBuf)
	if err != nil {
		fmt.Println("post error:", err.Error())
		return err
	}
	defer resp.Body.Close()

	resp_body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		fmt.Println("ioutil error:", err.Error())
		return err
	}
	fmt.Println(resp.Status)
	fmt.Println(string(resp_body))
	return nil
}
~~~

上面的例子展示了客户端如何向服务器上传一个文件的例子，客户端通过multipart.Writer把文件的文本流写入一个缓存中，然后调用`http.Post`方法把缓存传到服务器。

如果还有其他普通的字段例如username之类的需要同时写入，那么可以调用multipart的WriteField方法写很多其他类似的字段。



# 访问数据库

Go没有内置的驱动支持任何数据库，但是Go定义了database/sql接口，用户可以基于驱动接口开发相应数据库的驱动。

## database/sql接口

**sql.Register**

这个存在于database/sql的函数是用来注册数据库驱动的，当第三方开发者开发数据库驱动时，都会实现init函数，在init里面会调用这个`Register(name string, driver driver.Driver)`来完成本驱动的注册。

第三方数据库驱动都是通过调用这个函数来注册自己的数据库驱动名称以及相应的driver实现。在database/sql内部通过一个map来存储用户定义的相应驱动。因此通过database/sql的注册函数可以同时注册多个数据库驱动，只要不重复。

**driver.Driver**

Driver是数据库驱动的接口。定义了一个method:`Open(name string)`，这个方法返回一个数据库的Conn接口。

~~~go
type Driver interface{
    Open(name string) (Conn, err)
}
~~~

返回的Conn只能用来进行一次goroutine的操作，也就是说不能把这个Conn应用于读个goroutine里面。因为这样会导致数据紊乱。

**driver.Conn**

Conn是一个数据库连接的接口定义，定义了一系列方法，这个Conn只能应用在一个goroutine里面，不能使用多个goroutine里面。

~~~go
type Conn interface{
    Prepare(query string)(Stmt, error)
    Close() error
    Begin()(Tx, error)
}
~~~

`Prepare`函数返回与当前连接相关的执行sql语句的准备状态，可以进行查询、删除等操作。

`Close`函数关闭当前的连接，执行释放连接拥有的资源等清理工作。因为驱动实现了database/sql里面建议的conn pool，所以不用再去实现缓存conn之类的，容易引起问题。

`Begin`函数返回一个代表事务处理的`Tx`，通过它可以进行查询、更新等操作，或者对事务进行回滚和递交等。

**driver.Stmt**

Stmt是一种准备好的状态，和Conn相关联，而且只能应用于一个goroutine中，不嫩个应用于多个goroutine。

~~~go
type Stmt interface{
    Close() error
    NumInput() int
    Exec(args []Value)(Result, error)
    Query(args []Value)(Rows, error)
}
~~~

`Close`函数关闭当前的连接状态，但是如果当前正在执行query，query还是有效返回row数据。

`NumInput`函数返回当前预留参数的个数，当返回>=0时数据库驱动就会智能检查调用者的参数。当数据库驱动包不知道预留参数的时候，返回-1.

`Exec`函数执行Prepare准备好的sql，传入参数执行update/insert等操作，返回Result数据

`Query`函数执行Prepare准备好的sql，传入需要的参数执行Select操作，返回Row结果集。

**driver.Tx**

事务处理一般就两个过程，递交或回滚。数据库驱动里面也只需要实现这两个函数就可以：

~~~go
type Tx interface{
    Commit() error
    Rollback() error
}
~~~

这两个函数一个用来递交，一个用来回滚事务

**driver.Execer**

这是一个Conn可选择实现的接口

~~~go
type Execer interface{
    Exec(query string, args []Value)(Result,error)
}
~~~

如果这个接口没有定义，那么在调用DB.Exec，就会先调用Prepare返回Stmt，然后执行Stmt的Exec，然后关闭Stmt。

**driver.Result**

这个是执行update/insert等操作返回的结果接口定义

~~~go
type Result interface{
    LastInsertId()(int 64,error)
    RowsAffected()(int64, error)
}
~~~

`LastInsertId`函数返回由数据库执行插入操作得到的自增ID号

`RowsAffected`函数返回query操作影响的数据条目数

**driver.Rows**

Rows是执行查询返回的结果集接口定义

~~~go
type Rows interface{
    Columns() []string
    Close()	error
    Next(dest []Value) error
}
~~~

`Columns`函数返回查询数据库表的字段信息，这个返回的slice和sql查询的字段是一一对应，而不是返回整个表的所有字段

`Close`函数用来关闭Rows迭代器

`Next`函数用来返回下一条数据，把数据赋值给dest。dest里面的元素必须是driver.Value的值，除了string，返回的数据里面所有的string都必须要转换成[]byte。如果最后没有数据，Next函数最后返回io.EOF

**driver.RowsAffected**

其实就是一个int64的别名，但是实现了Result接口，用来底层实现Result的表示方式

~~~go 
type RowsAffected int64
func (RowsAffected) LastInsertId()(int64, error)
func (v RowsAffected) RowsAffected() (int64, error)
~~~

**driver.ValueConverter**

该接口定义；额如何把一个普通的值转换成driver.Value接口

~~~go
type ValueConverter interface{
    ConvertValue(v interface{}) (Value, error)
}
~~~

这个接口的好处：

-   转化driver.Value到数据库表相应的字段，例如int64的数据转化成数据库表uint16字段
-   把数据库查询结果转化成driver.Value的值
-   在scan函数里面如何把driver.Value值转化成用户定义的值

**driver.Valuer**

该接口定义了返回一个driver.Value的方式

~~~go
type Valuer interface{
    Value()(Value, error)
}
~~~

很多类型都实现了这个Value方法，用来自身与driver.Value的转化

**database/sql**

内部建议性地实现了一个conn pool

~~~go
type DB struct{
    driver 		driver.Driver
    dsn			string
    mu			sync.Mutex
    freeConn	[]driver.Conn
    closed		bool
}
~~~



# session和数据存储

web开发中一个很重要的议题就是如何做好用户的整个浏览过程的控制，因为HTTP是无协议的，所以用户的每一次请求都是无状态的，不知道在整个web操作过程中哪些连接与该用户有关。经典的解决方案是cookie个session，cookie机制是一种客户端机制，把用户数据保存在客户端，而session机制是一种服务端的机制，服务器使用一种类似于散列表的结构来保存信息，每一个网站访客都会被分配给一个唯一的标志符，即sessionID，它的存放形式无非两种：要么经过url传递，要么保存在客户端的cookie里，当然也可以把sessionID保存到数据库里，这样会更安全，但效率方面会有所下降。

## session和cookie

cookie简而言之是在本地计算机保存一些用户操作的历史信息（当然包括登录信息），并在用户再次访问该站点时浏览器通过HTTP协议将本地cookie内容发送给服务器，从而完成验证，或继续上一步操作。

session简而言之是在服务器上保存用户操作的历史信息。服务器使用sessionID来标识session，sessionID由服务器负责产生，保证随机性和唯一性，相当于一个随机密钥，避免在我手或传输中暴露用户真实密码。但该方式下，仍然需要将发送请求的客户端与session进行对应，所以可以借助cookie机制来获取客户端的标识（即sessionid），也可以通过GET方式将id提交给服务器。

**cookie**

由浏览器维持的，存储在客户端的一小段文本信息，伴随着用户请求和页面在web服务器和浏览器之间传递。用户每次访问站点时，web应用程序都可以读取cookie包含的信息。浏览器设置里面有cookie隐私数据选项。

cookie是有时间限制的，根据生命期不同分为两种：会话cookie和持久cookie。

如果不设置过期时间，则表示这个cookie生命周期为从创建到浏览器关闭为止，只要不关闭浏览器窗口，cookie就消失。这种生命期为浏览会期的cookie被称为会话cookie。会话cookie一般不保存在硬盘上而是保存在内存中。

如果设置了过期时间，浏览器就会把cookie保存到硬盘上，关闭后再次打开浏览器，这些cookie依然有效直到超过设定的过期时间。存储在硬盘上的cookie可以在不同的浏览器进程间共享，比如两个IE窗口。而对于保存在内存的cookie，不同的浏览器有不同的处理方式。

**Go设置cookie**

~~~go
http.SetCookie(w responseWriter, cookie *Cookie)
//w表示需要写入的response，cookie是一个struct
type Cookie struct {
	Name  string
	Value string

	Path       string    // optional
	Domain     string    // optional
	Expires    time.Time // optional
	RawExpires string    // for reading cookies only

	// MaxAge=0 means no 'Max-Age' attribute specified.
	// MaxAge<0 means delete cookie now, equivalently 'Max-Age: 0'
	// MaxAge>0 means Max-Age attribute present and given in seconds
	MaxAge   int
	Secure   bool
	HttpOnly bool
	SameSite SameSite
	Raw      string
	Unparsed []string // Raw text of unparsed attribute-value pairs
}
~~~

设置cookie:

~~~go
expireation := *time.LocalTime()
expiration.Yead +=1
cookie := http.Cookie{Name:"username", Value:"astaxie", Expires : expiration}
http.SetCookie(w,&cookie)
~~~

**Go读取cookie**

~~~go
cookie, _ := r.Cookie("username")
fmt.Fprint(w,cookie)
//另一种读取方式
for _,cookie := range r.Cookies(){//注意：r.Cookie和r.Cookies是两个函数
    fmt.Fprint(w,cookie.Name)
}
~~~

**session**



