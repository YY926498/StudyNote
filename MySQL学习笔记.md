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





