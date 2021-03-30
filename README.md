<!--
 * @Author: your name
 * @Date: 2021-03-30 16:13:34
 * @LastEditTime: 2021-03-30 16:15:04
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /mztknJson/README.md
-->
# 设计思路

简单来说，输入字符串 --> 解析到特定结构体中 --> 从结构体(DOM)获取值    ；  另一个功能：能够从结构体中恢复成一个Json字符串 (stringnify)

注：[关于命名风格统一](##命名风格统一)

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

解析函数





## 命名风格统一

1. 类和结构首字母大写
2. 驼峰式命名规则
3. 变量首字母小写
4. 类私有成员使用前下划线表示
5. 全局变量用g开头

