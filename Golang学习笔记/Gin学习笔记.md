# Gin学习笔记

## Parameters in path

~~~go
router := gin.router()
//这个处理器匹配/user/yang，但是不匹配/user或者/user/
router.GET("/user/:name",func(c *gin.Context){...})
//这个处理器匹配/user/yang/和/user/yang/send,但是其中参数action带/，使用时需要处理
router.GET("/user/:name/*action", func(c *gin.Context) {...})
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

