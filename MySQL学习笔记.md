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















