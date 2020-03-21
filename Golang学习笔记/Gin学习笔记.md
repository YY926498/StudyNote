# Gin学习笔记

## Parameters in path

~~~go
router := gin.router()
//这个处理器匹配/user/yang，但是不匹配/user或者/user/
router.GET("/user/:name/*action", func(c *gin.Context) {
    name := c.Param("name")
    action := c.Param("action")
    message := "get /:name/*action " + name + " is " + action
    c.String(http.StatusOK, message)
    fmt.Println("c.Params:", c.Params)
})
//这个处理器匹配/user/yang/和/user/yang/send,但是其中参数action带/，使用时需要处理

router.POST("/user/:name/*action", func(c *gin.Context) {
    if c.FullPath() == "user/:name/*action" {
        fmt.Println("true", c.Params)
        c.String(http.StatusOK, "true")
    } else {
        fmt.Println("false", c.Params)
        c.String(http.StatusOK, "false")
    }
})
~~~

## Querystring parameters

~~~go
router.GET("/welcome", func(c *gin.Context) {
    //使用DefaultQuery时，如果url中没有相应查询，就会将第二个参数作为默认值
		firstname := c.DefaultQuery("firstname", "Guest")
		lastname := c.Query("lastname")
		c.String(http.StatusOK, "Hello %s %s", firstname, lastname)
	})
~~~

## Mutlipart/Urlencoded Form

~~~go
router.POST("/form_post", func(c *gin.Context) {
		message := c.PostForm("message")
    //同DefaultQuery相同
		nick := c.DefaultPostForm("nick", "annoymous")
		c.JSON(http.StatusOK, gin.H{
			"status":  "posted",
			"message": message,
			"nick":    nick,
		})
		fmt.Println(c.Params, c.Request)
	})
~~~

## Map as querystring or postform parameters

~~~go
//QueryMap解析出URL中以?开头的map，PostFormMap解析出Body中的map
router.POST("/post", func(c *gin.Context) {
    ids := c.QueryMap("ids")
    names := c.PostFormMap("names")
    fmt.Println("ids: %v, names: %v", ids, names)
})
~~~

## Upload file

文件名不该可信。

~~~go
router.POST("/upload", func(c *gin.Context) {
    file, _ := c.FormFile("file")
    log.Println(file.Filename)
    c.String(http.StatusOK, fmt.Sprintf("'%s' upload!", file.Filename))
})
//其中发送的请求头：Content-Type:multipart/form-data
~~~

## Multiple files

~~~go
router.POST("/upload", func(c *gin.Context) {
    form, _ := c.MultipartForm()
    files := form.File["file"]
    for _, file := range files {
        log.Println(file.Filename)
        tempfile, _ := os.OpenFile(file.Filename, os.O_CREATE, 0666)
        tempfile.Close()
        c.SaveUploadedFile(file, "./"+file.Filename)
    }
    c.String(http.StatusOK, fmt.Sprintf("%d files uploaded!", len(files)))
})
~~~

## Grouping routes

~~~go
v1 := router.Group("/v1")
{
    v1.POST("/login", loginEndPoint)
    v1.POST("/submit", submitEndPoint)
}
v2 := router.Group("/v2")
{
    v2.POST("/login", loginEndPoint)
    v2.POST("/submit", submitEndPoint)
}
~~~

## 不带中间件的默认gin

~~~go
router := gin.New()
router.Use(gin.Logger())//全局中间件
router.Use(gin.Recover())
//下面两句同：authorized := r.Group("/",AuthRequired())
authorized := router.Group("/")
authorized.Use(AuthRequired())
~~~

## 写日志文件

~~~go
f, _ := os.Create("gin.log")
//同时写日志到多个文件
gin.DefaultWriter = io.MultiWriter(f, os.Stdout)
router := gin.Default()
router.GET("/ping", func(c *gin.Context) {
    c.String(http.StatusOK, "pong")
})
~~~

## 定制日志格式

~~~go
router := gin.New()
router.Use(gin.LoggerWithFormatter(func(param gin.LogFormatterParams) string {
    return fmt.Sprintf("%s - [%s] \" %s %s %s %d %s \"%s\" %s\"\n",
                       param.ClientIP,
                       param.TimeStamp.Format(time.RFC1123),
                       param.Method,
                       param.Path,
                       param.Request.Proto,
                       param.StatusCode,
                       param.Latency,
                       param.Request.UserAgent(),
                       param.ErrorMessage,
                      )
}))
~~~

## 模型绑定与验证

可以绑定JSON、XML、YAML和标准表单形式。

提供两种设置方法进行绑定：

-   类型-Must bind
    -   方法-`Bind`，`BindJSON`，`BindQuery`，`BindYAML`，`BindHeader`
    -   行为-使用`MustBindWith`如果绑定错误，就会调用`c.AbortWithError(400,err).SetType(ErrorTypeBind)`。这个响应状态是400，并且`Content-Type`头设置为`text/plain;charset=utf-8`。如果之后试图设置响应码，将会导致警告。如果希望有更好的控制，使用`ShouldBind`。
-   类型-Should bind
    -   方法-`ShouldBind`,`ShouldJSON`,`ShouldBindXML`,`ShouldBindQuery`,`ShouldBindYAML``ShouldBindHeader`。
    -   行为-如果绑定发生错误，错误将会返回。

如果已经确保绑定，可以调用`MustBindWith`或者`ShouldBindWith`。如果一个域为`binding:"required"`，有一个空值，将会返回一个错误。如果可以忽略一个绑定，可以使用`binding:"-"`。

~~~go
type Login struct {
	User     string `form:"user" json:"user" xml:"user" binding:"required"`
	Password string `form:"password" json:"password" xml:"password" binding:"-"`
}
router.POST("/loginJSON", func(c *gin.Context) {
    var json Login
    if err := c.ShouldBindJSON(&json); err != nil {
        c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
        return
    }

    if json.User != "manu" || json.Password != "123" {
        c.JSON(http.StatusUnauthorized, gin.H{"status": "unauthorized"})
        return
    }
    c.JSON(http.StatusOK, gin.H{"status": "you are logged in"})
})
router.POST("/loginXML", func(c *gin.Context) {
    var xml Login
    if err := c.ShouldBindXML(&xml); err != nil {
        c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
        return
    }
    if xml.User != "manu" || xml.Password != "123" {
        c.JSON(http.StatusUnauthorized, gin.H{"status": "unauthorized"})
        return
    }
    c.JSON(http.StatusOK, gin.H{"status": "you are logged in"})
})
router.POST("/loginForm", func(c *gin.Context) {
    var form Login
    if err := c.ShouldBind(&form); err != nil {
        c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
        return
    }
    if form.User != "manu" || form.Password != "123" {
        c.JSON(http.StatusUnauthorized, gin.H{"status": "unauthorized"})
        return
    }
    c.JSON(http.StatusOK, gin.H{"status": "you are logged in"})
})
~~~



## 定制验证器

~~~go
type Booking struct {
	CheckIn  time.Time `form:"check_in" binding:"required" time_format:"2006-01-02"`
	CheckOut time.Time `form:"check_out" binding:"required" time_format:"2006-01-02"`
}
var bookableDate validator.Func = func(fl validator.FieldLevel) bool {
	date, ok := fl.Field().Interface().(time.Time)
	if ok {
		today := time.Now()
		if today.After(date) {
			return false
		}
	}
	return true
}
//main
router := gin.Default()
if v, ok := binding.Validator.Engine().(*validator.Validate); ok {
    v.RegisterValidation("bookabledate", bookableDate)
}
router.GET("/bookable", getBookable)
//
func getBookable(c *gin.Context) {
	var b Booking
	if err := c.ShouldBindWith(&b, binding.Query); err == nil {
		c.JSON(http.StatusOK, gin.H{"message": "Booking dates are valid!"})
	} else {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
	}
}
~~~

## 只绑定查询字符串

~~~go
if c.ShouldBindQuery(&person) == nil {
    log.Println("=====Only Bind By Query String=====")
    log.Println(person.Name)
    log.Println(person.Address)
}
~~~

## 绑定查询字符串或者Post Data

~~~go
if c.ShouldBind(&person) == nil {
    log.Println(person)
}
~~~

## 绑定URI

~~~go
type Person struct {
	ID   string `uri:"id" binding:"required,uuid"`
	Name string `uri:"name" binding:"required"`
}
route.GET("/:name/:id", func(c *gin.Context) {
    var person Person
    if err := c.ShouldBindUri(&person); err != nil {
        c.JSON(http.StatusBadRequest, gin.H{"msg": err})
        return
    }
    c.JSON(http.StatusOK, gin.H{"name": person.Name, "uuid": person.ID})
})
~~~

## 绑定头部

~~~go
type TestHeader struct {
	Rate   int    `header:"Rate"`
	Domain string `header:"Domain"`
}
r.GET("/", func(c *gin.Context) {
    h := TestHeader{}
    if err := c.ShouldBindHeader(&h); err != nil {
        c.JSON(http.StatusBadRequest, err)
        return
    }
    fmt.Printf("%#v\n", h)
    c.JSON(http.StatusOK, gin.H{"Rate": h.Rate, "Domain": h.Domain})
})
~~~

## 另外还可以绑定HTML检测

~~~go
type myForm struct{
    Colors []string `form:"colors[]"`
}
func formHandler(c *gin.Context){
    var fakeForm myForm
    c.ShouldBind(&fakeForm)
    c.JSON(200,gin.H{"color":fakeForm.Colors})
}
~~~

~~~html
<form action="/" method="POST">
    <p>
        Check some colors
    </p>
    <label for="red">Red</label>
    <input type="checkbox" name="colors[]" value="red" id="red">
</form>
~~~

## 绑定Multipart/Urlencoded

~~~go
package main

import (
	"mime/multipart"
	"net/http"

	"github.com/gin-gonic/gin"
)

type ProfileForm struct {
	Name   string                `form:"name" binding:"required"`
	Avatar *multipart.FileHeader `form:"avatar" binding:"required"`
	//如果是多文件
	//Avatar []*multipart.FileHeader `form:"avatar" binding:"required"`
}

func main() {
	router := gin.Default()
	router.POST("/profile", func(c *gin.Context) {
		var form ProfileForm
		//可以使用显式绑定声明：c.ShouldBindWith(&form,binding.Form)
		//或者使用自动绑定：c.ShouldBind()
		if err := c.ShouldBind(&form); err != nil {
			c.String(http.StatusBadRequest, "bad request")
			return
		}
		err := c.SaveUploadedFile(form.Avatar, form.Avatar.Filename)
		if err != nil {
			c.String(http.StatusInternalServerError, "unknown error")
			return
		}
		c.String(http.StatusOK, "OK")
	})
	router.Run()
}
~~~

## 提交XML,JSON,YAML和ProtoBuf

~~~go
package main

import (
	"net/http"

	"github.com/gin-gonic/gin/testdata/protoexample"

	"github.com/gin-gonic/gin"
)

func main() {
	r := gin.Default()
	r.GET("/someJSON", func(c *gin.Context) {
		c.JSON(http.StatusOK, gin.H{"message": "hey", "status": http.StatusOK})
	})
	r.GET("/moreJSON", func(c *gin.Context) {
		var msg struct {
			Name    string `json:"user"`
			Message string
			Number  int
		}
		msg.Name = "Yang"
		msg.Message = "hey"
		msg.Number = 13
		c.JSON(http.StatusOK, msg)
	})
	r.GET("/someXML", func(c *gin.Context) {
		c.XML(http.StatusOK, gin.H{"message": "hey", "status": http.StatusOK})
	})
	r.GET("/someYAML", func(c *gin.Context) {
		c.YAML(http.StatusOK, gin.H{"message": "hey", "status": http.StatusOK})
	})
	r.GET("/someProtoBuf", func(c *gin.Context) {
		reps := []int64{int64(1), int64(2)}
		label := "test"
		data := &protoexample.Test{
			Label: &label,
			Reps:  reps,
		}
		c.ProtoBuf(http.StatusOK, data)
	})
	r.Run()
}
~~~

## 安全JSON

使用SecureJSON可以预防JSON劫持，如果结构体是一个数组类型，默认添加前缀`while(1),`

~~~go
/**
* @Author: YangYu
* @Date: 2020/3/16 10:38
* @File: main
* @Software: GoLand
 */
package main

import (
	"net/http"

	"github.com/gin-gonic/gin"
)

func main() {
	r := gin.Default()

	r.GET("/someJSON", func(c *gin.Context) {
		names := []string{"yang", "yu"}
		c.SecureJSON(http.StatusOK, names)
	})
	r.Run()
}

//输出
while(1);[
    "yang",
    "yu"
]
~~~

