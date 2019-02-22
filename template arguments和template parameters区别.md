# template arguments和template parameters区别

`Arrar<int,10>`:其中在template名称之后紧跟着用角括号括起来的称为**template arguments**(模板实参)。其中**template name + template arguments**组合称为**template-id**。

- **template parameters**（模板形参）是在`template`声明或定义中位于关键字`template`之后的名称（例如T）
- **template arguments**是用以替换`template parameters`的东西（例如int和10）。和`template parameters`不同的是，`template arguments`并不局限于名称（也可以是数据）



基本原则：任何template arguments都必须是编译期可确定的类型或数值。