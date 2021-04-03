/*
 * @Author: your name
 * @Date: 2021-03-30 16:19:33
 * @LastEditTime: 2021-04-03 08:13:59
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /mztknJson/mztknJson/parser.hpp
 */
#ifndef MZTKN_PARSER_H__
#define MZTKN_PARSER_H__

#include "mztknJson.h"
#include "value.h"

MZTKNJSON_NAMESPACE_BEGIN

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
    PARSE_INVALID_UNICODE_SURROGATE,
    PARSE_MISS_COMMA_ORSQUARE_BRACKET,
    PARSE_MISS_KEY,
    PARSE_MISS_COLON,
    PARSE_MISS_COMMA_OR_CURLY_BRACKET
} ParseState;

class Value;

enum ValueType;
// enum ValueType;

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
    int  parse_string_raw(Context& c, char** str, size_t* len);     //为了解析object时的重用
    const char* parse_hex4(const char* p, unsigned& u);
    void encode_utf8(Context& c, unsigned u);
    int  parse_literal(Context& c, Value* v, const char* literal, ValueType type);
    int  parse_array(Context& c, Value* v);
    int  parse_object(Context& c, Value* v);
    
};



MZTKNJSON_NAMESPACE_END

#endif