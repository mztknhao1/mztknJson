/*
 * @Author: your name
 * @Date: 2021-03-30 16:19:33
 * @LastEditTime: 2021-04-03 10:02:44
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /mztknJson/mztknJson/parser.hpp
 */
#ifndef MZTKN_PARSER_H__
#define MZTKN_PARSER_H__

#include "mztknJson.h"
#include "context_stack.hpp"
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