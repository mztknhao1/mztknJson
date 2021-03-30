/*
 * @Author: your name
 * @Date: 2021-03-30 16:19:33
 * @LastEditTime: 2021-03-30 19:53:10
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
    PARSE_ROOT_NOT_SINGULAR
} ParseState;

struct Value{
    ValueType _type;
};


class Parser{
public:
    int parse(Value* v, const char* json);

    ValueType get_type(const Value* v);


private:
    struct Context{
        const char *_json;
        explicit Context(const char *json){
            assert(json!=NULL);
            _json = json;
        }
    };

    void parse_whitespace(Context& c);

    int  parse_value(Context& c, Value* v);
    int  parse_null(Context& c, Value* v);
    int  parse_true(Context& c, Value* v);
    int  parse_false(Context& c, Value* v);

};



MZTKNJSON_NAMESPACE_END

#endif