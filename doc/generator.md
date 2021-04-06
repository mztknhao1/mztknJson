<!--
 * @Author: your name
 * @Date: 2021-04-03 09:53:05
 * @LastEditTime: 2021-04-03 09:59:32
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /mztknJson/doc/generator.md
-->
# JSON生成器

使用Parser类可以生成一个树形结构，整个结构以 Value节点组成：

```c++
Value{
    union{
        struct {member* m; size_t size} _o;
        struct {char* s; size_t len}_s;
        double _n;
        struct {Value* e, size_t size}_a;
    }
    ValueType _type;
}

//使用方法：
Parser p;
Value v;
p.parse(&v, json);  //其中json是c_str() C风格的字符串
```

那接下来就需要把这样一个结构生成一个JSON文本，这个过程称为"字符串化stringify"

```c++
class Generator{
    static char* stringify(const Value& v, size_t* length = NULL);
}
```