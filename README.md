<!--
 * @Author: your name
 * @Date: 2021-03-30 16:13:34
 * @LastEditTime: 2021-04-02 20:07:04
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /mztknJson/README.md
-->
# 设计思路

简单来说，输入字符串 --> 解析到特定结构体中 --> 从结构体(DOM)获取值    ；  另一个功能：能够从结构体中恢复成一个Json字符串 (stringnify)

注：[关于命名风格统一](##命名风格统一)

## 优势与待优化

* 兼容UTF-8编码

* 使用简单

### 优化

* 可以改用使用unordered_multimap实现object对象的存储结构 

## 解析器

解析器可以设计为**递归式**或者**迭代式**

* 递归式解析器较快，但是极端情况下可能会出现栈溢出
* 迭代时解析器使用自定义的堆栈去维持解析状态

本项目实现的方式暂时设定为**递归式**

## 数据结构

#### 整体结构

JSON是一个树状的数据结构，每个节点使用 `Value`结构体表示，称为Json Value。为不同的节点类型设计一个enum，这个enum种共7种类型，包括:
{NULL, FALSE, TRUE, NUMBER, STRING, ARRAY, OBJECT}

（1） 首先实现解析 null, true, false

#### 节点结构

```c++
typedef enum {NULL, FALSE, TRUE, NUMBER, STRING, ARRAY, OBJECT} ValueType;

struct Value{
    ValueType _type;
}
```

#### 解析类 Parser 

最基本的方法是 `int parse(Value* v, const char* json);` 返回值 `ParseState`

```c++
typedef enum {PARSE_OK, PARSE_EXPECT_VALUE, PARSE_INVALID_VALUE, PARSE_ROOT_NOT_SINGULAR} ParseState; 

int parse(Value* v, const char* json);

//获取节点的类型
ValueType getType(Value* v);
```

关于[状态码](./doc/状态码.md)的含义详细


传入解析函数的应该是 const char* 为了避免修改输入字符串 `parse`

**更多的设计思路查看各个分目录文档**

* [Parser设计思路](./doc/Parser.md)


## 使用test.c做一些单元测试

## 命名风格统一

1. 类和结构首字母大写，使用驼峰式命名规则
2. 使用小写加下划线命名函数；宏定义用全大写
3. 变量首字母小写
4. 类私有成员使用前下划线表示
5. 全局变量用g开头
6. 成员函数使用小写开头，

## 参考

* [json_tutorial](https://github.com/miloyip/json-tutorial)