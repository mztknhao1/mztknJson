/*
 * @Author: your name
 * @Date: 2021-03-30 16:47:25
 * @LastEditTime: 2021-03-30 20:06:07
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /mztknJson/mztknJson/parser.cpp
 */
#include "parser.h"

MZTKNJSON_NAMESPACE_BEGIN

#define EXPECT(c, ch)   do {assert(*c._json == (ch)); c._json++;} while(0)

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
            return parse_null(c,v);
            break;
        case 't':
            return parse_true(c, v);
            break;
        case 'f':
            return parse_false(c, v);
            break;
        case '\0':
            return PARSE_EXPECT_VALUE;
            break;
        default:
            return PARSE_INVALID_VALUE;
    }
}

int  Parser::parse_null(Context& c, Value* v){
    EXPECT(c, 'n');
    if(c._json[0]!='u' || c._json[1] != 'l' || c._json[2] != 'l'){
        return PARSE_INVALID_VALUE;
    }
    c._json += 3;
    return PARSE_OK;
}

int Parser::parse_true(Context& c, Value* v){
    EXPECT(c, 't');
    if(c._json[0]!='r' ||c._json[1]!='u' || c._json[2]!='e'){
        return PARSE_INVALID_VALUE;
    }    
    c._json += 3;
    v->_type = JSON_TRUE;
    return PARSE_OK;
}

int Parser::parse_false(Context& c, Value* v){
    EXPECT(c, 'f');
    if(c._json[0]!='a' ||c._json[1]!='l' || c._json[2]!='s' || c._json[3]!='e'){
        return PARSE_INVALID_VALUE;
    }    
    c._json += 4;
    v->_type = JSON_FALSE;
    return PARSE_OK;
}



ValueType Parser::get_type(const Value* v){
    assert(v!=NULL);
    return v->_type;
}







MZTKNJSON_NAMESPACE_END