<!--
 * @Author: your name
 * @Date: 2021-03-30 17:34:46
 * @LastEditTime: 2021-03-31 09:31:30
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /mztknJson/doc/Parser.md
-->
# Parser设计思路

* [解析数字类型](##解析数字类型)

为了避免解析函数之间传递多个参数，我们把相关内容数据放入一个Context的结构体。注意这个类不负责构造和析构 json字符数组，仅作为private

```c++
    struct Context{
    public:
        const char *_json;
        explicit Context(char *json){
            assert(json!=NULL);
            _json = json;
        }
    };

```

## 解析数字类型

数字类型的语法：

```
number = [ "-" ] int [ frac ][ exp ]
int = "0" / digit1-9 *digit
frac = "." 1*digit
exp = ("e"/"E")["-"/"+"]1*digit
```

number以十进制表示，主要由4部分组成：负号、整数、小数、指数。只有整数是必须部分，正号是不合法的。
整数部分如果是0开始，只能是单个0；而由1-9开始，可以加任意数量的数字(0-9). `0123`不是一个合法的数字。
小数部分比较直观，就是小数点后面是1到多个数字(0-9)。
JSON可以使用科学计数法，指数部分由大or小写e开始，然后可有正负号。上述过程可由图表示：

![number](./images/number.png)

### number存储的类型

之前的Null, true, false都可以直接保存为对应类型，但是数字我们需要考虑保存数数字的类型，简单起见，这里使用`double`来存储其类型

```c++
struct Value{
    double _n;
    ValueType _type;
}
```

当且仅当`_type == JSON_NUMBER`时，_n才表示数字，所以获取该值的API如下：

```c++
double Parser::get_number(const Value* v){
    assert(v!=NULL && v->type == JSON_NUMBER);
    return v->_n;
}
```

对应的我们也有一些测试用例。。。

### 十进制数转二进制double

这并不是一个容易的事情。可以参考RapidJson实现位于`rapidjson/reader.h 1468 - 1747行`。简单起见，这里首先使用标准库strtod().该函数可以转换JSON要求的格式；但是一些JSON不允许的格式，strtod()也可以转换
所以我们需要自行格式检验：

```c++
int parse_number(Context& c, Value* v){
    char* end;
    //TODO 检查是否合法
    v->n = strtod(c._json, &end);
    if(c._json == end){
        return PARSE_INVALID_VALUE;
    }
    c._json = end;
    v->type = JSON_NUMBER;
    return PARSE_OK;
}

```