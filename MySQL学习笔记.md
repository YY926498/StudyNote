# MySQL学习教程

## 基础概念

**数据库**：保存有组织的数据的容器（通常是一个文件或一组文件）。

**表**：某种特定类型数据的结构化清单。

**表名**：表名的唯一性取决于多个因素，如数据库名或表名等的结合。同一个数据库中不能有相同的表名，但是不同的数据库之间可以有相同的表名。

**模式**：关于数据库和表的布局及特性的信息。

**列**：表中的一个字段。所有表都是由一个或多个列组成。

**数据类型**：所容许的数据的类型。每个表列都有相应的数据类型，它限制（或容许）该列中存储的数据。

**行**：表中的一个记录。

**主键**：一列（或一组列），其值能够唯一区分表中每个行。应该保证每个表中具有一个主键，以便于以后的数据操纵和管理。主键应该满足的要求：

- 任意两行都具有相同的主键值
- 每个行都必须具有一个主键值（主键值不允许NULL值）

如果主键是由多列组成，上述条件应用到构成主键的所有列，所有列值的组合必须是唯一的（但单个列的值可以不唯一）。

主键的最好习惯：

- 不更新主键列中的值
- 不重用主键列的值
- 不在主键列中使用可能会更改的值。

## 检索数据

### **select**：

从一个或多个表中检索信息。必须给出两条信息：

- 想选择什么
- 从什么地方选择

#### 检索单个列

`select  prod_name from products`：利用select语句从products表中检索一个名为prod_name的列。所需的列名在select关键字之后给出，from关键字指处从其中检索数据的表名。

返回的数据不一定是排序后的数据。

结束SQL语句：多条SQL语句必须以分号分割。

SQL语句不区分大小写。但是一个良好的习惯是对SQL关键字大写，对所有列和表名使用小写。并且再次处理SQL语句时，其中所有空格都被忽略，可以分成一行或多行，分成多行容易阅读和调试。

#### 检索多个列

在select关键字之后给出多个列名，列名之间必须以逗号分隔。（在选择多个列时，一定要在列名之间加上逗号，但最后一个列名后不加。如果在最后一个列名后加了逗号，将出现错误）。

~~~mysql
SELECT prod_id,prod_name,prod_price
FROM products;
~~~

这条语句使用SELECT语句从表products中选择数据。指定了3个列名，列名之间用逗号分隔。

数据表示：SQL语句一般返回原始的无格式的数据。表示一般在显示该数据的应用程序中规定。

#### 检索所有列

在实际列名的位置使用星号`*`通配符来达到。

~~~mysql
SELECT *
FROM products;
~~~

如果给定一个通配符`*`，则返回表中的所有列。列的顺序一般是列在表定义中出现的顺序。但有时候不是这样的，由于表的模式的变化可能会导致顺序的变化。

#### 检索不同的行

使用`DISTINCT`关键字，指示MySQL只返回不同的值

~~~mysql
SELECT DISTINCT vend_id
FROM products;
~~~

上述语句告诉MySQL只返回不同（唯一）的vend_id行。如果使用DISTINCT关键字，它必须直接放在列名的前面。

不能部分使用DISTINCT  DISTINCT关键字应用于所有列而不仅是前置它的列。如果给出`SELECT DISTINCT vend_id, prod_price FROM products;`除非指定的两个列的某一行均与其他一行相同，才被认为是相同的行，如果仅是某个列的一行与其他行相同，不能认为重复。

#### 限制结果

为了返回前几行，使用`LIMIT`子句。例：

~~~MYSQL
SELECT prod_name
FROM products
LIMIT 5;
~~~

`LIMIT 5`指示MySQL返回不多于5行。

~~~mysql
SELECT prod_name
FROM products
LIMIT 5,5;
~~~

`LIMIT 5,5`指示MySQL返回从行5开始的5行。第一个数为位置，第二个数为要检索的行数。

因此，`LIMIT`带一个数的，总是从第一行开始，给出的数为返回的行数。带两个值得`LIMIT`可以指定从行号为为第一个值得位置开始。

**行0**：检索出来的第一行为行0而不是行1.因此，`LIMIT 1,1`将检索出第二行而不是第一行。

**行数不够**：`LIMIT`中指定要检索的行数为检索的最大行数。如果没有足够的行，MySQL将只返回它能返回的那么多行。

LIMIT的另一种写法：`LIMIT 4 OFFSET 3`代表从行3开始取4行。就像`LIMIT 3,4`一样。

#### 使用完全限定的表名

使用完全限定的列名

~~~mysql
SELECT products.prod_name FROM products;
~~~

使用完全限定的表名（假定products表位于`crashcourse`数据库中）

~~~mysql
SELECT products.prod_name FROM crashcourse.products;
~~~

---

## 排序检索数据

### 按单列排序

关系数据库设计理论认为，如果不明确规定排序顺序，则不应该假定检索出的数据的顺序有意义。

**子句**：SQL语句由子句构成，有些子句是必需的，而有的是可选的。一个子句通常由一个关键字和所提供的数据组成。

为了明确地排序用`SELECT`语句检索出的数据，可使用`ORDER BY`子句。`ORDER BY`子句取一个或多个列的名字，据此对输出进行排序。

~~~mysql
SELECT prod_name FROM products ORDER BY prod_name;
~~~

通过非选择列进行排序：通常，ORDER BY子句中使用的列将是为显示所选择的列。但是，实际上并不一定要这样，用非检索的列排序数据是完全合法的。

### 按多个列排序

按多个列排序，只要指定列名，列名之间用逗号分开即可，就像选择多个列时一样。

~~~mysql
SELECT prod_id,prod_price,prod_name 
FROM products 
ORDER BY prod_price,prod_name;
~~~

在排序过程中，仅在多个行具有相同的prod_price值时才对产品按prod_name进行排序。如果prod_price列中的所有值都是唯一的，则不会按prod_name排序。

### 指定排序方向

默认的为升序排序，另外可以指定`DESC`关键字，进行降序排序

~~~mysql
SELECT prod_id,prod_price,prod_name
FROM products
ORDER BY prod_price DESC;
~~~

如果对多个列进行排序，每个列排序的方式不同，可采用下列方式：

~~~mysql
SELECT prod_id,prod_name,prod_price
FROM products
ORDER BY prod_price DESC, prod_name;
~~~

先以prod_price降序排序，如果遇到重复的prod_price，再以prod_name升序排序。

DESC关键字只应用到直接位于其前面的列名。如果想对多个列进行降序排序，必须对每个列指定DESC关键字。

与DESC相反的关键字是ASC。但是一般没有什么用处。

利用ORDER BY和LIMIT的组合，能够找出当前排序下的对应行。在给出ORDER BY 子句时，应该保证它位于FROM子句之后。如果使用LIMIT，它必须位于ORDER BY之后。使用子句的次序不对将产生错误信息。

## 过滤数据

### 使用WHERE子句

检索数据时，通常只会根据特定操作或报告的需要提取表数据的子集。只检索所需数据需要指定搜索条件，搜索条件也被称为过滤条件。

在SELECT语句中，数据根据WHERE子句中指定的搜索条件进行过滤。WHERE子句在表名（FROM子句）之后给出，如下所示：

~~~mysql
SELECT prod_name,prod_price
FROM products
WHERE prod_price = 2.50;
~~~

这条语句从products表中检索出两个列，但不返回所有行，只返回prod_price=2.50的行。

WHERE子句的位置：在同时使用ORDER BY和WHERE子句时，应该让ORDER BY位于WHERE之后，否则将会发生错误。

### WHERE子句操作符

~~~mysql
=	等于
<>	不等于
!=	不等于
<	小于
<=	小于等于
>	大于
>=	大于等于
BETWEEN	在指定的两个值之间
~~~

#### 检查单个值

~~~mysql
SELECT prod_anme,prod_price
FROM products
WHERE prod_price <= 10;
~~~

检索价格小于等于10的所有产品

#### 不匹配检查

~~~mysql
SELECT vend_id,prod_name
FROM products
WHERE vend_id <> 1000 
ORDER BY vend_id 
LIMIT 1 OFFSET 0;
~~~

查找vend_id不等于1000的按照vend_id递增排序的第一行。

使用引号：在WHERE子句中可能使用引号，单引号用来限定字符串。如果将值与串类型的列进行比较，则需要限定引号。用来与数值列进行比较的值不用引号。

#### 范围值检查

使用BETWEEN操作符。

~~~mysql
SELECT prod_name,prod_price
FROM products
WHERE prod_price BETWEEN 5 AND 10;
~~~

检索价格在5到10之间。BETWEEN匹配范围中的所有制，包括指定的开始值和结束值。

#### 空值检查

在创建表时，可以指定其中的列是否可以不包含值。在一个列不包含值时，称其为包含空值NULL。

NULL 无值，与字段包含0、空字符串或仅仅包含空格不同。

特殊的WHERE子句，用来检查具有NULL的列。

~~~mysql
SELECT prod_name,prod_price
FROM products
WHERE prod_price IS NULL;
~~~

返回没有价格的所有产品。

​	NULL与不匹配：在通过过滤选择出不具有特定值的行时，可能希望返回具有NULL值得行。但是，不行。因为未知具有特殊的含义，数据库不知道他们是否匹配，所以在匹配过滤或不匹配过滤时不返回它们。

因此，在过滤数据时，一定要验证返回数据中确实给出了被过滤列具有NULL的行。

---

## 数据过滤

组合WHERE子句建立功能更强的更高级的搜索条件。MySQL允许给出多个WHERE子句，这些子句可以以两种方式使用：以AND子句的方式或OR子句的方式使用。

### AND子句：

~~~mysql
SELECT prod_id,prod_price,prod_name
FROM products
WHERE vend_id = 1003 AND prod_price <= 1000;
~~~

返回vend_id=1003且价格小于等于1000的所有产品的id、价格和名称。

### OR子句：

~~~mysql
SELECT prod_name,prod_id
FROM products
WHERE vend_id = 1002 OR vend_id = 1003;
~~~

返回vend_id=1002或者vend_id=1003的商品名称和id。

### 计算次序

~~~mysql
SELECT prod_name,prod_price
FROM products
WHERE vend_id = 1002 OR vend_id = 1003 AND prod_price >=10;
~~~

SQL在处理OR操作符前，优先处理AND操作符。当SQL看到上述WHERE子句时，理解为由vend_id=1003制造的任何价格在10（含）以上的产品，或者由vend_id=1002制造的任何产品，而不管其价格如何。

解决方法是使用圆括号明确地分组相应的操作符。如下：

~~~mysql
SELECT prod_name,prod_price
FROM products
WHERE (vend_id = 1002 OR vend_id = 1003) AND prod_price >= 10;
~~~

圆括号具有较AND和OR操作符高的计算次序。

**在WHERE子句中使用圆括号**：任何时候使用具有AND和OR操作符的WHERE子句，都应该使用圆括号明确地分组操作符。不要过分依赖默认计算次序，即使它确实是你想要的东西也是如此。

### IN操作符

IN操作符用来指定条件范围，范围中的每个条件都可以进行匹配。IN取合法值的由逗号分隔的清单，全都在圆括号中。

~~~mysql
SELECT prod_name, prod_price
FROM products
WHERE vend_id IN (1002,1003)
ORDER BY prod_name;
~~~

此SELECT语句检索vend_id为1002和1003的所有数据。IN操作符后的合法值清单，必须在圆括号中。与OR等价。

IN操作符的优点：

- 在使用长的合法选项清单时，IN操作符的语法更清楚且更直观
- 在使用IN时，计算的次序更容易管理（因为使用的操作符更少）
- IN操作符一般比OR操作符清单执行更快
- IN的最大优点是可以包含其他SELECT语句，使得能够更动态的建立WHERE子句。

### NOT操作符

WHERE子句中的NOT操作符有且只有一个功能，那就是否定它之后所跟的任何条件。

~~~mysql
SELECT prod_name, prod_price
FROM products
WHERE vend_id NOT IN (1002,1003)
ORDER BY prod_name;
~~~

这里的NOT否定跟在它之后的条件，因此，现在匹配vend_id不是1002和1003的所有其他数据。

MySQL中的NOT 支持使用NOT对IN、BETWEEN和EXISTS子句取反。

NOT仅对其后的一个条件有影响，再之后的条件无影响

~~~mysql
SELECT DISTINCT prod_name, prod_price
FROM products
WHERE prod_price NOT IN(5,10) AND prod_price = 9;
~~~

上面的语句是找出价格不等于5和10并且价格等于9的所有数据。

## 用通配符进行过滤

### LIKE操作符

**通配符**：用来匹配值得一部分的特殊字符

**搜索模式**：由字面值、通配符或两者组合构成的搜索条件。

为在搜索子句中使用通配符，必须使用LIKE操作符。LIKE指示MySQL后跟的搜索模式利用通配符匹配而不是直接相等匹配进行比较。

**谓词**：操作符何时不是操作符？答案是它作为谓词时。从技术上说，LIKE是谓词而不是操作符。虽然最终的结果是相同的，但应该对此术语有所了解。

#### 百分号(%)通配符

%表示任何字符出现任意次数。

~~~mysql
SELECT prod_id, prod_name
FROM products
WHERE prod_name LIKE 'jet%';
~~~

使用搜素模式‘jet%’。在执行这条子句时，将检索任何以‘jet’起头的词。‘%’告诉MySQL接受jet之后的任意字符，不管有多少字符。

如果需要区分大小写，需要在MySQL中进行配置，因此搜索可以是区分大小写的。

通配符可以在搜索模式中任意位置使用，并且可以使用多个通配符。

~~~mysql
SELECT prod_id, prod_name
FROM products
WHERE prod_name LIKE '%anvil%';
~~~

搜索模式‘%anvil%’表示匹配任何位置包含文本anvil的值，而不论它之前或之后出现什么字符。

通配符也可以出现在搜索模式的中间，但不太有用。

~~~mysql
SELECT prod_name, prod_price
FROM products
WHERE prod_name LIKE 's%e';
~~~

%可以匹配0个字符、1个字符或多个。

**注意尾空格**：尾空格可能会干扰通配符匹配。一个简单的办法是在搜索模式最后附加一个%，一个更好的办法是使用函数去掉首尾空格。

**注意NULL**：虽然似乎%通配符可以匹配任何东西，但有一个例外，即NULL。即使是WHERE prod_name LIKE '%`也不能匹配用值NULL作为产品名的行。

#### 下划线(_)通配符

下划线的用途与%一样，但下划线只匹配单个字符而不是多个字符。

~~~mysql
SELECT prod_id,prod_name
FROM products
WHERE prod_name LIKE '_ ton anvil'
~~~

此WHERE子句中的搜索模式给出了后面跟有文本的两个通配符。结果只显示匹配搜索模式的行。即必须是如`1 ton anvil`，但是`34 ton anvil`无法匹配到

与%能匹配0个字符不一样，_总是匹配一个字符，不能多也不能少。

### 使用通配符的技巧

- 不要过度使用通配符。如果其他操作符能达到相同的目的，应该使用其他操作符。
- 在确实需要使用通配符时，除非绝对有必要，否则不要把他们用在搜索模式的开始处。把通配符置于搜索模式的开始处，搜索起来是最慢的。
- 仔细注意通配符的位置。如果放错地方，可能不会返回想要的数据。

---

## 用正则表达式进行搜索

正则表达式的作用是匹配文本，将一个模式（正则表达式）与一个文本串进行比较。MySQL用WHERE子句对正则表达式提供了初步的支持，允许指定正则表达式，过滤SELECT检索出的数据。

### 基本字符匹配

~~~mysql
SELECT prod_name, prod_price
FROM products
WHERE prod_name REGEXP '1000'
ORDER BY prod_name;
~~~

除关键字`LIKE`被`REGEXP`替代外，这条语句非常像使用`LIKE`的语句。它告诉`MySQL：REGEXP`后所跟的东西作为正则表达式处理。

~~~mysql
SELECT DISTINCT prod_name,prod_price
FROM products
WHERE prod_name REGEXP '.000'
ORDER BY prod_name;
~~~

这里使用正则表达式`.000`。其中`.`是正则表达式语言中一个特殊的字符。它表示匹配任意一个字符，因此，1000和2000都匹配且返回。

LIKE和REGEXP：在LIKE和REGEXP之间有一个重要的差别。如下：

~~~mysql
SELECT prod_name, prod_price
FROM products
WHERE prod_name LIKE '1000'
ORDER BY prod_name;
~~~

~~~mysql
SELECT prod_name, prod_price
FROM products
WHERE prod_name REGEXP '1000'
ORDER BY prod_name;
~~~

LIKE匹配整个列。如果被匹配的文本在列值中出现，LIKE将不会找到它，相应的行也不会被返回（除非使用通配符）。而REGEXP在列值内进行匹配，如果被匹配的文本在列值中出现，REGEXP将会找到它，相应的行将被返回。这是一个重要的差别。

匹配不区分大小写：正则表达式匹配，如果要区分大小写，使用BINARY关键字，如`WHERE prod_name REGEXP BINARY ‘JetPack .000’`。

### 进行OR匹配

为搜索两个串之一（或者为这个串，或者为另一个串），使用`|`，如下所示：

~~~mysql
SELECT prod_name
FROM products
WHERE prod_name REGEXP '1000|2000'
ORDER BY prod
~~~

语句中使用了正则表达式`1000|2000`。`|`为正则表达式的OR操作符。它表示匹配其中之一，因此1000和2000都匹配并返回。

### 匹配几个字符之一

匹配特定的字符，可以通过指定一组用`[]`括起来的字符来完成。如下：

~~~mysql
SELECT prod_name
FROM products
WHERE prod_name REGEXP '[123] Ton'
ORDER BY prod_name;
~~~

这里，使用了正则表达式`[123] Ton`。`[123]`定义了一组字符，它的意思是匹配1或2或3，因此1 ton 和2 ton和3 ton都匹配且返回。

`[]`是另一种形式的OR语句。事实上，正则表达式`[123]Ton`为`[1|2|3]ton`的缩写。但是如下写法存在一定问题：

~~~mysql
SELECT prod_name
FROM products
WHERE prod_name REGEXP '1|2|3 Ton'
ORDER BY prod_name;
~~~

这时的意思是‘1’或‘2’或‘3 ton’。除非把字符`|`括在一个集合中，否则将应用于整个串。

字符集合也可以被否定，即，它们将匹配除指定字符外的任何东西。为否定一个字符集，在集合开始处放置一个`^`即可。因此，尽管`[123]`匹配字符`1`、`2`或`3`，但`[^123]`却匹配除这些字符外的任何东西。

### 匹配范围

在集合中使用`-`来定义一个范围。`[0-9]`等同于`[0123456789]`。范围不限于完整的集合，`[1-3]`和`[6-9]`也是合法的范围。此外，范围不一定是数值，`[a-z]`匹配任意字母字符。

~~~mysql
SELECT prod_name
FROM products
WHERE prod_name REGEXP '[1-5] Ton'
ORDER BY prod_name;
~~~

匹配1到5。

### 匹配特殊字符

如果要找出`.`字符的值，如下：

~~~mysql
SELECT vend_name
FROM vendors
WHERE vend_name REGEXP '.'
ORDER BY vend_name;
~~~

这并不是期望的输出，`.`匹配任意字符，因此每个行都检索出来，为了匹配特殊字符，必须用‘\\\\’为前导。`\\-`表示查找`-`，`\\.`表示查找`.`。

~~~MYSQL
SELECT vend_name
FROM vendors
WHERE vend_name REGEXP '\\.'
ORDER BY vend_name;
~~~

查找`.`。这就是所谓的转义，正则表达式内具有特殊意义的所有字符都必须以这种方式转义。这包括`.  |  []`以及迄今为止使用过的其他特殊字符。

`\\`也用来引用元字符（具有特殊含义的字符），如下表所示：

| 元字符 |   说明   |
| :----: | :------: |
| `\\f`  |   换页   |
| `\\n`  |   换行   |
| `\\r`  |   回车   |
| `\\t`  |   制表   |
| `\\v`  | 纵向制表 |

**匹配`\`** 为了匹配反斜杠`\`本身，需要使用`\\\`。

**\或\\\?**：多数正则表达式实现使用单个反斜杠转义特殊字符，以便能使用这些字符本身。但MySQL要求两个反斜杠（MySQL自己解释一个，正则表达式库解释一个）。

### 匹配字符类

存在找出经常使用的数字、所有字母字符或所有数字字母字符等的匹配。为更方便的工作，可以使用预定义的字符集，称为字符类。如下表所示：

|      类      |                       说明                        |
| :----------: | :-----------------------------------------------: |
| `[:alnum:]`  |          任意字母和数字(同`[a-zA-Z0-9]`)          |
| `[:alpha:]`  |             任意字符（同`[a-zA-Z]`）              |
| `[:blank:]`  |              空格和制表（同`[\\t]`）              |
| `[:cntrl:]`  |         ASCII控制字符（ASCII 0到31和127）         |
| `[:digit:]`  |               任意数字（同`[0-9]`）               |
| `[:graph:]`  |          与`[:print:]`相同，但不包括空格          |
| `[:lower:]`  |             任意小写字母（同`[a-z]`）             |
| `[:print:]`  |                  任意可打印字符                   |
| `[:punct:]`  |  既不在`[:alnum:]`又不在`[:cntrl:]`中的任意字符   |
| `[:space:]`  | 包括空格在内的任意空白字符(同`[\\f\\n\\r\\t\\v]`) |
| `[:upper:]`  |             任意大写字母（同`[A-Z]`)              |
| `[:xdigit:]` |        任意十六进制数字（同`[a-fA-F0-9]`）        |

### 匹配多个实例

有时需要对匹配的数目进行更强的控制。可用下列的正则表达式重复元字符来完成：

| 元字符  |             说明             |
| :-----: | :--------------------------: |
|   `*`   |        0个或多个匹配         |
|   `+`   | 1个或多个匹配（等于`{1,}`）  |
|   `?`   | 0个或1个匹配（等于`{0,1}`）  |
|  `{n}`  |        指定数目的匹配        |
| `{n,}`  |     不少于指定数目的匹配     |
| `{n,m}` | 匹配数目的范围（m不超过255） |

例子：

~~~MYSQL
SELECT prod_name
FROM products
WHERE prod_name REGEXP '\\([0-9] sticks?\\)'
ORDER BY prod_name;
~~~

正则表达式`\\([0-9] sticks?\\)`  解释：`\\(`匹配`(`，`[0-9]`匹配任意数字，`sticks?`匹配`stick`和`sticks`（`s`后的`?`是`s`可选，因为`?`匹配它前面的任何字符的0次或1次出现，`\\)`匹配`)`。没有`?`，匹配stick和sticks会非常困难。

~~~mysql
SELECT prod_name
FROM products
WHERE prod_name REGEXP '[[:digit:]]{4}'
ORDER BY prod_name;
~~~

`[:digit:]`匹配任意数字，因而它为数字的一个集合。`{4}`确切地要求它前面的额字符（任意数字）出现4次，所以`[[:digit:]]{4}`匹配连在一起的任意4位数字。

需要注意的是，在使用正则表达式时，编写某个特殊的表达式几乎总是不止有一种方法。上面的例子也可以如下编写：

~~~mysql
SELECT prod_name
FROM products
WHERE prod_name REGEXP '[0-9][0-9][0-9][0-9]'
ORDER BY prod_name;
~~~

### 定位符

为了匹配特定位置的文本，需要使用下表列出的定位符：

|  元字符   |    说明    |
| :-------: | :--------: |
|    `^`    | 文本的开始 |
|    `$`    | 文本的结尾 |
| `[[:<:]]` |  词的开始  |
| `[[:>:]]` |  词的结尾  |

如果想找出一个数（包括以小数点开始的数）开始的所有产品，简单搜索`[0-9\\.]`（或`[[:digit:]\\.]`）不行，因为它将在文本内任意位置查找匹配。解决方法是使用`^`定位符，如下所示：

~~~mysql
SELECT prod_name
FROM products
WHERE prod_name REGEXP '^[0-9\\.]'
ORDER BY prod_name;
~~~

`^`匹配串的开始。因此`^[0-9\\.]`只在`.`或任意数字为串中第一个字符时才匹配它们。没有`^`，则还要多检索出那些中间有数字的行。

**`^`的双重用途**：`^`有两种用法。在集合中（用`[`和`]`定义），用它来否定该集合，否则，用来指串的开始处。

**是REGEXP起类似LIKE的作用**：LIKE和REGEXP的不同在于，LIKE匹配整个串而REGEXP匹配子串。利用`^`开始每个表达式，用`$`结束每个表达式，可以使REGEXP的作用与LIKE一样。

---

## 创建计算字段

计算字段并不实际存在于数据库表中。计算字段是运行在SELECT语句内创建的。

**字段**：基本上与列的意思相同，经常互换使用，不过数据库一般称为列，而术语字段通常用在计算字段的连接上。

**客户机与服务器的格式**：可在SQL语句内完成的许多转换和格式化的工作都可以直接在客户机应用程序内完成。但一般来说，在数据库服务器上完成这些操作比在客户机中完成要快得多。

### 拼接字段

**拼接**：将值联结到一起构成单个值

在SELECT语句中，可使用`Concat()`函数来拼接两个列。

**MySQL的不同之处**：多数DBMS使用`+`或`||`来实现拼接，MySQL则使用`Concat()`函数来实现。当把SQL语句转换成MySQL语句时一定要把这个区别铭记在心。

~~~mysql
SELECT Concat(vend_name,'(',vend_country,')')
FROM vendors
ORDER BY vend_name;
~~~

**Concat()拼接串**：即把多个串连接起来形成一个较长的串。Concat()需要一个或多个指定的串，各个串之间用逗号分隔。

上面的SELECT语句连接以下4个元素：

- 存储在vend_name列中的名字
- 包含一个空格和一个左圆括号的串
- 存储在vend_country列中的国家
- 包含一个右圆括号的串。

### 删除空格

~~~mysql
SELECT Concat(RTrim(vend_name),'(',RTrim(vend_country),')')
FROM vendors
ORDER BY vend_name;
~~~

**RTrim()函数**：去掉右边的所有空格。

**Trim函数**：MySQL除了支持`RTrim()`还支持`LTrim()`以及`Trim()`（去掉左右两边的空格）。

### 使用别名

通过计算后的列是没有名字的，只是一个值。但是未命名的列不能在客户机中使用，因此SQL支持列别名。别名(alias)是一个字段或值得替换名。别名用AS关键字赋予。

~~~mysql
SELECT Concat(RTrim(vend_name),'(',RTrim(vend_country),')') AS
vend_title
FROM vendors
ORDER BY vend_name;
~~~

SELECT语句本身与以前使用的相同，只不过这里的语句中计算字段之后跟了文本AS vend_title。它指示SQL创建一个包含指定计算的名为vend_title的计算字段。任何客户机可以按名引用这个列，就像它是一个实际的表列一样。

**别名的其他用途**：别名还有其他的用途。常见的用途包括在实际的表列名包含不符合规定的字符时重新命名它，在原来的名字含混或容易误解时扩充它，等等。

**导出列**：别名有时也被称为导出列，不管称为什么，他们所表示的都是相同的东西。

### 执行算术运算

~~~mysql
SELECT prod_id,quantity,item_price,
		quantity*item_price AS expand_price
FROM orderitems
WHERE order_num=20005;
~~~

输出中显示的expand_price列为一个计算字段，此计算为quantity*item_price。客户机现在可以使用这个新计算列，就像使用其他列一样。

MySQL支持下表中所列出的基本算术操作符。此外，圆括号可用来区分优先顺序。

| 操作符 | 说明 |
| :----: | :--: |
|  `+`   |  加  |
|  `-`   |  减  |
|  `*`   |  乘  |
|  `/`   |  除  |

**如何测试计算**： `SELECT`提供了测试和试验函数与计算的一个很好的办法。虽然`SELECT`通常用来从表中检索数据，但可以省略`FROM`子句以便简单地访问和处理表达式。例如，` SELECT 3*2;`将返回6， `SELECT Trim('abc');`将返回abc，而`SELECT Now();`利用`Now()`函数返回当前日期和时间。通过这些例子，可以明白如何根据需要使用`SELECT`进行试验。 

----

## 使用数据处理函数

### 函数

#### 文本处理函数

~~~mysql
SELECT vend_name, Upper(vend_name) AS vend_name_upcase
FROM vendors
ORDER BY vend_name;
~~~

Upper()将文本转换为大写。

常用的文本处理函数：

|    函数     |        说明        |
| :---------: | :----------------: |
|   Left()    |  返回串左边的字符  |
|  Length()   |    返回串的长度    |
|  Locate()   | 找出串中的一个子串 |
|   Lower()   |   将串转换为小写   |
|   LTrim()   |  去掉串左边的空格  |
|   Right()   |  返回串右边的字符  |
|   RTrim()   |  去掉串右边的空格  |
|  Soundex()  | 返回串的SOUNDEX值  |
| SubString() |   返回子串的字符   |
|   Upper()   |   将串转换为大写   |

SOUNDEX是一个将任何文本串转换为描述其语音表示的字母数字模式的算法。soundex考虑了类似的发音字符和音节，使得能够对串进行发音比较而不是字母比较。

#### 日期和时间处理函数

日期和时间采用相应的数据类型和特殊的格式存储，以便能快速和有效地排序或过滤，并且节省物理存储空间。 

常用的日期和时间处理函数

|     函数      |             说明             |
| :-----------: | :--------------------------: |
|   AddDate()   |   增加一个日期（天、周等）   |
|   AddTime()   |   增加一个时间（时、分等）   |
|   CurDate()   |         返回当前日期         |
|   CurTime()   |         返回当前时间         |
|    Date()     |    返回日期时间的日期部分    |
|  DateDiff()   |       计算两个日期之差       |
|  Date_Add()   |     高度灵活的日期运算函     |
| Date_Format() | 返回一个格式化的日期或时间串 |
|     Day()     |     返回一个日期的天数部     |
|  DayOfWeek()  | 对于一个日期，返回对应的星期 |
|    Hour()     |     返回一个时间的小时部     |
|   Minute()    |     返回一个时间的分钟部     |
|    Month()    |     返回一个日期的月份部     |
|     Now()     |       返回当前日期和时       |
|   Second()    |      返回一个时间的秒部      |
|    Time()     |  返回一个日期时间的时间部分  |
|    Year()     |    返回一个日期的年份部分    |

首先需要注意的是MySQL使用的日期格式。无论你什么时候指定一 个日期，不管是插入或更新表值还是用WHERE子句进行过滤，日期必须为格式`yyyy-mm-dd`。因此， 2005年9月1日，给出为2005-09-01。虽然其他的日期格式可能也行，但这是首选的日期格式，因为它排除了多义性（如，
04/05/06是2006年5月4日或2006年4月5日或2004年5月6日或……）。 

~~~mysql
SELECT cust_id,order_num
FROM orders
WHERE Date(order_data) = '2005-09-01';
~~~

查找下单日期为'2005-09-01'。

~~~mysql
SELECT cust_id, order_num
FROM orders
WHERE Data(order_date) BETWEEN '2005-09-01' AND '2005-09-30';
~~~

其中， BETWEEN操作符用来把2005-09-01和2005-09-30定义为一个要匹配的日期范围。 

~~~mysql
SELECT cust_id, order_num
FROM orders
WHERE Year(order_date) = 2005 AND Month(order_date) = 9;
~~~

Year()是一个从日期（或日期时间）中返回年份的函数。类似，Month()从日期中返回月份。因此， WHERE Year(order_date)  = 2005 AND Month(order_date) = 9检索出order_date为2005年9月的所有行 。

#### 数值处理函数

数值处理函数仅处理数值数据。这些函数一般主要用于代数、三角或几何运算，因此没有串或日期—时间处理函数的使用那么频繁。 

|  函数  |        说明        |
| :----: | :----------------: |
| Abs()  | 返回一个数的绝对值 |
| Cos()  |  返回一个角度的余  |
| Exp()  | 返回一个数的指数值 |
| Mod()  |  返回除操作的余数  |
|  Pi()  |     返回圆周率     |
| Rand() |   返回一个随机数   |
| Sin()  | 返回一个角度的正弦 |
| Sqrt() | 返回一个数的平方根 |
| Tan()  | 返回一个角度的正切 |

---

## 汇总数据

### 聚集数据

**聚集函数**：运行在行组上，计算和返回单个值得函数。

|  函数   |       说明       |
| :-----: | :--------------: |
|  AVG()  | 返回某列的平均值 |
| COUNT() |  返回某列的行数  |
|  MAX()  | 返回某列的最大值 |
|  MIN()  | 返回某列的最小值 |
|  SUM()  |  返回某列值之和  |

#### AVG()函数

通过对表中行数计数并计算特定列值之和，求得该列的平均值。AVG()可用来返回所有列的平均值，也可用来返回特定列或行的平均值。

~~~mysql
SELECT AVG(prod_price) AS avg_price
FROM products;
~~~

此SELECT语句返回值avg_price，它包含products表中所有产品的平均价格。

AVG()也可以用来确定特定列或行的平均值。如下：

~~~mysql
SELECT AVG(prod_price) AS avg_price
FROM products
WHERE vend_id = 1003;
~~~

这条SELECT语句与之前一条的不同之处在于它包含了WHERE子句。此WHERE子句仅过滤出vend_id为1003的产品，因此avg_price返回的值只是该供应商的产品的平均值。

**只用于单个列**：AVG()函数只能用来确定特定数值列的平均值，而且列名必须作为函数参数给出。为了获得多个列的平均值，必须使用多个AVG()函数。

**NULL值**：AVG()函数忽略列值为NULL的行。

#### COUNT()函数

两种使用方式：

- 使用COUNT(*)对表中行的数目进行计数，不管表列中包含的是空值（null）还是非空值
- 使用COUNT(column)对特定列中具有值得行进行计数，忽略NULL值

~~~mysql
SELECT COUNT(*) AS num_cust
FROM customers;
~~~

在此例中，利用COUNT(*)对所有行进行计数，不管行中各列有什么值。计数值在num_cust中返回。

~~~mysql
SELECT COUNT(cust_email) AS num_cust
FROM customers;
~~~

这条SELECT语句使用COUNT(cust_email)对cust_email列中有值得行进行计数。

**NULL值**：如果指定列名，则指定列的值为空的行被COUNT()函数忽略，但如果COUNT()函数中用的是星号（*），则不忽略。

#### MAX()函数

MAX()函数返回的是指定列中的最大值。MAX()要求指定列名。

~~~mysql
SELECT MAX(prod_price) AS max_price
FROM products;
~~~

这里，MAX()返回的是products表中最贵的物品的价格。

**对非值数据使用MAX()**：虽然MAX()一般用来找出最大数值或日期值，但MySQL允许将它用来返回任意列中的最大值，包括返回文本列中的最大值。在用于文本数据时，如果数据按相应的列排序，则MAX()返回最后一行。

**NULL值**：MAX函数忽略列值为NULL的行。

#### MIN()函数

与MAX()函数相似。

#### SUM函数

用来返回指定列值得和。

~~~mysql
SELECT SUM(quantity) AS items_ordered
FROM orderitems
WHERE order_num = 20005;
~~~

函数SUM(quantity)返回订单中所有物品数量之和，WHERE子句保证只统计某个物品订单中的物品。

SUM()也可以用来合计计算值。

~~~mysql
SELECT SUM(item_price*quantity) AS total_price
FROM orderitems
WHERE order_num = 20005;
~~~

函数SUM(item_price*quantity)返回订单中所有物品价钱之和，WHERE子句同样保证只统计某个物品订单中的物品。

**在多个列上进行计算**：利用标准的算术操作符，所有聚集函数都可用来执行多个列上的计算。

#### 聚集不同值

以上5个聚集函数都可以如下使用：

- 对所有的列执行计算，指定ALL参数或不给参数（ALL默认）
- 只包含不同的值，指定DISTINCT参数。

~~~mysql
SELECT AVG(DISTINCT prod_price) AS avg_price
FROM products
WHERE vend_id = 1003;
~~~

注：对于COUNT()函数，DISTINCT不能用于COUNT(*)，即COUNT(DISTINCT)，这样会产生错误。类似地，DISTINCT必须使用列名吗，不能用于计算或表达式。

DISTINCT用于MIN或MAX无意义。

#### 组合聚集函数

~~~MYSQL
SELECT COUNT(*) AS num_items,
	   MIN(prod_price) AS price_min,
	   MAX(prod_price) AS price_max,
	   AVG(prod_price) AS price_avg
FROM products;
~~~

这里用单挑SELECT语句执行了4个聚集计算，返回4个值。

---

## 分组数据

### 数据分组

~~~mysql
SELECT COUNT(*) AS num_prods
FROM products
WHERE vend_id = 1003;
~~~

上面是返回每供应商1003提供的产品数量。

### 创建分组

分组是在SELECT语句的GROUP BY子句中建立的。如下：

~~~mysql
SELECT vend_id, COUNT(*) AS num_prods
FROM products
GROUP BY vend_id;
~~~

上面的SELECT语句指定了两个列，vend_id包含产品供应商的ID，num_prods为计算字段（用COUNT(*)函数建立）。GROUP BY子句指示MySQL按vend_id排序并分组数据。这导致对每个vend_id而不是整个表计算num_prods一次。

因为使用了GROUP BY，就不必指定要计算和估值的每个组。系统会自动完成。GROUP BY 子句指示MySQL分组数据，然后对每个组而不是整个结果进行聚集。

使用GROUP BY子句前，需要知道如下重要的规定：

- GROUP BY 子句可以包含任意数目的列。这使得能对分组进行嵌套，为数据分组提供更细致的控制。
- 如果在GROUP BY子句中嵌套了分组，数据将在最后规定的分组上进行汇总。换句话说，在建立分组时，指定的所有列都一起计算（所以不能从个别列取回数据）。
- GROUP BY子句中列出的每个列都必须是检索列或有效的表达式（但不能是聚集函数）。如果在SELECT中使用了表达式，则必须在GROUP BY子句中指定相同的表达式。不能使用别名。
- 除聚集计算语句外，SELECT语句中的每个列都必须在GROUP BY子句中给出。
- 如果分组列中具有NULL值，则NULL将作为一个分组返回。如果列中有多行的NULL值，他们将分为一组。
- <u>GROUP BY 子句必须出现在WHERE子句之后，ORDER BY子句之前</u>。

**使用ROLLUP**：使用WITH ROLLUP关键字，可以得到每个分组以及每个分组汇总级别（针对每个分组）的值，如下所示：

~~~mysql
SELECT vend_id, COUNT(*) AS num_prods
FROM products
GROUP BY vend_id WITH ROLLUP;
~~~

### 过滤分组

除了使用GROUP BY分组数据外，MySQL还允许过滤分组，规定包括哪些分组，排除哪些分组。WHERE过滤指定的是行而不是分组。达成这个目标可以采用HAVING子句。与WHERE子句类似。唯一的差别是WHERE子句过滤行，HAVING子句过滤分组。

**HAVING支持所有WHERE操作符**：所有的WHERE子句的条件（包括通配符条件和带多个操作符的子句）。君可用于HAVING子句。

~~~mysql
SELECT cust_id, COUNT(*) AS orders
FROM orders
GROUP BY cust_id
HAVING COUNT(*) >= 2;
~~~

这条SELECT语句的前3行类似于上面的语句。最后一行增加了HAVING子句，它过滤COUNT(*)>=2的那些分组。

这里WHERE子句不起作用，因为过滤是基于分组聚集值而不是特定行值。

**HAVING和WHERE的差别**：WHERE在数据分组前进行过滤，HAVING在数据分组后进行过滤。这是一个重要的区别，WHERE排除的行不包括在分组中。这可能会改变计算值，从而影响HAVING子句中基于这些值过滤掉的分组。

~~~mysql
SELECT vend_id,COUNT(*) AS num_prods
FROM products
WHERE prod_price >= 10
GROUP BY vend_id
HAVING COUNT(*) >= 2;
~~~

第一行是使用了聚集函数的基本SELECT，它与前面的例子很相像。WHERE子句过滤所有prod_price至少为10的行。然后按vend_id分组数据。HAVING子句过滤计数为2或2以上的分组。

### 分组和排序

虽然GROUP BY和ORDER BY经常完成相同的工作，但他们是非常不同的。如下表所示：

|                   ORDER BY                   |                         GROUP BY                         |
| :------------------------------------------: | :------------------------------------------------------: |
|                排序产生的输出                |             分组行。但输出可能不是分组的顺序             |
| 任意列都可以使用（甚至非选择的列也可以使用） | 只可能使用选择列或表达式列，而且必须使用每个选择列表达式 |
|                  不一定需要                  |     如果与聚集函数一起使用列（或表达式），则必须使用     |

第一项差别极为重要。

**不要忘记ORDER BY**：一般在使用GROUP BY 子句时，应该也给出ORDER BY子句。这是保证数据正确排序的唯一方法。千万不要仅依赖GROUP BY排序数据。

~~~mysql
SELECT order_num, SUM(quantity*item_price) AS ordertotal
FROM orderitems
GROUP BY order_num
HAVING SUM(quantity*item_price) >=50;
~~~

上面的输出可能是乱序的，如果要排序输出，应该如下：

~~~mysql
SELECT order_num, SUM(quantity*item_price) AS ordertotal
FROM orderitems
GROUP BY order_num
HAVING SUM(quantity*item_price) >= 50
ORDER BY ordertotal;
~~~

这个例子，GROUP BY子句用来返回订单号(order_num列)分组数据，以便SUM(*)函数能够返回总计订单价格。HAVING子句过滤数据，使得只返回总计订单价格大于等于50的订单。最后，ORDER BY子句排序输出。

### SELECT子句顺序

具体的子句有：

|   子句   |        说明        |      是否必须使用      |
| :------: | :----------------: | :--------------------: |
|  SELECT  | 要返回的列或表达式 |           是           |
|   FROM   |  从中检索数据的表  | 仅在从表选择数据时使用 |
|  WHERE   |      行级过滤      |           否           |
| GROUP BY |      分组说明      | 仅在按组计算聚集时使用 |
|  HAVING  |      组级过滤      |           否           |
| ORDER BY |    输出排序顺序    |           否           |
|  LIMIT   |    要检索的行数    |           否           |

---

## 使用子查询

**查询**：任何SQL语句都是查询。但此术语一般指SELECT语句。

**子查询**：嵌套在其他查询中的查询。

### 利用子查询进行过滤

例子：现在需要列出订购物品TNT2的所有客户，步骤如下：

- 检索所有包含物品TNT2的所有订单的编号
- 检索具有前一步骤列出的订单编号的所有客户的ID
- 检索前一步骤返回的所有客户ID的客户信息

第一条SELECT语句查找prod_id为TNT2的所有订单物品

~~~mysql
SELECT order_num
FROM orderitems
WHERE prod_id = 'TNT2';
~~~

下一步，查询上条语句返回的订单号，假如orde_num为20005和20007。

~~~mysql
SELECT cust_id
FROM orders
WHERE order_num IN (20005,20007);
~~~

得到对应的客户ID。

现在使用子查询，如下：

~~~mysql
SELECT cust_id
FROM orders
WHERE order_num IN (SELECT order_num 
                    FROM orderitems
                   WHERE prod_id = 'TNT2');
~~~

在SELECT语句中，子查询总是从内向外处理。在处理上面的SELECT语句时，MySQL实际上执行了两个操作。

首先，它执行了下面的查询：

~~~mysql
SELECT order_num FROM orderitems WHERE prod_id = 'TNT2';
~~~

此查询返回两个订单号20005和20007.然后这两个值以IN操作符要求的逗号分隔的格式传递给外部查询的WHERE子句。外部查询变成：

~~~mysql
SELECT cust_id FROM orders WHERE  order_num IN (20005,20007);
~~~

这样的结果与硬编码的WHERE子句所返回的值相同。

**格式化SQL**：包含子查询的SELECT语句难以阅读和调试，特别是较为复杂时更是如此。如上所示把子查询分解为多行并且适当地进行缩进，能极大地简化子查询的使用 。

**列必须匹配**：在WHERE子句中使用子查询（如这里所示），应该保证SELECT语句具有与WHERE子句中相同数目的列。通常，子查询将返回单个列并且与单个列匹配，但如果需要也可以使用多个列 。

虽然子查询一般与IN操作符结合使用，但也可以用于测试等于（=）、不等于（<>）等。 

**子查询和性能**：这里给出的代码有效并获得所需的结果。但是，使用子查询并不总是执行这种类型的数据检索的最有效的方法。 

### 作为计算字段使用子查询

使用子查询的另一方法是创建计算字段。假如需要显示customers表中每个客户的订单总数。订单与相应的客户ID存储在orders表中。 

为了执行这个操作，遵循下面的步骤

- 从customers表中检索客户列表
- 对于检索出的每个客户，统计其在orders表中的订单数目

如下，对客户10001的订单进行计数

~~~mysql
SELECT COUNT(*) AS orders
FROM orders
WHERE cust_id = 10001;
~~~

对每个客户执行COUNT(*)计算，应该将COUNT(*)作为一个子查询。

~~~mysql
SELECT cust_name,
	   cust_state,
	   (SELECT COUNT(*)
         FROM orders
         WHERE orders.cust_id = customers.cust_id) AS orders
FROM customers
ORDER BY cust_name;
~~~

这条SELECT语句对customers表中每个客户返回3列：cust_name、cust_state和orders。orders是一个计算字段，它是由圆括号中的子查询建立的。该子查询对检索出的每个客户执行一次。

子查询中的WHERE子句与前面使用的WHERE子句稍有不同，因为它使用了完全限定列名。下面的语句告诉SQL比较orders表中的cust_id与当前正从customers表中检索的cust_id:

~~~MYSQL
WHERE orders.cust_id = customers.cust_id;
~~~

**相关子查询**：涉及外部查询的子查询

任何时候只要列名可能有多义性，就必须使用这种语法（表名和列名由一个句点分隔）。如果不使用完全限定的列名时，如下：

~~~mysql
SELECT cust_name,
       cust_state,
       (
           SELECT COUNT(*)
           FROM orders
           WHERE cust_id = cust_id
       ) AS orders
FROM customers
ORDER BY cust_name;
~~~

这种语句是错误的。有两个cust_id列，一个在customers中，另一个在orders中，需要比较这两个列以正确地把订单与他们相应的顾客匹配。如果不完全限定列名，MySQL将假定你是对orders表中的cust_id进行自身比较。而SELECT COUNT(*) FROM orders WHERE cust_id = cust_id;总是返回orders表中的订单总数（因为MySQL查看每个订单的cust_id是否与自身匹配，当然，它们总是匹配的）。

**逐渐增加子查询来建立查询**：用子查询测试和调试查询很有技巧性，特别是在这些语句的复杂性不断增加的情况下更是如此。用子查询建立（和测试）查询的最可靠的方法是逐渐进行。首先，建立和测试最内层的查询。然后，用硬编码数据建立和测试外层查询，并且仅在确认它正常后才嵌入子查询。这时，再次测试它。对于要增加的每个查询，重复这些步骤。这样做仅给构造查询增加了一点点时间，但节省了以后（找出查询为什么不正常）的大量时间，并且极大地提高了查询一开始就正常工作的可能性。

---

## 联结表

SQL最强大的功能之一就是能在数据检索查询的执行中联结（join）表。联结是利用SQL的SELECT能执行的最重要的操作，很好地理解联结及其语法是学习SQL的一个极为重要的组成部分。

### 关系表

关系表的设计就是要保证把信息分解成多个表，一类数据一个表。各表通过某些常用的值（即关系设计中的关系）互相关联。

**外键**：外键为某个表中的一列，它包含另一个表的主键值，定义了两个表之间的关系。

这样做的好处如下：

- 信息不重复，从而不浪费时间和空间
- 如果某一个信息变动，可以只更新对应表的单个记录，相关表中的数据不用改动
- 由于数据无重复，显然数据是一致的，这使得处理数据不用改动

**可伸缩性**：能够适应不断增加的工作量而不失败。设计良好的数据库或应用程序称之为可伸缩性好。

用联结的理由：如果数据存储在多个表中，用单条SELECT语句检索出数据，此时可以使用联结。联结是一种机制，用来在SELECT语句中关联表，因此称之为联结。使用特殊的语法，可以联结多个表返回一组输出，联结在运行时关联表中正确的行。

**维护引用完整性**：联结不是物理实体。它在实际的数据库表中不存在。联结由MySQL根据需要建立，它存在于查询的执行当中。在使用关系表时，仅在关系列中插入合法的数据非常重要。这就是维护引用完整性，它是通过在表的定义中指定主键和外键来实现的。

### 创建联结

联结的创建非常简单，规定要联结的所有表以及它们如何关联即可。如下：

~~~mysql
SELECT vend_name, prod_name, prod_price
FROM vendors, products
WHERE vendors.vend_id = products.vend_id
ORDER BY vend_name,prod_name;
~~~

此处，SELECT语句与前面的所有语句一样指定要检索的列。这里，最大的差别是所指定的两个列（prod_name和prod_price）在一个表中，而另一个列（vend_name）在另一个表中。

这里的FROM子句列出了两个表，分别是vendors和products。它们就是这条SELECT语句联结的两个表的名字。这两个表用WHERE子句正确联结，WHERE子句指示MySQL匹配vendors表中的vend_id和products表中的vend_id。这里需要完全限定列名，因为如果只给出vend_id，则MySQL不知道指的是哪一个。

**完全限定列名**：在引用的列可能出现二义性时，必须使用完全限定列名（用一个点分隔的表名和列名）。如果引用一个没有用表名限制的具有二义性的列名，MySQL将返回错误。

#### WHERE子句的重要性

在一条SELECT语句中联结几个表时，相应的关系是在运行中构造的。在数据库表的定义中不存在能指示MySQL如何对表进行联结的东西。在联结两个表时，实际上做的是将第一个表中的每一行与第二个表中的每一行配对。WHERE子句作为过滤条件，它只包含匹配给定条件（这里是联结条件）的行。没有WHERE子句，第一个表中的每一行将与第二个表中的每个行配对，而不管它们逻辑上是否可以配在一起。

**笛卡尔积**：由没有哦联结条件的表关系返回的结果是笛卡尔积。检索出的行的数目将是第一个表中的行数乘以第二个表中的行数。

**不要忘了WHERE子句**：应该保证所有连接都有WHERE子句，否则MySQL将返回比想要的数据多得多的数据。同理，应该保证WHERE子句的正确性。不正确的过滤条件将导致MySQL返回不正确的数据。

**叉联结**：返回称为叉联结的笛卡尔积的联结类型。

### 内部联结

之前使用的联结称为等值联结，它基于两个表之间的相等测试。这种联结也称为内部联结。对于这种联结可以使用稍微不同的语法来明确指定联结的类型。

~~~mysql
SELECT vend_name,prod_name, prod_price
FROM vendors INNER JOIN products
ON vendors.vend_id = products.vend_id;
~~~

这里，两个表之间的关系是FROM子句的组成部分，以INNER JOIN指定。在使用这种语法时，联结条件用特定的ON子句而不是WHERE子句给出。传递给ON的实际条件与传递给WHERE的相同。

**使用哪种语法**：ANSI SQL规范首选INNER JOIN语法。此外，尽管使用WHERE子句定义联结的确比较简单，但是使用明确的联结语法能够确保不会忘记联结条件，有时候这样做也能影响性能。

### 联结多个表

SQL对一条SELECT语句中可以联结的表的数目没有限制。创建联结的基本规则也相同。首先列出所有表，然后定义表之间的关系。例如：

~~~mysql
SELECT prod_name,vend_name, prod_price,quantity
FROM orderitems,products,vendors
WHERE products.vend_id = vendors.vend_id 
AND orderitems.prod_id = products.prod_id
AND order_num = 20005;
~~~

此例子显示编号为20005的订单中的物品。订单物品存储在orderitems表中。每个产品按其产品ID存储，它引用products表中的产品。这些产品通过供应商ID联结到vendors表中相应的供应商，供应商ID存储在每个产品的记录中。这里的FROM子句列出了3个表，而WHERE子句定义了这两个联结条件，而第三个联结条件用来过滤出订单20005中的物品。

**性能考虑**：MySQL在运行时关联指定的每个表以处理联结。这种处理可能是非常耗费资源的，因此应该仔细，不要联结不必要的表。联结的表越多，性能下降越厉害。

之前的子查询例子：

~~~mysql
SELECT cust_name, cust_contact
FROM customers
WHERE cust_id IN(SELECT cust_id
                 FROM orders
                 WHERE order_num IN(SELECT order_num
                                    FROM orderitems
                                    WHERE prod_id = 'TNT2'));
~~~

子查询并不总是执行复杂SELECT操作的最有效的方法，下面使用联结方式查询：

~~~mysql
SELECT cust_name, cust_contact
FROM custorms, orders, orderitems
WHERE customers.cust_id = orders.cust_id
AND orderitems.order_num = orders.order_num
AND prod_id = 'TNT2';
~~~

这个查询中返回数据需要使用3个表。但这里不使用嵌套子查询，而是使用两个联结。前两个关联联结中的表，后一个过滤产品TNT2的数据。

联结是SQL中最重要最强大的特性，有效地使用联结需要对关系数据库设计有基本的了解。

---

## 创建高级联结

### 使用表别名

之前一个例子如下：

~~~mysql
SELECT Concat(RTrim(vend_name),'(',RTrim(vend_country),')') AS
vend_title
FROM vendors
ORDER BY vend_name;
~~~

别名除了用于列名和计算字段外，SQL还允许给表名起别名。理由如下：

- 缩短SQL语句
- 允许在单条SELECT语句中多次使用相同的表

例子如下：

~~~mysql
SELECT cust_name, cust_contact
FROM customers AS c, orders AS o, orderitems AS oi
WHERE c.cust_id = o.cust_id
AND oi.order_num = o.order_num
AND prod_id = 'TNT2';
~~~

可以看到，FROM子句中的3个表全都具有别名。customers AS c建立c作为customers的别名，等等。这使得能使用省写的c而不是全名customers。表别名不仅能用于WHERE子句，它还可以用于SELECT的列表、ORDER BY子句以及语句的其他部分。

表别名只在查询执行中使用。与列别名不一样，表别名不返回到客户机。

### 使用不同类型的联结

#### 自联结

假如发现某物品（其ID为DTNTR）存在问题，因此想知道生产该物品的供应商生产的其他物品是否也存在这些问题。

~~~mysql
SELECT prod_id, prod_name
FROM products
WHERE vend_id = (SELECT vend_id 
                 FROM products 
                 WHERE prod_id = 'DTNTR');
~~~

这使用了子查询。内部的SELECT语句做了一个简单的检索，返回生产ID为DTNTR的物品供应商的vend_id。该ID用于外部查询的WHERE子句中，以便检索出这个供应商生产的所有物品。

现在使用联结的相同查询：

~~~mysql
SELECT p1.prod_id, p1.prod_name
FROM products AS p1, products AS p2
WHERE p1.vend_id = p2.vend_id
AND p2.prod_id = 'DTNTR';
~~~

此查询中需要的两个表实际上是相同的表，因此Products表在FROM子句中出现了两次。虽然这是完全合法的，但对products的引用具有二义性，因为MySQL不知道引用的是products表中的哪一个实例。

为解决此问题，使用了表别名。products的第一次出现为别名p1，第二次出现为别名p2.现在可以将这些别名作为表名。然后使用p1前缀明确地给出所需列的全名。如果不这样，MySQL将返回错误，因为分别存在两个名为prod_id、prod_name的列。MySQL不知道想要的是哪一个列（即使事实上是同一个列）。WHERE（通过匹配p1中的vend_id和p2中的vend_id）首先联结两个表，然后按第二个表中的prod_id过滤数据，返回所需的数据。

**用自联结而不用子查询**：自联结通常作为外部语句用来替代从相同表中检索数据时使用的子查询语句。虽然最终的结果是相同的，但有时候处理联结远比处理子查询快得多。应该试一下两种方法，以确定哪一种的性能更好。

#### 自然联结

标准的联结（内部联结）返回所有数据，甚至相同的列多次出现。**自然联结**多次出现，使每一个列只返回一次。

系统不完成这项工作，由自己完成它。自然联结是这样一种联结，其中只能选择那些唯一的列。这一般是通过对使用通配符（SELECT *），对所有其他表的列明确使用的子集来完成的。

~~~mysql
SELECT c.*, o.order_num, o.order_Date, oi.prod_id, oi.quantity, oi.item_price
FROM customers AS c, orders AS o, orderitems AS oi
WHERE c.cust_id = o.cust_id
AND oi.order_num = o.order_num
AND prod_id = 'FB';
~~~

在这个例子中，通配符只对第一个表使用。所有其他列明确列出，所以没有重复的列被检索出来。

#### 外部联结

许多联结将一个表中的行与另一个表中的行相关联。但有时候会需要包含没有关联行的那些行。例如，可能需要使用联结来完成以下工作：

- 对每个客户下了多少订单进行计数，包括那些至今尚未下订单的客户；
- 列出所有产品及订购数量，包括没有人订购的产品；
- 计算平均销售规模，包括那些至今尚未下订单的客户

在上述例子中，联结包含了那些在相关表中没有关联行的行。这种类型的联结称为**外部联结**。如下：

~~~mysql
SELECT customers.cust_id, orders.order_num
FROM customers INNER JOIN orders
ON customers.cust_id = orders.cust_id;
~~~

外部联结类似。为了检索所有客户，包括那些没有订单的客户，可如下进行：

~~~mysql
SELECT customers.cust_id, orders.order_num
FROM customers LEFT OUTER JOIN orders
ON customers.cust_id = orders.cust_id;
~~~

类似于上面的内部联结，这条SELECT语句使用了关键字OUTER JOIN来指定联结的类型（而不是在WHERE子句中指定）。但是，与内部联结关联两个表中的行不同的是，外部联结还包括没有关联行的行。在使用OUTER JOIN语法时，必须使用LEFT或者RIGHT关键字指定包括其所有行的表（RIGHT指出的是OUTER JOIN右边的表，而LEFT指出的是OUTER JOIN左边的表）。上面的例子使用LEFT OUTER JOIN从FROM子句的左边表中选择所有行。为了从右边的表中选择所有行，应该使用RIGHT OUTER JOIN，如下例所示：

~~~mysql
SELECT customers.cust_id, orders.order_num
FROM customers RIGHT OUTER JOIN orders
ON orders.cust_id = customers.cust_id;
~~~

**没有*=操作符**：MySQL不支持简化字符\*=和=*的使用。这两种操作符在其他DBMS中很流行。

**外部联结的类型**：存在两种基本的外部联结形式：左外部联结和右外部联结。它们之间的唯一差别是所关联的表的顺序不同。换句话说，左外部联结可通过颠倒FROM或WHERE子句中表的顺序转换为右外部联结。因此，两种类型的外部联结可互相转换，究竟使用哪一种纯粹是根据方便而定。

### 使用带聚集函数的联结

如下：

~~~mysql
SELECT customers.cust_name,customers.cust_id, COUNT(orders.order_num) AS num_ord
FROM customers INNER JOIN orders
ON customers.cust_id = orders.cust_id
GROUP BY customers.cust_id;
~~~

此SELECT语句使用INNER JOIN将customers和orders表互相关联。GROUP BY子句按客户分组数据，因此，函数调用COUNT(orders.order_num)对每个客户的订单计数，将它作为num_ord返回。

聚集函数也可以与其他联结使用：

~~~mysql
SELECT customers.cust_name,customers.cust_id,COUNT(orders.order_num) AS num_ord
FROM customers LEFT OUTER JOIN orders
ON customers.cust_id = orders.cust_id
GROUP BY customers.cust_id;
~~~

这个例子使用左外部联结来包含所有客户，甚至包含那些没有任何下订单的客户。

### 使用联结和联结条件

- 注意所使用的联结类型。一般我们使用内部联结，但使用外部联结也是有效的。
- 保证使用正确的联结条件，否则将返回不正确的数据。
- 应该总是提供联结条件，否则会出现笛卡尔积。
- 在一个联结中可以包含多个表，甚至对于每个联结可以采用不同的联结类型。虽然这样做事合法的，一般也很有用，但应该在一起测试它们前，分别测试每个联结。这将使故障排除更为简单。

## 组合查询

执行多个查询（多条SELECT语句），并将结果作为单个查询结果集返回。这些组合通常被称为并或复合查询。可能用到的情况如下：

- 在单个查询中从不同的表返回类似结构的数据。
- 对单个表执行多个查询，按单个查询返回数据。

**组合查询和多个WHERE条件**： 多数情况下，组合相同表的两个查询完成的工作与具有多个WHERE子句条件的单条查询完成的工作相同。换句话说，任何具有多个WHERE子句的SELECT语句都可以作为一个组合查询给出，在以下段落中可以看到这一点。这两种技术在不同的查询中性能也不同。因此，应该试一下这两种技术，以确定对特定的查询哪一种性能更好。

### 创建组合查询

用UNION操作符来组合数条SQL查询。

#### 使用UNION

给出每条SELECT语句，在各条语句之间放上关键字UNION。

假如需要价格小于等于5的所有物品的一个列表，而且还想包括供应商1001和1002生产的所有物品。

~~~mysql
SELECT vend_id, prod_id,prod_price
FROM products
WHERE prod_price <= 5;
~~~

~~~mysql
SELECT vend_id, prod_id, prod_price
FROM products
WHERE vend_id IN(1001,1002);
~~~

第一条SELECT检索价格不高于5的所有物品。第二条SELECT使用IN找出供应商1001和1002生产的所有物品。组合查询如下：

~~~mysql
SELECT vend_id, prod_id,prod_price
FROM products
WHERE prod_price <= 5
UNION 
SELECT vend_id, prod_id, prod_price
FROM products
WHERE vend_id IN (1001,1002);
~~~

UNION指示MySQL执行两条SELECT语句，并把输出合并成单个查询结果集。使用多条WHERE子句如下：

~~~mysql
SELECT vend_id,prod_id,prod_price
FROM products
WHERE prod_price <= 5
OR vend_id IN (1001,1002);
~~~

在这里，使用UNION可能比使用WHERE子句更为复杂，但是更复杂的过滤条件，或者从多个表中检索数据时，使用UNIION可能会使处理更简单。

#### UNION规则

- UNION必须由两条或两条以上的SELECT语句组成，语句之间用关键字UNION分隔（因此，如果组合4条SELECT语句，必须使用3个UNION关键字）。
- UNION中的每个查询必须包含相同的列、表达式或聚集函数（不过各个列不需要以相同的次序列出）。
- 列数据类型必须兼容：类型不必完全相同，但必须是DBMS可以隐含地转换的类型。

#### 包含或取消重复的行

UNION从查询结果集中自动去除了重复的行。这是UNION默认行为，如果需要保留重复的行，可以使用UNION ALL。

~~~mysql
SELECT vend_id,prod_id,prod_price
FROM products
WHERE prod_price <= 5
UNION ALL
SELECT vend_id, prod_id,prod_price
FROM products
WHERE vend_id IN (1001,1002);
~~~

**UNION与WHERE**：UNION几乎总是完成与多个WHERE条件相同的工作。UNION ALL为UNION的一种形式，它完成WHERE子句完成不了的工作。如果确实需要每个条件的匹配行全部出现（包括重复行），则必须使用UNION ALL而不是WHERE。

#### 对组合查询结果进行排序

SELECT语句的输出用ORDER BY子句排序。在用UNION组合查询时，只能使用ORDER BY子句，它必须出现在最后一条SELECT语句之后。对于结果集，不存在用一种方式排序一部分，而又使用另一种方式排序另一部分的情况，因此不允许使用多条ORDER BY子句。

~~~mysql
SELECT vend_id,prod_id,prod_price
FROM products
WHERE prod_price <= 5
UNION 
SELECT vend_id, prod_id,prod_price
FROM products
WHERE vend_id IN (1001,1002)
ORDER BY vend_id,prod_price;
~~~

MySQL将用最后一条ORDER BY子句对所有SELECT语句返回的结果进行排序。

UNION可以组合不同的表。

---

## 全文本搜索

