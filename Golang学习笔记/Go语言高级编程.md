# Go语言高级编程

## 汇编

**定义整数变量**

~~~go
package pkg

var Id = 9527
~~~

使用以下命令查看Go语言程序对应的伪汇编代码：

~~~go
$ go tool compile -S pkg.go
"".Id SNOPTRDATA size=8
0x0000 37 25 00 00 00 00 00 00                          7%......
~~~

其中`go tool compile`命令用于调用Go语言提供的底层命令工具，其中`-S`参数表示输出汇编格式。输出的汇编比较简单，其中`“”.Id`对应`Id`变量符号，变量的内存大小是8个字节。变量的初始化内容为`37 25 00 00 00 00 00 00`，对应9527。`SNOPTRDATA`是相关标志，其中`NOPTR`表示数据中不包含指针数据。

**定义字符串变量**

~~~go
var Name = "gopher"
~~~

~~~go
go.string."gopher" SRODATA dupok size=6
        0x0000 67 6f 70 68 65 72                                gopher
"".Name SDATA size=16
        0x0000 00 00 00 00 00 00 00 00 06 00 00 00 00 00 00 00  ................
        rel 0+8 t=1 go.string."gopher"+0
~~~

输出中出现了一个新的符号`go.string."gopher"`，根据其长度和内容分析可以猜测是对应底层的“gopher”字符串数据。因为Go语言的字符串并不是值类型，Go字符串其实是一种只读的引用类型。如果代码中出现了相同的”gopher“只读字符串时，程序链接后可以引用的同一个符号`go.string."gopher"`。因此，该符号有一个`SRODATA`标志表示这个数据在只读内存段，`dupok`表示出现多个相同的标识符的数据时只保留一个就可以了。

真正Go字符串变量`Name`对应的大小只有16个字节，其实`Name`变量并没有直接对应“gopher”字符串，而是对应16字节大小的`reflect.StringHeader`结构体。

~~~go
type reflect.StringHeader struct {
    Data 	uintptr
    Len 	int
}
~~~

从汇编角度看，Name变量其实对应的是`reflect.StringHeader`结构体类型。前8个字节对应底层真实字符串数据的指针，也就是符号`go.string."gopher"`对应的地址。后8个字节对应底层真实字符串数据的有效长度，这里是6个字节。

## `RPC`和`Protobuf`

`RPC`是远程过程调用的缩写，通俗地说就是调用远处的一个函数。是分布式系统中不同节点间流行的通信方式。

Go语言的RPC规则：方法只能有两个可序列化的参数，其中第二个参数是指针类型，并且返回一个error类型，同时必须是公开的方法。

然后就可以定义的类型的对象注册为一个RPC服务。

~~~go
rpc.RegisterName(name string，new(对象))
~~~

其中`rpc.Register`函数会将对象类型中所有满足RPC规则的对象方法注册为RPC函数，所有注册的方法会放在`name`服务空间之下。然后建立一个唯一的TCP连接，并且通过`rpc.ServeConn`函数在该TCP连接上为对方提供RPC服务。

客户端：首先是通过`rpc.Dial`拨号RPC服务，然后通过`client.Call`调用具体的RPC方法。在调用`client.Call`时，第一个参数是用点号链接的RPC服务名字和方法名字，第二和第三个参数分别是我们定义的RPC方法的两个参数。

#### 更安全的RPC接口

开发人员分为三种：

-   服务端实现RPC方法的开发人员
-   客户端刁颖RPC方法的人员
-   制定服务端和客户端RPC接口规范的设计人员

如果要重构`rpc`服务，第一步需要明确服务的名字和接口：

~~~go
const HelloServiceName = "path/to/pkg.HelloService"

type HelloServiceInterface interface{
    Hello(request string, reply *string)error
}
func RegisterHelloService(svc HelloServiceInterface) error{
    return rpc.RegisterName(HelloServiceName, svc)
}
~~~

RPC服务的接口规范分为三个部分：首先是服务的名字，然后是服务要实现的详细方法列表，最后是注册该类型服务的函数。为了避免名字冲突，在RPC服务的名字中增加了包路径前缀（这个是RPC服务抽象的包路径，并非完全等价Go语言的包路径）。

在定义了RPC服务接口规范之后，客户端就可以根据规范编写RPC调用的代码了：

~~~go
func main(){
    cilent, err := rpc.Dial("tcp","localhost:1234")
    if err!=nil {
        log.Fatal("dialing:",err)
    }
    var reply string
    err = cilent.Call(HelloServiceName+".Hello","hello",&reply)
    if err != nil {
        log.Fatal(err)
    }
}
~~~

通过`client.Call`函数调用RPC方法依然比较繁琐，同时参数的类型依然无法得到编译器提供的安全保障。可以在接口规范部分增加对客户端的简单包装：

~~~go
type HelloServiceClient struct {
    *rpc.Client
}

var HelloServiceInterface = (*HelloServiceClient)(nil)
func DialHelloService( network, address string) (*HelloServiceClient, error){
    c, err := rpc.Dial(network,address)
    if err!=nil{
        return nil,err
    }
    return &HelloServiceClient{Client: c},nil
}
func (p *HelloServiceClient) Hello (request string, reply *string) error {
    return p.Client.Call(HelloServiceName+".Hello",request,reply)
}
~~~

#### 跨语言的RPC

**`jsonrpc`**

服务端：

~~~go
rpc.RegisterName(ServiceName, new(Service))
listener, err := net.Listen("tcp",":1234")

conn, err := listener.Accept()
rpc.ServeCodec(jsonrpc.NewServeCodec(conn))
~~~

客户端：

~~~go
conn, err := net.Dial("tcp",":1234")
client := rpc.NewClientWithCodec(jsonrpc.NewClientCodec(conn))
client.Call(Servicefunc,in,&out)
~~~

请求的`json`数据对象在内部对应两个结构体：客户端是`clientRequest`，服务端是`serverRequest`。内容基本一致：

~~~go
type clientRequest struct {
    Method 	string			`json:"method"`
    Params 	[1]interface{}	`json:"params"`
    Id		uint64			'json:"id"'
}

type serverRequest struct {
    Method 	string			`json:"method"`
    Params 	*json.RawMessage`json:"params"`
    Id		*json.RawMessage'json:"id"'
}
~~~

其中服务端返回的结果：id对应输入的id参数，result为返回的结果，error部分在出问题时表示错误信息。对于顺序调用来说，id不是必须的。但是Go语言的RPC框架支持异步调用，当返回结果的顺序和调用的顺序不一致时，可以通过id来识别对应的调用。

返回的`json`数据也是对应内部的两个结构体：客户端是`clientResponse`，服务端是`serverResponse`。两个结构体的内容同样也是类似的：

~~~go
type clientResponse struct {
    Id 		uint64				`json:"id"`
    Result	*json.RawMessage	`json:"result"`
    Error	interface{}			`json:"error"`
}
type serverResponse struct {
    Id 		*json.RawMessage	`json:"id"`
    Result	interface{}			`json:"result"`
    Error	interface{}			`json:"error"`
}
~~~

#### HTTP上的RPC

新的RPC服务其实是一个类似REST规范的接口，接收请求并采用相应处理流程：

~~~go
rpc.RegisterName("HelloService", new(HelloService))
http.HandleFunc("/jsonrpc",func(w http.ResponseWriter, r *http.Request)){
    var conn io.ReadWriteCloser = struct{
        io.Writer
        io.ReadCloser
    }{
        ReadCloser: r.Body
        Writer:w
    }
    rpc.ServeRequest(jsonrpc.NewServerCodec(conn))
}
http.ListenAndServe(":1234",nil)
~~~

RPC的服务假设在“/jsonrpc”路径，在处理函数中基于`http.ResponseWriter`和`http.Request`类型的参数构造一个`io.ReadWriteCloser`类型的`conn`通道。然后基于conn构建针对服务端的json编码解码器。最后通过`rpc.ServeRequest`函数为每次请求处理一个RPC方法调用。

### `Protobuf`

`Protobuf`的最基本的数据单元是message，类似于结构体。

首先创建`hello.proto`文件，其中包装`HelloService`服务中用到的字符串类型:

~~~protobuf
syntax = "proto3"
package main
message String{
    string value = 1;
}
~~~

开头的`syntax`语句表示采用`proto3`的语法。所有成员均采用类似Go语言中的零值初始化（不再支持自定义默认值），因此消息成员也不再需要支持required特性。然后package指令指明当前是main包（这样可以和Go的包名保持一致），也可以针对不同的语言定制对应的包路径和名称。最后message关键字定义一个新的String类型，在最终生成的Go语言代码中对应一个String结构体。String类型中只有一个字符串类型的value成员，该成员编码时用1编号代替名字。

在XML或JSON等数据描述语言中，一般通过成员的名字来绑定对应的数据。但是`Protobuf`编码却是通过成员的唯一编号来绑定对应的数据，因此`Protobuf`编码后数据的体积会比较小。

**用`Protobuf`定义语言无关的RPC服务接口是它真正的价值所在**。

通过`Protobuf`来定义`HelloService`服务,在`hello.proto`文件中定义如下：

~~~protobuf
service HelloService {
    rpc Hello(String) returns (String);
}
~~~

### 玩转RPC

#### 反向RPC

通常的RPC是基于C/S结构，RPC的服务端对应网络的服务器，RPC的客户端也对应网络客户端。但是对于一些特殊场景，比如在公司内网提供一个RPC服务，但是在外网无法链接到内网的服务器。这种时候我们可以参考类似反向代理的技术，首先从内网主动链接到外网的TCP服务器，然后基于TCP链接向外网提供RPC服务。

以下是启动反向RPC服务的代码：

~~~go
func main(){
    rpc.Register(new(HelloService))
    for {
        conn, _ := net.Dial("tcp","localhost:1234")
        if conn == nil {
            time.Sleep(time.Second)
            continue
        }
        rpc.ServeConn(conn)
        conn.Close()
    }
}
~~~

反向RPC的内网服务将不再主动提供TCP监听服务，而是首先主动链接到对方的TCP服务器。然后基于每个建立的TCP链接向对方提供RPC服务。

而RPC客户端则需要在一个公共的地址提供一个TCP服务，用于接收RPC服务器的连接请求：

~~~go
func main(){
    listener, err := net.Listen("tcp",":1234")
    clientChan := make(chan *rpc.Client)
    go func(){
        conn, err :=listener.Accept()
        clientChan <- rpc.NewClient(conn)
    }()
    doClientWork(clientChan)
}
~~~

当每个链接建立后，基于网络链接构造RPC客户端对象并发送到`clientChan`管道。客户端执行RPC调用的操作在`doClientWork`函数完成：

~~~go
func doClientWork (clientChan <- chan*rpc.Client){
    client := <-clientChan
    defer client.Close()
    
    var reply string
    err = client.Call("HelloService.Hello","hello",&reply)
}
~~~

首先从管道去取一个RPC客户端对象，并且通过defer语句指定在函数退出前关闭客户端，然后是执行正常的RPC调用。

#### 上下文信息

基于上下文可以针对不同客户端提供定制化的RPC服务。

首先改造`HelloService`，里面增加对应链接的conn成员：

~~~go
type HelloService struct {
    conn net.Conn
}
~~~

然后为每个链接启动独立的RPC服务：

~~~go
func main(){
    listener, err := net.Listen("tcp",":1234")
    if err != nil {
        log.Fatal("ListenTCP error:", err)
    }
    for{
        conn, err := listener.Accept()
        if err != nil {
            log.Fatal("Accept error:",err)
        }
        
        go func(){
            defer conn.Close()
            p:=rpc.NewServer()
            p.Register(&HelloService{conn:conn})
            p.ServeConn(conn)
        }()
    }
}
~~~

基于上下文信息，可以方便地为RPC服务增加简单的登陆状态的验证：

~~~go
type HelloService struct {
	conn    net.Conn
	isLogin bool
}
func (p *HelloService) Hello(request string, reply *string) error {
	if !p.isLogin {
		return fmt.Errorf("please login")
	}
	*reply = "hello:" + request + ",from" + p.conn.RemoteAddr().String()
	log.Println("Hello ok")
	return nil
}

func (p *HelloService) Login(request string, reply *string) error {
	if request != "user:password" {
		return fmt.Errorf("auth failed")
	}
	log.Println("login ok")
	p.isLogin = true
	*reply = fmt.Sprintf("login ok")
	return nil
}
~~~

这样可以要求客户端链接RPC服务时，首先要执行登陆操作，登陆成功后才能正常执行其他的服务。

客户端代码如下：

~~~go
type HelloService struct {
	conn    net.Conn
	isLogin bool
}

func main() {
	conn, err := rpc.Dial("tcp", ":1234")
	if err != nil {
		log.Fatal("TCPDial error:", err)
	}
	defer conn.Close()
	var reply string
	if err = conn.Call("HelloService.Login", "user:password", &reply); err != nil {
		log.Fatal("login error:", err)
	}
	fmt.Println("login recv:", reply)

	if err = conn.Call("HelloService.Hello", "world", &reply); err != nil {
		log.Fatal("hello error:", err)
	}
	fmt.Println("hello recv:", reply)
}
~~~

### `gRPC`入门

## Go Web

### router请求路由

RESTful如下：

~~~go
const(
	MethodGet 		= "GET"
    MethodHead		= "HEAD"
    MethodPost		= "POST"
    MethodPut		= "PUT"
    MethodPatch		= "PATCH"
    MethodDelete	= "DELETE"
    MethodConnect	= "CONNECT"
    MethodOptions	= "OPTIONS"
    MethodTrace		= "TRACE"
)
~~~

常见的请求路径：

~~~go
GET 	/repos/:owner/:repo/comments/:id/reactions
POST	/projects/:project_id/columns
PUT		/user/starred/:owned/:repo
DELETE	/user/starred/:owner/:repo
~~~

RESTful风格的API重度依赖请求路径。会将很多参数放在请求URI中。除此之外还会使用并不那么常见的HTTP状态码。

#### `httprouter`

因为`httprouter`中使用的是显式匹配，所以在设计路由的时候需要规避一些会导致路由冲突的情况，例如：

~~~go
conflict:
GET		/user/info/:name
GET 	/user/:id

no conflict:
GET		/user/info/:name
POST	/user/:id
~~~

简单来说，如果两个路由拥有一致的http方法（指GET/POST/DELETE）和请求路径前缀，且在某个位置出现A路由是`wildcard`（指`:id`这种形式）参数，B路由则是普通字符串，那么就会发生路由冲突。路由冲突会在初始化阶段直接panic。

另外就是，`httprouter`考虑到字典树的深度，在初始化时会对参数的数量进行限制，所以在路由中的参数数目不能超过255，否则会导致`httprouter`无法识别后续的参数。

除支持路径中的wildcard参数之外，`httprouter`还可以支持`*`号来进行通配，不过`*`号开头的参数只能放在路由的结尾，如下：

~~~go
Pattern:	/src/*filepath

/src				filepath = ""
/src/somefile.go	filepath = "somefile.go"
~~~

这种设计在RESTful中可能不太常见，主要是为了能够使用`httprouter`来做简单的HTTP静态文件服务器。

除了正常情况下的路由支持，`httprouter`也支持对一些特殊情况下的回调函数进行定制，例如404的时候：

~~~go
r := httprouter.New()
r.NotFound = http.HandlerFunc(func(w http.ResponseWriter, r *http.Request){
    w.Write([]byte("oh no, not found"))
})
~~~

或者内部panic的时候：

~~~go
r.PanicHandler = func(w http.ResponseWriter, r *http.Request, c interface{}){
    log.Printf("Recovering from panic, Reason: %#v",c.(error))
    w.WriteHeader(http.StatusInternalServerError)
    w.Write([]byte(c.(Error).Error()))
}
~~~

#### 原理

`httprouter`使用的数据结构是压缩字典树。其中每个节点不只存储一个字母。使用压缩字典树可以减少树的层数，同时因为每个节点上数据存储也比通常的字典树要多，所以程序的局部性较好，从而对CPU缓存友好。

#### root节点创建

~~~~go
type Router struct{
    //...
    trees map[string]*node
    //...
}
~~~~

trees中的key为HTTP1.1的RFC中定义的各种方法，具体有：

~~~http
GET
HEAD
OPTIONS
POST
PUT
PATCH
DELETE
~~~

每一种方法对应的都是一颗独立的压缩字典树，这些树彼此之间不共享数据。

某个方法第一次插入的路由就会导致对应字典树的根节点被创建。

压缩字典树的节点类型为`*httprouter.node`，其中部分结构说明如下：

~~~go
path:当前节点对应的路径中的字符串

wildChild:子节点是否为参数节点，即wildcard node，或者说， :id 这种类型的节点

nType:当前节点类型，有四个枚举值：分别为static/root/param/catchAll.
	static:		//非根节点的普通字符串节点
	root:		//根节点
	param:		//参数节点，例如:id
	catchAll:	//通配符节点，例如*anyway
indices:子节点索引，当子节点为非参数类型，即本节点的wildChild为false时，会将每个子节点的首字母放在该索引数组。说是数组，实际上是个string
~~~

#### 子节点插入

当插入`GET /marketplace_listing/plans`，GET树的结构股如下：

~~~go
path:"/marketplace_listing/plans/"
wildChild:false
nType:root
indices:""
~~~

因为第一个路由没有参数，path都被存储到根节点上了，所以只有一个节点。

然后插入`GET /marketplace_listing/plans/:id/accounts`，新的路径与之前的路径有共同的前缀，且可以直接在之前叶子节点后进行插入，那么结果也很简单：

~~~go
path:"/marketplace_listing/plans/"
wildChild:true
nType:root
indices:""

path:":id"
wildChild:false
nType:param
indices:""

path:"/accounts"
wildChild:false
nType:default
indices:""
~~~

由于`:id`这个节点只有一个字符串的普通子节点，所以indices还依然不需要处理。

#### 边分裂

然后插入`GET /search`这会导致树的边分裂：

~~~go
//一级节点
path:"/"
wildChild:false
nType:root
indices:"ms"

//二级节点
path:"marketplace_listing/plans/"
wildChild:true
nType:default
indices:""

path:"search"
wildChild:false
nType:default
indices:""

//下层节点
...
~~~

原有路径和新的路径在初始的`/`位置发生分裂，这样需要把原有的root节点内容下移，再由新路由`search`同样作为子节点挂在root节点之下。这时候因为子节点出现多个，root节点的indices提供子节点索引，这时候该字段就需要排上用场。`“ms”`代表子节点的首字母分别为m(marketplace)和s(search)。

#### 子节点冲突处理

在路由本身只有字符串的情况下，不会发生任何冲突。只有当路由中含有`wildChild`（类似:id）或者`catchAll`的情况下才可能发生冲突。

子节点的冲突处理很简单，分几种情况：

1.  在插入`wildChild`节点时，父节点的`children`数组非空且`wildChild`被设置为`false`。例如`GET /user/getAll`和`GET /user/:id/getAddr`，或者`GET /user/*aaa`和`GET /user/:id`。
2.  在插入`wildChild`节点时，父节点的`children`数组非空且`wildChild`被设置为`true`，但该父节点的`wildChild`子节点要插入的`wildChild`名字不一样。例如`GET /user/:id/info`和`GET /user/:name/info`。
3.  在插入`catchAll`节点时，父节点的children非空。例如：`GET /src/abc`和`GET /src/*filename`，或者`GET  /src/:id`和`GET /src/*filename`。
4.  在插入static节点时，父节点的`wildChild`字段被设置为`true`。
5.  在插入static节点时，父节点的children非空，且子节点`nType`为`catchAll`。

只要发生冲突，都会在初始化的时候`panic`。

## 中间件

`http.Handler`的定义在`net/http`包中：

~~~go
type Handler interface {
    ServeHTTP(ResponseWriter, *Request)
}
~~~

任何方法实现了`ServeHTTP`，即是一个合法的`http.Handler`。`http`库的`Handler`，`HandlerFunc`和`ServeHTTP`的关系：

~~~go
type Handler interface {
    ServeHTTP(ResponseWriter, *Request)
}

type HandlerFunc func(ResponseWriter, * Request)

func (f HandlerFunc) ServeHTTP(w ResponseWriter, r *Request){
    f(w,r)
}
~~~

只要handler函数签名是：

~~~go
func(ResponseWriter, *Request)
~~~

那么这个`handler`和`http.HandlerFunc`就有了一致的函数签名，可以将该`handler()`函数进行类型转换，转为`http.HandlerFunc`。而`http.handlerFunc`实现了`http.Handler`这个接口。在`http`库需要调用`handler`函数来处理`http`请求时，会调用`HandlerFunc()`的`ServeHTTP()`函数，可见一个请求的基本调用链如下：

~~~go
h = getHandler() -> h.ServeHTTP(w,r) -> h(w,r)
~~~

把自定义的`handler`转换为`http.HandlerFunc()`这个过程是必须的，因为自定义的`handler`没有直接实现`ServeHTTP`这个接口。注意：`HandlerFunc`和`HandleFunc`的区别：

~~~go
func HandleFunc(pattern string, handler func(ResponseWriter, *Request)){
    DefaultServeMux.HandleFunc(pattern,handler)
}
//调用
func (mux *ServeMux) HandleFunc(pattern string, Handler func(ResponseWriter, *Request)){
    mux.Handle(pattern, HandlerFunc(handler))
}
~~~

中间件要做的事就是通过一个或多个函数对handler进行包装，返回一个包括了各个中间件逻辑的函数链。例如：

~~~go
customizedHandler = logger(timeout(ratelimit(helloHandler)))

调用关系如下：
logger -> timeout -> ratelimit -> helloHandler -> helloHandler -> ratelimit 
-> timeout -> logger
~~~

这个流程在进行请求处理的时候就是 不断地进行函数压栈再出栈。

### 更优雅的中间件写法

~~~go
r = NewRouter()
r.Use(logger)
r.Use(timeout)
r.Use(ratelimit)
r.Add("/",helloHandler)
~~~

实现：

~~~go
type middleware func(http.Handler) http.Handler

type Router struct {
    middlewareChain []middleware
    mux map[string]http.Handler
}

func NewRouter() *Router {
    return &Router{}
}

func (r *Router) Use(m middleware){
    r.middlewareChain = append(r.middlewareChain,m)
}

func (r *Router) Add(route string, h http.Handler) {
    var mergedHandler = h
    for i:= len(r.middlewareChain)-1; i>=0; i-- {
        mergedHandler = r.middlewareChain[i](mergedHandler)
    }
    r.mux[route] = mergedHandler
}
~~~

## validator请求校验

### 重构请求校验函数

假设数据已经通过某个开元绑定库绑定到了具体的结构体上。

~~~go
type RegisterReq struct {
    Username		string		`json:"username"`
    PasswordNew		string		`json:"password_new"`
    PasswordRepeat	string		`json:"password_repeat"`
    Email			string		`json:"email"`
}
func register(req RegisterReq) error {
    if len(req.Username) >0 {
        if len(req.PasswordNew) > 0 && len(req.PasswordRepeat) > 0 {
            if req.PasswordNew == req.PasswordRepeat {
                if emailFormatValid(req.Email){
                    createUser()
                    return nil
                }else{
                    return errors.New("invalid email")
                }
            }else{
                return errors.New("password and reinput must be the same")
            }
        }else{
            return errors.New("password and password reinput must be longer than 0")
        }
    }else{
        return errors.New("length of username cannot be 0")
    }
}
~~~

优化如下：

~~~go
func register(req RegisterReq) error{
    if len(req.Username)==0{
        return errors.New("length of username cannot be 0")
    }
    if len(req.PasswordNew)==0 || len(req.PasswordRepeat)==0{
        return errors.New("password and password reinput must be longer than 0")
    }
    if req.PasswordNew != req.PasswordRepeat {
        return errors.New("password and reinput must be the same")
    }
    if emailFormatValid(req.Email){
        return error.New("invalid email")
    }
    createUser()
    return nil
}
~~~

上述方法虽然令校验过程代码看起来优雅，但是还是需要为每一个http请求都去写一套差不多的`validate()`函数，使用validator可以解除这项劳动。

其中validator的原理主要采用反射对结构体进行树形便利。如果由于反射的校验成了系统的性能瓶颈，此时可以使用Go内置的`Parser`对源代码进行扫描，然后根据结构体的定义生成校验代码。

## Database和数据库打交道



### database/sql

`database/sql`库实际上只提供了一套操作数据库的接口和规范，例如抽象好的SQL预处理，连接池管理，数据绑定，事务，错误处理等。但是并没有提供具体某种数据库实现的协议支持。和具体数据打交道，还需要再引入相应数据库的驱动：

~~~go
import (
	"database/sql"
    _ "github.com/go-sql-driver/mysql"
)

db, err := sql.Open("mysql","user:password@/dbname")
~~~

其中

~~~go
import _ "github.com/go-sql-driver/mysql"
~~~

这条语句会调用`mysql`包的`init`函数，做的事情也很简单：

~~~go
func init(){
    sql.Register("mysql",&MySQLDriver{})
}
~~~

在`sql`包的全局`map`里把`mysql`这个名字的`driver`注册上。`Driver`在`sql`包中是一个接口：

~~~go
type Driver interface{
    open(name string)(Conn, error)
}
~~~

调用`sql.Open()`返回的`db`对象就是这里的`Conn`。

~~~go
type Conn interface {
    Prepare(query string) (Stmr, error)
    Close() error
    Begin()(Tx, error)
}
~~~

也是一个接口。

### 提高生产效率的ORM和SQL Builder

~~~go
对象关系映射（Object Relational Mapping,ORM)是一种程序设计技术，用于实现面向对象编程语言里不同类型系统的数据之间的转换。从效果上讲，其实是一个可在编程语言里使用的“虚拟对象数据库”
~~~

最常见的`ORM`做的是从`db`到程序的类或结构体这样的映射，ORM的目的是屏蔽掉DB层。

但是ORM可能会对数据库的读请求放大N倍。比如一个商品列表有15个SKU，那么每次打开这个页面，至少需要执行1（查询商品列表）+15（查询相关的商铺信息）次查询，如果商品列表继续增加，查询次数成线性增长。可以使用过滤器：

~~~go
o := orm.NewOrm()
num, err := o.QueryTable("cardgroup").Filter("Cards_Card_Name",cardName).All(&cardgroups)
~~~

在某些ORM背后可能隐藏了非常难以察觉的细节，比如生成的SQL语句会自动`limit 1000`。

由于隐藏重要的细节，ORM可能是失败的设计。其所隐藏的重要细节对于上了规模的系统开发来说至关重要。

相比ORM来说，SQL Builder在SQL和项目可维护性之间取得了比较好的平衡。首先SQL Builder不像ORM那样屏蔽了过多的细节，其次从开发的角度来讲，SQL Builder进行简单的封装后也可以非常高效的完成开发。比如：

~~~~go
where := map[string]interface{
    "order_id > ?" : 0,
    "customer_id != ?" : 0,
}
limit := []int{0,100}
orderBy := []string{"id asc", "create_time desc"}
orders := orderModel.GetList(where,limit,orderBy)
~~~~

## Ratelimit服务流量限制

### 常见的流量限制手段

漏桶、令牌桶

1.  漏桶是指我们有一个一直装满了水的桶，每过固定的一段时间即向外漏一滴水。如果你接到了这滴水，那么就可以继续服务请求，如果没有接到，那么就需要等待下一滴水。
2.  令牌桶则是指匀速向桶中添加令牌，服务请求时需要从桶中获取令牌，令牌的数目可以按照需要消耗的资源进行相应的调整。如果没有令牌，可以选择等待或者放弃。

漏桶流出的速率固定，而令牌桶只要在桶中有令牌，那就可以拿。令牌桶在桶中没有令牌的情况下会退化为漏桶模型。实际应用中令牌桶应用较为广泛。

### 原理

令牌桶模型就是对全局计数的加减法操作过程，但使用计数需要加读写锁。考虑到Go中的channel，可以使用buffered channel来完成简单的加令牌取令牌操作。

~~~go
var tokenBucket = make(chan struct{},capacity)
fillToken := func(){
    ticker := time.NewTicker(fillInterval)
    for{
        select{
            case <-ticker.C:
            select{
                case tokenBucket <- struct{}{}:
                default:
            }
            fmt.Println("current token cnt:",len(tokenBucket),time.Now())
        }
    }
}
~~~

取令牌操作：

~~~go
getToken := func(block bool) bool {
		var takenResult bool
		if block {
			select {
			case <-tokenBucket:
				takenResult = true
			}
		} else {
			select {
			case <-tokenBucket:
				takenResult = true
			default:
				takenResult = false
			}
		}
		return takenResult
	}
~~~

但是这种做法比较麻烦。只需要根据时间段和单个时间段插入的令牌数，就可以计算出当前的令牌数。

### 服务瓶颈和QoS

如果一个系统遇到了性能问题，那么监控图的反应一般都是最快的。

虽然性能指标很重要，但对用户提供服务时还应考虑服务整体的`QoS`。`QoS`就是服务质量。`QoS`包含可用性、吞吐量、时延、时延变化和丢失等指标。

## layout常见大型Web项目分层

MVC框架：

1.  控制层（controller)-负责转发请求，对请求进行处理
2.  视图（View）-界面设计人员进行图形界面设计
3.  模型（Model）-程序员编写程序应有的功能（实现算法等）、数据库专家进行数据管理和数据库设计（可以实现具体的功能等）。

现在一般是前后分离的架构。将V从MVC中抽离单独称为项目，前后端之间通过ajax交互。现在比较流行的纯后端API模块一般采用下述划分方法：

1.  Controller，与上述类似，服务入口，负责处理路由，参数校验，请求转发。
2.  Logic/Service，逻辑（服务）层，一般是业务逻辑的入口，可以认为从这里开始，所有请求参数都是合法的。业务逻辑和业务流程都是这一块。常见的设计中会将该层称为Business Rules。
3.  DAO/Repository，这一层主要负责和数据、存储打交道。将下层存储以更简单的函数、接口形式暴露给Logic层来使用。负责数据的持久化工作。

## 接口和表驱动开发

### 使用函数封装业务流程

把相似的行为放在一起，然后打包成一个一个的函数。

### 使用接口来做抽象

在进行抽象之前，应该想明白一点是，引入接口对我们的系统本身是否有意义，这是要按照场景去进行分析的。

### 接口的优缺点

接口设计的正交性，模块之间不需要知晓相互的存在，A模块定义接口，B模块实现这个接口即可。

好处：

1.  依赖反转，这是接口在大多数语言中对软件项目能产生的影响，在Go的正交接口的设计场景下甚至可以去除依赖；
2.  由编译器来帮助我们在编译期就能检查到类似的“未完全实现接口”这样的错误，如果业务未实现某个流程，但又将其实例作为接口强行使用的话，编译不会通过。

接口是一种编译器进行检查的保证类型安全的手段。

在函数中如果有`if`和`switch`的话，会使函数的圈复杂度上升，这时可以用表驱动的方式来存储需要的实例。比如用哈希表。

## 灰度发布和A/B test

灰度发布的实现：

1.  通过分批次部署实现灰度发布
2.  通过业务规划进行灰度发布

在对系统的旧功能进行升级迭代时，第一种方式用的比较多。新功能上线时，第二种方式用的比较多。

### 通过分批次部署实现灰度发布

假如服务部署在15个实例上，把这15个实例分为四组，按照先后顺序，分别有1-2-4-8台机器，保证每次扩展时大概都是二倍的关系。

为什么要用2倍？这样能够保证不管有多少台机器，都不会把组划分得太多。并且上线最开始影响到的用户在整体用户中占的比例也不大。

在上线时，最有效的观察手段是查看程序的错误日志，如果较明显的逻辑错误，一般错误日志的滚动速度都会有肉眼可见的增加。这种错误也可以通过metrics一类的系统上报给公司内的监控系统，所以在上线过程中，也可以通过观察监控曲线，来判断是否有异常发生。

如果有异常情况，首先要做的自然就是回滚。

### 通过业务规划进行灰度发布

#### 可选规则

常见的灰度发布系统会有下列规则提供选择：

1.  按城市发布
2.  按概率发布
3.  按百分比发布
4.  按白名单发布
5.  按业务线发布
6.  按UA(APP、Web、PC)
7.  按分发渠道发布

常见的哈希算法有：md5，sha1和开源的murmur3等，其中murmur3的计算速度较其余两种更快。对于哈希算法来说，除了性能方面的问题，还要考虑哈希后的值是否分布均匀。

# 分布式系统

## 分布式id生成器

snowflake算法是一个典型算法。

首先确定数值是64位，int64类型，被划分为四部分，不含开头的第一个bit，因为这个bit代表符号位。用41位来表示收到请求时的时间戳，单位为毫秒，然后五位来表示数据中心的id，然后再五位来表示机器的实例id，最后是12位的循环自增id（到达1111,1111,1111后归位0）。

这样机制可以支持在同一台机器上，同一毫秒内产生`2^12=4096`条消息。

### worker_id分配

其中时间戳、数据中心id、机器实例id和顺序id：时间戳和顺序id是程序在运行期生成的。但数据中心id和机器实例id需要在部署阶段就能够获取到。并且一旦程序启动之后，就是不可更改的。

一般不同数据中心的机器，会提供对应的获取数据中心id的API，所以数据中心id可以在部署阶段轻松获取到。而机器实例id是逻辑上给机器分配的一个id，一个简单的方法是由能够提供这种自增id功能的工具来支持，比如MySQL。另外可以直接写在配置中，上线时，由部署脚本完成机器实例id字段的替换。

## 分布式锁

### 进程内加锁

使用`sync`包的互斥锁即可

### trylock

在单机系统中，`trylock`并不是一个好选择。因为大量的`goroutine`抢锁可能会导致CPU无意义的资源浪费。

活锁：程序看起来正常运行，但CPU周期被浪费在抢锁，而非执行任务上，从而程序整体的执行效率低下。活锁的问题定位比较麻烦，所以在单机场景下，不建议使用这种锁。

### 基于Redis的setnx

在分布式场景下，也需要这种抢占的逻辑，此时可以使用`Redis`提供的`setnx`命令。

### 基于ZooKeeper

基于`ZooKeeper`的锁与基于`Redis`的锁的不同之处在于`Lock`成功之前会一致阻塞，这与单机场景中的`mutex.lock`很相似

这种分布式的阻塞锁比较适合分布式任务调度场景，但不适合高频次持锁时间短的抢锁场景。

### 基于etcd

类似于`ZooKeeper`。

### 如何选择合适的锁

如果不允许数据丢失，就不要使用`Redis`的`setnx`的简单锁。对锁数据的要求极高的话，那只能使用`etcd`或者`ZooKeeper`这种通过一致性协议保证数据可靠性的锁方案。但是可靠的锁旺旺都是较低的吞吐量和较高的延迟。并且`etcd`和`ZooKeeper`集群是没有办法通过增加节点来提高其性能的。要对其进行横向扩展，只能增加搭建多个集群来支持更多的请求。

## 延时任务系统

有时需要在确定的时间点发布重要公告，或者需要在某个时间点做出动作等。一般有两种思路来解决这个问题：

1.  实现一套类似crontab的分布式定时任务管理系统
2.  实现一个支持定时发送消息的消息队列。

这两种方法的本质都是实现一个定时器。

### 定时器的实现

一般是时间堆和时间轮

#### 时间堆

最常见的时间堆一般用小顶堆实现。对于定时器来说，如果堆顶元素比当前的时间还要打，那么说明堆内所有元素都比当前时间大。定时检查的时间复杂度是`o(1)`。

Go自身的内置定时器就是用时间堆来实现的，不过并没有使用二叉堆，而是使用了扁平一些的四叉堆。

#### 时间轮

用时间轮来实现定时器时，需要定义每一个格子的“刻度”，可以将时间轮想象成一个时钟，中心有秒针顺时针转动。每次转动到一个刻度时，就需要去查看该刻度挂载的任务列表是否有已经到期的任务。

### 任务分发

每当定时任务呗触发之后需要通知用户侧，有两种思路：

1.  将任务被触发的消息封装为一条消息，发往消息队列，由用户侧对消息队列进行监听
2.  对女用户预先配置的回调函数进行调用

## 负载均衡

### 基于洗牌算法的负载均衡

### ZooKeeper集群的随机节点挑选问题

