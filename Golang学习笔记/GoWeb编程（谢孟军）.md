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

本来含义是指有始有终的一系列动作/消息，当session与网络协议相关联时，往往隐含“面向连接”和/或“保持连接”这两个含义。

在Web开发环境下，指一类用来在客户端和服务器端之间保持状态的解决方案。

session机制是一种服务器端的机制，服务器使用一种类似于散列表的结构来保存信息。但程序为某个客户端的请求创建一个session的时候，服务器首先检查这个客户端的请求里是否包含了一个session标识（sessionID），如果已经包含一个sessionID则说明以前已经为此客户创建过一个session，服务器就按照sessionID把这个session检索出来使用（如果检索不到，可能会新建一个，这种情况出现在服务端已经删除了该用户对应的session对象，但用户认为地在请求的url后面附加上一个session的参数）。如果客户请求不包含session ID，则为此客户创建一个session并且同时生成一个与此session相关联的sessionID，这个sessionID将在本次响应中返回给客户端保存。

session机制本身并不复杂，然而其实现和配置上的灵活性却使得具体情况复杂多变。要求我们不能把仅仅某一次的经验或者某一个浏览器，服务器的经验当做普遍适用的。

## Go如何使用session

**session创建过程**

session的基本原理是由服务器为每一个会话维护一份信息数据，客户端和服务端依靠一个全局唯一的标识来访问这份数据，以达到交互的目的。当用户访问Web应用时，服务端程序会随需要创建session，这个过程概括为三个步骤：

1.  生成全局唯一标识符（sessionID）
2.  开辟数据存储空间。一般会在内存中创建相应的数据结构，但这种情况下，系统一旦停电，所有的会话数据就会丢失，这将造成严重后果。所以为了解决这类问题，可以将会话数据写到文件里或存储在数据库里，当然这样会增加IO开销，但是它可以实现某种程度的session持久化，也更有利于session共享
3.  将session的全局唯一标识符发送给客户端

上述三个步骤中最重要的是如何发送这个session的唯一标识符上，考虑到HTTP协议的定义，数据无非可以放到请求行、头域或Body里，所以一般来说会有两种常用的方式：cookie和URL重写。

1.  Cookie服务端通过设置Set-cookie头就可以将session的标识符传送到客户端，而客户端此后的每一次请求都会带上这个标识符，另外一般包含session信息的cookie会将失效时间设置为0（会话cookie），即浏览器进程有效时间。至于浏览器怎么处理这个0，每个浏览器都有自己的方案，但差别不会太大
2.  URL重写 所谓URL重写，就是在返回给用户的页面里的所有URL后面追加session标识符，这样用户在收到响应之后，无论点击响应页面里的哪个连接或提交表单，都会自动带上session标识符，从而就实现了会话的保持。虽然这种做法比较麻烦，但是，如果客户端禁用了cookie的话，这种方案就是首选

**Go实现session管理**

**session管理设计**

-   全局session管理器
-   保证sessionID的全局唯一性
-   为每个客户关联一个session
-   session的存储（可以存储到内存、文件、数据库等）
-   session过期处理

**session劫持防范**

一个解决方案是sessionID的值只允许cookie设置，而不是通过URL重置方式设置，同时设置cookie的httponly为true，这个属性是设置是否可通过客户端脚本访问这个设置的cookie。第一，这个可以防止cookie被XSS读物从而引起session劫持。第二，cookie设置不会像URL重置方式那么容易获取sessionID。

第二步就是在每个请求里面加上token，实现类似防止form重复递交类似的功能，在每个请求里面加上一个隐藏的token，然后每次验证这个token，从而保证用户的请求都是唯一性的。

**间隔生成新的SID**

另一个解决方案是：给session额外设置一个创建时间的值，一旦过了一定的时间，就销毁这个sessionID，重新生成新的sessionID,，这样可以一定程度上防止session劫持的问题。



# 文本处理

## XML处理

**解析XML**

可以通过xml包里的Unmarshal函数达到目的：

~~~go
func Unmarshal(data []byte, v interface{})error
~~~

其中data接收的是XML数据流，v是需要输出的结构，定义为interface{}，就是可以把XML转换成任意格式。目前支持struct，slice和string。XML包内部采用反射来进行数据的映射，所以v里面的字段必须是导出的。Unmarshal解析的时候XML元素和字段怎么对应起来：这是一个优先级读取流程，首先会读取struct tag，如果没有，那么就会对应字段名。必须注意一点的是解析的时候tag、字段名和XML元素都是大小写敏感的，所以必须一一对应字段。

解析XML到struct的时候遵循如下规则：

-   如果struct的一个字段是string或者[]byte类型且它的tag含有`“,innerxml”`，Unmarshal将会对此字段所对应的元素内嵌的原始xml累加到此字段上
-   如果struct字段中有一个叫做XMLName，且类型为xml.Name子弹，那么在解析的时候就会保存这个element的名字到该字段
-   如果某个struct字段的tag定义中含有XML结构中element的名称，那么解析的时候就会把相应的element值赋值给该字段
-   如果某个struct字段的tag定义中含有`“,attr”`，那么解析的时候就会将该结构对应的element的与字段同名的属性的值赋值给该字段
-   如果某个struct字段的tag定义型如：`“a>b>c”`， 则解析的时候，会将xml结构a下面的b下面的c元素的值赋值给该字段
-   如果某个struct字段的tag定义了`“-”`，那么不会为该字段解析匹配任何xml数据
-   如果某个struct字段后面的tag定义了`“,any”`，那么如果他的子元素在不满足其他的规则的时候就会匹配到这个字段
-   如果某个XML元素包含一条或者多条注释，那么这些注释将被累加到第一个tag含有`“,comments”`的字段上，这个字段的类型可能是[]byte或string，如果没有这样的字段存在，那么注释将会被抛弃

注：为了正确解析，Go语言的xml包要求struct定义中的所有字段必须是可导出的。

**输出XML**

~~~go
func Marshal(v interface{})([]byte error)
func MarshalIndent(v interface{},prefix, indent string)([]byte, error)
~~~

注意：上述两个函数输出的信息都不带xml头的，因此，需要手动加入`xml.Header`。

xml包生成相应的xml文件规则：

-   如果v是array或者slice，那么输出每一个元素，类似value
-   如果v是指针，那么会Marshal指针指向的内容，如果指针为空，什么都不输出
-   如果v是interface，那么就处理interface包含的数据
-   如果v是其他数据类型，就会输出这个数据类型所拥有的字段信息

生成的XML文件中的element的名字按照如下优先级从struct中获取：

-   如果v是struct，XMLName的tag中定义的名称
-   类型为xml.Name的名叫XMLName的字段的值
-   通过struct中字段的tag来获取
-   通过struct的字段名用来获取
-   marshall的类型名称

如何设置struct中字段的tag信息用来控制最终xml文件的生成：

-   XMLName不会被输出
-   tag中含有`“-”`的字段不会被输出
-   tag中含有`“name,attr”`，会以name作为属性名，字段值作为值输出为这个XML元素的属性
-   tag中含有`“,attr”`，会以这个struct的字段名作为属性名输出为XML元素的属性，类似上一条，只是这个name默认是字段名
-   tag中含有`“,chardata”`，输出为xml的character data而非element
-   tag中含有`“,innerxml”`，将会被原样输出，而不会进行常规的编码过程
-   tag中含有`“,comment”`，将会被当作xml注释来输出，而不会进行常规的编码过程，字段值中不能含有`--`字符串
-   tag中含有`“omitempty”`，如果该字段值的值为空值那么该字段就不会被输出到XML，空值包括：false,0,nil指针或nil接口，任何长度为0的array，slice，map或者string
-   tag中含有`“a>b>c”`，那么就会循环输出三个元素a包含b，b包含c，例如如下代码就会输出：

~~~xml
FirstName 	string	`xml:"name>first"`
LastName	string	`xml:"name>last"`

<name>
    <first>Asta</first>
    <last>Xie</last>
</name>
~~~

## JSON处理

**解析JSON**

~~~go
func Unmarshal(data []byte,v interface{}) error
~~~

-   首先查找tag含有JSON的key的可导出的struct字段（首字母大写）
-   其次查找字段名与JSON的key相同的导出字段
-   最后查找类似key或者key这样的除了首字母之外其他大小写不敏感的导出字段

注：能够被赋值的字段必须是可导出字段（首字母大写）。同时JSON解析的时候只会解析能找到的字段，如果找不到的字段就会被忽略，这样的一个好处是：当接收到一个很大的JSON数据结构哦而却只想获取部分数据的时候，只需将想要的数据对应的字段名大写，即可解决。

**解析到interface**

JSON包中采用map[string]interface{}和[]interface{}结构来存储任意JSON对象和数组。Go类型和JSON类型的对应关系如下：

-   bool代表JSON booleans
-   float64代表JSON numbers
-   nil代表JSON null

针对JSON的输出，在定义struct tag的时候需要注意一下：

-   字段的tag是“-”，那么这个字段就不会输出到JSON
-   tag中带有自定义名称，那么这个自定义名称会出现在JSON的字段名中
-   tag中如果带有`“omitempty”`选项，那么如果该字段值为空，就不会输出到JSON串中
-   如果字段类型是bool,string,int,int64等，而tag中带有`“,string”`选项，那么这个字段在输出到JSON的时候会把该字段对应的值转换为JSON字符串。

通过Marshal函数进行转换的时候，需要注意几点：

-   JSON对象只支持string作为key，所以要编码一个map，那么必须是map[string]T这种类型
-   Channel，complex和function是不能被编码成JSON
-   嵌套的数据是不能被编码的，不然会让JSON编码进入死循环
-   指针在编码的时候会输出指针指向的内容，而空指针会输出null

## 正则处理

**通过正则判断是否匹配**

~~~go
func Match(pattern string, b []byte)(matched bool, err error)
func MatchReader(pattern string, r io.RuneReader)(matched bool, err error)
func MatchString(pattern string, s string)(matched bool, err error)
~~~

上面三个函数实现了同一个功能，就是判断pattern是否和输入源匹配，匹配的话就返回true，如果解析正则错误则返回error。

**解析正则**

~~~go
func Compile(expr string)(*Regexp,error)
func CompilePOSIX(expr string)(*Regexp,error)
func MustCompile(str string)*Regexp
func MustCompilePOSIX(str string)*Regexp
~~~

CompilePOSIX和Compile的不同点在于POSIX必须使用POSIX语法，它使用最左最长方式搜索，而Compile是采用最左方式搜索（例如[a-z]{2,4}这样一个正则表达式，应用于“aa09aaa88aaaa”这个文本串时，CompilePOSIX返回aaaa,而Compile则返回的是aa）。前缀有Must的函数表示，在解析正则语法的时候，如果匹配模式串不满足正确的语法则直接panic，而不加Must的则只是返回错误。



## 模板处理

**模板中如何插入数据**

字段操作

Go语言的模板通过`{{}}`来包含需要在渲染时被替换的字段，`{{.}}`表示当前的对象，如果要访问当前对象的字段通过`{{.FieldName}}`，但是需要注意的是，这个字段必须是导出的，否则在渲染的时候就会报错

**输出嵌套字段**

如果字段里面还有对象，可以使用`{{with ...}}...{{end}}`和`{{range ...}}{{end}}`来进行数据的输出。

-   `{{range}}`这个和Go语法里面的range类似，循环操作数据
-   `{{with}}`操作是值当前对象的值，类似于上下文的概念，必须要是有效的地址

**条件处理**

在Go模板里面如果需要进行条件判断，可以使用和Go语言的if-else语法类似的方式来处理

~~~go
{{if .Flag}}
	if部分
{{else if .Flag1}}
	else if部分
{{else}}
	else部分
{{end}}
~~~

注意：if里面无法进行条件判断，只能是bool值

**pipeline**

~~~go
{{. | html}}
~~~

类似于UNIX的pipeline，上述部分是将输出全部转化为html的实体。

**模板变量**

~~~go
$variable := pipeline
//详细例子
{{with $x := "output" | printf "%q"}}{{$x}}{{end}}
{{with $x := "output"}}{{$x}}{{end}}
{{with $x := "output"}}{{$x | printf "%q"}}{{end}}
~~~

**模板函数**模板在输出对象的字段值时，采用了fmt包把对象转化成了字符串，但是有时候需求可能不是这样，比如进行字符串替换，此时需要自定义函数来实现这个功能。

每一个模板函数都有一个唯一的名字，然后与一个Go函数关联，通过如下的方式关联

~~~go
type FuncMap map[string]interface{}
~~~

例如，想要email函数的模板函数名是emailDeal，它关联的Go函数名称是EmailDealWith，那么可以通过如下方式来注册这个函数：

~~~go
t = t.Funcs(template.FuncMap{"emailDeal":EmailDealWith})
//EmailDealWith函数原型必须是
func EmailDealWith(args ...interface{})string
~~~

示例代码如下：

~~~go
package main
//Friend 里面存放名字
type Friend struct {
	Fname string
}

//Person 存放人员信息
type Person struct {
	UserName string
	Emails   []string
	Friends  []*Friend
}

//EmailDealWith 替换@
func EmailDealWith(args ...interface{}) string {
	ok := false
	var s string
	if len(args) == 1 {
		s, ok = args[0].(string)
	}
	if !ok {
		s = fmt.Sprint(args...)
	}

	//find the @ symbol
	substrs := strings.Split(s, "@")
	if len(substrs) != 2 {
		return s
	}
	return strings.Join(substrs, "at")
}
func main() {
	f1 := Friend{Fname: "yang.go"}
	f2 := Friend{Fname: "yu"}
	t := template.New("fieldname example")
	t = t.Funcs(template.FuncMap{"emailDeal": EmailDealWith})
	t, _ = t.Parse(`hello {{.UserName}}!
					{{range .Emails}}
						an Emails {{.|emailDeal}}
					{{end}}
					{{with .Friends}}
					{{range .}}
						my friend name is {{.Fname}}
					{{end}}
					{{end}}
					`)
	curstr := make([]string, 2)
	curstr[0] = "yang@gmail.com"
	curstr[1] = "yu@163.com"
	p := Person{UserName: "YONGZHOU",
		Emails:  curstr,
		Friends: []*Friend{&f1, &f2}}
	t.Execute(os.Stdout, p)
}
~~~

**Must操作**

作用是检测模板是否正确，例如大括号是否匹配，注释是否正确的关闭，变量是否正确的书写

