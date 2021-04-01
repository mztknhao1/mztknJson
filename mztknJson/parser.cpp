/*
 * @Author: your name
 * @Date: 2021-03-30 16:47:25
 * @LastEditTime: 2021-04-01 09:56:17
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /mztknJson/mztknJson/parser.cpp
 */
#include <errno.h>
#include <math.h>
#include <malloc.h>
#include "parser.h"

MZTKNJSON_NAMESPACE_BEGIN

#define EXPECT(c, ch)   do {assert(*c._json == (ch)); c._json++;} while(0)
#define ISDIGIT(ch)     ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch)     ((ch) >= '1' && (ch) <= '9')

int Parser::parse(Value* v, const char* json){
    assert(v != NULL);
    v->set_null();
    Context c(json);
    int ret;
    
    // 处理空格
    parse_whitespace(c);
    
    if((ret = parse_value(c, v)) == PARSE_OK){
        parse_whitespace(c);
        if(*c._json != '\0'){
            v->set_null();
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
        case '"':
            return parse_string(c, v);
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
    v->set_type(JSON_NUMBER);
    return PARSE_OK;
}



ValueType Parser::get_type(const Value* v){
    assert(v!=NULL);
    return v->get_type();
}

double Parser::get_number(const Value* v){
    assert(v!=NULL && v->get_type() == JSON_NUMBER);
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
    v->set_type(type);
    return PARSE_OK;
}

int Parser::parse_string(Context& c, Value* v){
    assert(v!=NULL);
    size_t head = c._top, len;
    const char* p;
    EXPECT(c, '\"');
    p = c._json;
    for(;;){
        char ch = *p++;
        switch(ch){
            case '\"':
                len = c._top - head;
                v->set_string((const char*)c.Pop(len), len);
                c._json = p;
                return PARSE_OK;
            case '\\':
                switch(*p++){
                    case '\"': c.put_char('\"'); break;
                    case '\\': c.put_char('\\'); break;
                    case '/' : c.put_char('/');  break;
                    case 'b' : c.put_char('\b'); break;
                    case 'f' : c.put_char('\f'); break;
                    case 'n' : c.put_char('\n'); break;
                    case 'r' : c.put_char('\r'); break;
                    case 't' : c.put_char('\t'); break;
                    default:
                        c._top = head;
                        return PARSE_INVALID_STRING_ESCAPE;
                }
            case '\0':
                c._top = head;
                return PARSE_MISS_QUOTATION_MARK;
            default:
                if((unsigned char)ch < 0x20){
                    c._top = head;
                    return PARSE_INVALID_STRING_CHAR;
                }
                c.put_char(ch);
        }
    }
}


MZTKNJSON_NAMESPACE_END