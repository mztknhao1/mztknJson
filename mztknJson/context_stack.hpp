/*
 * @Author: your name
 * @Date: 2021-04-03 10:01:40
 * @LastEditTime: 2021-04-03 10:33:35
 * @LastEditors: Please set LastEditors
 * @Description: 用于解析器内部的临时栈结构
 * @FilePath: /mztknJson/mztknJson/context_stack.hpp
 */
#ifndef MZTKN_CONTEXT_STACK_H__
#define MZTKN_CONTEXT_STACK_H__


struct Context{
    const char *_json;
    char* _stack;                    //预留的临时栈
    size_t _size, _top;
    explicit Context(const char *json){
        assert(json!=NULL);
        _json = json;
        _size = _top = 0;
        _stack = NULL;
    }
    Context(){
        _size = _top = 0;
        _stack = NULL;
    }
    ~Context(){
        assert(_top == 0);
        free(_stack);
    }
    void *Push(size_t sz){
        void* ret;
        assert(sz > 0);
        if(_top + sz >= _size){
            if(_size == 0){
                _size = PARSE_STACK_INIT_SIZE;
            }
            while(_top + sz >= _size)
                _size += _size >> 1;        // _size * 1.5
            _stack = (char*)realloc(_stack, _size);
        }
        ret = _stack + _top;
        _top += sz;
        return ret;
    }
    void put_char(char ch){
        *(char*)Push(sizeof(char)) = ch;
    }

    void put_string(const char* str, size_t len){
        memcpy(Push(len), str, len);
    }

    void *Pop(size_t sz){
        assert(_top >= sz);
        return _stack + (_top -= sz);
    }

};

#endif