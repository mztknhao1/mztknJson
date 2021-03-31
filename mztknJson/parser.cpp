/*
 * @Author: your name
 * @Date: 2021-03-30 16:47:25
 * @LastEditTime: 2021-03-31 10:03:16
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /mztknJson/mztknJson/parser.cpp
 */
#include <errno.h>
#include <math.h>
#include "parser.h"

MZTKNJSON_NAMESPACE_BEGIN

#define EXPECT(c, ch)   do {assert(*c._json == (ch)); c._json++;} while(0)
#define ISDIGIT(ch)     ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch)     ((ch) >= '1' && (ch) <= '9')

int Parser::parse(Value* v, const char* json){
    assert(v != NULL);
    v->_type = JSON_NULL;
    Context c(json);
    int ret;
    
    // 处理空格
    parse_whitespace(c);
    
    if((ret = parse_value(c, v)) == PARSE_OK){
        parse_whitespace(c);
        if(*c._json != '\0'){
            v->_type = JSON_NULL;
            ret = PARSE_ROOT_NOT_SINGULAR;
        }
    }
    return ret;
}


void Parser::parse_whitespace(Context& c){
    const char* p = c._json;
    while(*p == ' ' || *p == '\t' || *p == '\r'){
        p++;
    }
    c._json = p;
}

int Parser::parse_value(Context& c, Value* v){
    switch(*c._json){
        case 'n':
            return parse_literal(c, v, "null", JSON_NULL);
            break;
        case 't':
            return parse_literal(c, v, "true", JSON_TRUE);
            break;
        case 'f':
            return parse_literal(c, v, "false", JSON_FALSE);
            break;
        case '\0':
            return PARSE_EXPECT_VALUE;
            break;
        default:
            return parse_number(c, v);
    }
}

int Parser::parse_number(Context& c, Value* v){
    const char* p = c._json;
    if(*p == '-') p++;
    if(*p == '0') p++;
    else{
        if(!ISDIGIT1TO9(*p)) return PARSE_INVALID_VALUE;
        for(p++; ISDIGIT(*p);p++);
    }
    if(*p == '.'){
        p++;
        if(!ISDIGIT(*p)) return PARSE_INVALID_VALUE;
        for(p++; ISDIGIT(*p);p++);
    }
    if(*p == 'e' || *p == 'E'){
        p++;
        if(*p == '+' || *p == '-') p++;
        if(!ISDIGIT(*p)) return PARSE_INVALID_VALUE;
        for(p++; ISDIGIT(*p); p++);
    }
    errno = 0;
    v->_n = strtod(c._json, NULL);
    if(errno == ERANGE && (v->_n == HUGE_VAL || v->_n == -HUGE_VAL)){
        return PARSE_NUMBER_TOO_BIG;
    }
    c._json = p;
    v->_type = JSON_NUMBER;
    return PARSE_OK;
}



ValueType Parser::get_type(const Value* v){
    assert(v!=NULL);
    return v->_type;
}

double Parser::get_number(const Value* v){
    assert(v!=NULL && v->_type == JSON_NUMBER);
    return v->_n;
}

/*解析字面量*/
int  Parser::parse_literal(Context& c, Value* v, const char* literal, ValueType type){
    size_t i;
    EXPECT(c, literal[0]);
    for(i = 0;literal[i+1];i++){
        if(c._json[i]!=literal[i+1]){
            return PARSE_INVALID_VALUE;
        }
    }
    c._json += i;
    v->_type = type;
    return PARSE_OK;
}



MZTKNJSON_NAMESPACE_END