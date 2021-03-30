<!--
 * @Author: your name
 * @Date: 2021-03-30 17:34:46
 * @LastEditTime: 2021-03-30 17:45:21
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /mztknJson/doc/Parser.md
-->
# Parser设计思路


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