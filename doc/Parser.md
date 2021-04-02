<!--
 * @Author: your name
 * @Date: 2021-03-30 17:34:46
 * @LastEditTime: 2021-04-02 10:45:21
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

## 解析字符串

JSON字符串与C语言相似，并且支持9种转义序列

~~~
string = quotation-mark *char quotation-mark
char = unescaped /
   escape (
       %x22 /          ; "    quotation mark  U+0022
       %x5C /          ; \    reverse solidus U+005C
       %x2F /          ; /    solidus         U+002F
       %x62 /          ; b    backspace       U+0008
       %x66 /          ; f    form feed       U+000C
       %x6E /          ; n    line feed       U+000A
       %x72 /          ; r    carriage return U+000D
       %x74 /          ; t    tab             U+0009
       %x75 4HEXDIG )  ; uXXXX                U+XXXX
escape = %x5C          ; \
quotation-mark = %x22  ; "
unescaped = %x20-21 / %x23-5B / %x5D-10FFFF
~~~

JSON字符串是由两个双引号夹着零个至多个字符。字符分为无转义字符或转义序列。转义序列有9种，都是以反斜杠开始。

### 字符串表示

在C语言种，字符串一般表示为空结尾字符传结尾，然而JSON字符串是允许含有空字符的，所以我们可以分配内存来存储解析后的字符，并记录字符的数目。由于大多数C程序都假设字符串是空结尾字符，我们还是在最后加上一个空字符。
所以我们在Value结构体种加入两个成员 `char* s; size_t len`;当然一个值不可能同时为数字和字符串，我们用union结构,注意这里需要使用c++11的新特性匿名union；所以编译时需要std=c++

```c++
struct Value{
    union{
        struct {char* s; size_t len; } _s;
        double _n;
    };
    ValueType _type;
}
```

### 内存管理
字符串的长度不是固定的，我们需要动态分配内存。简单期间，我们使用malloc()和free()来管理内存。当设置一个值为字符串时，我们需要把参数种的字符串复制一份：

```c++
void set_string(Value* v, const char* s, size_t len){
    assert(v != NULL && (s != NULL || len == 0));
    mztkn_free(v);
    v->_s = (char*)malloc(len+1);
    memcpy(v->_s, s, len);
    v->_s.s[len] = '\0';
    v->type = JSON_STRING;
}

```

在设置值之前需要将之前分配的内存释放，这里采用mztkn_free()函数：

```c++
void Free(Value* v){
    assert(v!=NULL);
    if(v->_type == JSON_STRING)
        free(v->_s);
    v->set_null();
}
```

### 缓冲区与堆栈

解析字符串（以及之后的数组、对象）时，需要把解析的结果先存储在一个临时的缓冲区，再用set_string()把缓冲区的结果设进值之中。

本文设计Parser 拥有成员变量Context 初始化Context时，初始化一定大小的栈（缓冲区）这个栈是以字节存储的，每次可要求压入任意大小的数据，它会返回数据起始的指针，压入时若空间不足，便以1.5倍大小扩展。栈的初始大小由宏 `PARSE_STACK_INIT_SIZE`确定

### 解析字符串

通过上述栈、清空等叙述，我们把解析字符串的任务分为以下几个步骤：
* 备份栈顶

* 把解析到的字符压栈

* 计算出长度，并一次性把所有字符弹出，在设置至value种。

以下是部分实现，没有处理转义和一些不合法字符的校验

```c++
static int parse_string(Context& c, Value* v){
    size_t head = c->top, len;
    const char* p;
    EXPECT(c, '\"');
    p = c._json;
    for(;;){
        char ch = *p++;
        switch(ch){
            case '\"':
                len = c->top - head;
            //...
        }
    }
}
```

#### 一些特殊情况的处理

对于不合法的字符 使用： `if((unsigned char)ch < 0x20){return PARSE_INVALID_STRING_CHAR}` 

#### Unicode 处理

Unicode9.0 已经收录135种语言共128237个字符。UTF-8是8位字节。每种UTF会把一个码点存储位一个至多个编码单元(code unit)。UTF-8 采用字节编码单元，不会有字节序的问题。每个ASCII字符只需要一个字节存储。如果程序原来已字节方式存储字符，理论上不需要特别的改动就能自动处理UTF-8的数据。

由于UTF-8的普及性，mztknJSON支持UTF-8。

* JSON字符串中\uXXXX是以16进制表示码点U+0000 至 0+FFFF，我们需要：
    * 解析4位16进制整数为码点
    * 由于字符串以UTF-8存储，我们需要把这个码点编码成UTF-8
4位16进制数字只能表示 0 至 0xFFFF，但是UCS的码点从0至0X10FFFF，怎么表示多出来的码点？JSON会使用一对**(surrogate pair, H, L)**来表示：`\uXXXX\uYYYY`，在BNP种，保留了2048个码点，如果第一个码点是`U+D800 至 U+DBFF`，我们便直到代码对pair的高代理项(high surrogate)，之后应该伴随着一个U+DC00至U+DFFF的低代理项，然后使用下面的公式转换成真实码点：
`codepoint = 0x10000 + (H - 0xD800) x 0x400 + (L - 0xDC00`


(H, L)如果只有高代理项或者低代理项不在合法范围内，返回 PARSE_INVALID_UNICODE_SURROGATE错误，如果\u后面不是4位十六进制数字，则返回PARSE_INVALID_UNICODE_HEX错误。

UTF-8编码，把每个码点编码成1至4个字节，如下表所示：
| 码点范围            | 码点位数  | 字节1     | 字节2    | 字节3    | 字节4     |
|:------------------:|:--------:|:--------:|:--------:|:--------:|:--------:|
| U+0000 ~ U+007F    | 7        | 0xxxxxxx |
| U+0080 ~ U+07FF    | 11       | 110xxxxx | 10xxxxxx |
| U+0800 ~ U+FFFF    | 16       | 1110xxxx | 10xxxxxx | 10xxxxxx |
| U+10000 ~ U+10FFFF | 21       | 11110xxx | 10xxxxxx | 10xxxxxx | 10xxxxxx |
这种编码的好处是，码点U+000至U+007F编码为一个字节，与ASCII兼容

例如，欧元符号€ -> U+20AC

1. U+20AC 在 U+0800 ~ U+FFFF 的范围内，应编码成 3 个字节。
2. U+20AC 的二进位为 10000010101100
3. 3 个字节的情况我们要 16 位的码点，所以在前面补两个 0，成为 0010000010101100
4. 按上表把二进位分成 3 组：0010, 000010, 101100
5. 加上每个字节的前缀：11100010, 10000010, 10101100
6. 用十六进位表示即：0xE2, 0x82, 0xAC

~~~c
if (u >= 0x0800 && u <= 0xFFFF) {
    OutputByte(0xE0 | ((u >> 12) & 0xFF)); /* 0xE0 = 11100000 */
    OutputByte(0x80 | ((u >>  6) & 0x3F)); /* 0x80 = 10000000 */
    OutputByte(0x80 | ( u        & 0x3F)); /* 0x3F = 00111111 */
}
~~~

#### 最终解析过程如下：

遇到\u转义，调用parse_hex4()解析4位16进制数字，存储为码点u, 成功后返回解析后的文本指针，失败返回NULL,如果失败，就安徽PARSE_INVALID_UNICODE_HEX

### 解析数组

一个JSON数组可以包含零个至多个元素，这些元素可以是数组类型，也就是可以nested嵌套的。

`array = %x5B ws [value *(ws %x2C ws value) ] ws %x5D`

当中`%x5B是左括号[`, `%x2C 是 逗号`， `%x5D是右中括号`， ws是空白字符，一个数组可以包含零个至多个值，以逗号分隔
`[], [1,,2,true], [[1,2], [3,4], "abc"]`都是合法数组，JSON不接受末端额外的逗号

#### 设置数组结构，保存JSON数组类型

JSON数组存储零至多个元素，可以使用数组或者链表存储，数组最大的好处是以O(1)的复杂度访问任意元素；次要好处是内存紧凑，省内存的同时还有高缓存一致性。但是不能快速插入元素。这里采用数组

```c++

    union{
        struct {Value* e; size_t size;}_a;
        double    _n;
        struct {char* s; size_t len;} _s;
    };

```

其中 size是元素的个数，而不是字节的大小

#### 一个bug记录

```c++
Value e; 

//...

else if(*c._json == ']'){
    c._json++;
    v->_type = JSON_ARRAY;
    v->_a.size = size;
    size *= sizeof(Value);
    memcpy(v->_a.e = (Value*)malloc(size), c.Pop(size), size);          //由于这里不是深拷贝，只拷贝了指针
    return PARSE_OK;                                                    //Value e;结束后会调用析构函数
}else{
    ret = PARSE_MISS_COMMA_ORSQUARE_BRACKET;
}
```

e不是深拷贝，只拷贝了指针，Value e的生命周期结束后会调用析构函数内的Free()函数，导致字符串等需要分配内存的元素被析构，也就是v也失去了这块内容。为了将栈中的内容拷贝到v中，可以采用：①深拷贝，这里涉及到多次申请和释放内存②直接浅拷贝，然后在最后释放所有申请的内存. 我们采取后一种方式，所以需要记得每次退出前需要调用Free(); 还有一种解决方法；利用RAII,构造一个Value RAII类,由该类负责调用Free()函数，但是同样需要额外的操作，这里简单期起见采用第二种方法。