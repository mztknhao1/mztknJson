/*
 * @Author: your name
 * @Date: 2021-03-30 16:19:33
 * @LastEditTime: 2021-04-01 11:29:09
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /mztknJson/mztknJson/parser.hpp
 */
#ifndef MZTKN_PARSER_H__
#define MZTKN_PARSER_H__

#include "mztknJson.h"

MZTKNJSON_NAMESPACE_BEGIN

typedef enum {JSON_NULL, JSON_FALSE, JSON_TRUE, JSON_NUMBER, JSON_STRING, JSON_ARRAY, JSON_OBJECT} ValueType;

typedef enum {
    PARSE_OK = 0,
    PARSE_EXPECT_VALUE,
    PARSE_INVALID_VALUE,
    PARSE_ROOT_NOT_SINGULAR,
    PARSE_NUMBER_TOO_BIG,
    PARSE_MISS_QUOTATION_MARK,
    PARSE_INVALID_STRING_ESCAPE,
    PARSE_INVALID_STRING_CHAR,
    PARSE_INVALID_UNICODE_HEX,
    PARSE_INVALID_UNICODE_SURROGATE
} ParseState;

class Value{
public:
    union{
        double    _n;
        struct {char* s; size_t len;} _s;
    };
    Value(){_type = JSON_NULL;}
    ~Value(){
        Free();
    }
    void set_null(){
        _type = JSON_NULL;
    }
    ValueType get_type() const {
        return _type;
    }
    ValueType set_type(ValueType vt){
        _type = vt;
    }
    void Free(){
        if(_type == JSON_STRING)
            free(_s.s);
        _type = JSON_NULL;
    }
    int get_boolean(){
        assert(_type == JSON_TRUE || _type==JSON_FALSE);
        if(_type == JSON_TRUE){
            return 1;
        }else if(_type == JSON_FALSE){
            return 0;
        }
        return 0;
    }
    void set_boolean(int b){
        Free();
        _type = b?JSON_TRUE:JSON_FALSE;
    }
    double get_number(){
        assert(_type == JSON_NUMBER);
        return _n;
    }
    void set_number(double n){
        Free();
        _n = n;
        _type = JSON_NUMBER;
    }
    
    const char* get_string(){
        assert(_type == JSON_STRING);
        return _s.s;
    }
    size_t              get_string_length(){
        assert(_type == JSON_STRING);
        return _s.len;
    }
    void set_string(const char* s, size_t len){
        assert(s!=NULL || len==0);
        Free();
        _s.s = (char*)malloc(len+1);
        memcpy(_s.s, s, len);
        _s.s[len] = '\0';
        _s.len = len;
        _type = JSON_STRING;
    }

private:
    ValueType _type;


};


class Parser{
public:
    int parse(Value* v, const char* json);


    ValueType get_type(const Value* v);

    double    get_number(const Value* v);


private:
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

        void *Pop(size_t sz){
            assert(_top >= sz);
            return _stack + (_top -= sz);
        }

    };

    void parse_whitespace(Context& c);

    int  parse_value(Context& c, Value* v);
    int  parse_number(Context& c, Value* v);
    int  parse_string(Context& c, Value* v);
    const char* parse_hex4(const char* p, unsigned& u);
    void encode_utf8(Context& c, unsigned u);
    int  parse_literal(Context& c, Value* v, const char* literal, ValueType type);


    
};



MZTKNJSON_NAMESPACE_END

#endif