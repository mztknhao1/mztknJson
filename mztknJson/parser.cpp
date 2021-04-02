/*
 * @Author: your name
 * @Date: 2021-03-30 16:47:25
 * @LastEditTime: 2021-04-02 10:56:36
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
        case 't':
            return parse_literal(c, v, "true", JSON_TRUE);
        case 'f':
            return parse_literal(c, v, "false", JSON_FALSE);
        case '\0':
            return PARSE_EXPECT_VALUE;
        case '"':
            return parse_string(c, v);
        case '[':
            return parse_array(c, v);
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
    double n = strtod(c._json, NULL);
    if(errno == ERANGE && (n == HUGE_VAL || n == -HUGE_VAL)){
        return PARSE_NUMBER_TOO_BIG;
    }
    v->_n = n;
    c._json = p;
    v->set_type(JSON_NUMBER);
    return PARSE_OK;
}

ValueType Parser::get_type(const Value* v){
    assert(v!=NULL);
    return v->get_type();
}

const char *Parser::parse_hex4(const char* p, unsigned& u){
    int i;
    u = 0;
    for(i = 0; i < 4; i++){
        char ch = *p++;
        u <<= 4;
        if     (ch >= '0' && ch <= '9') u |= ch - '0';
        else if(ch >= 'A' && ch <= 'F') u |= ch - ('A' - 10);
        else if(ch >= 'a' && ch <= 'f') u |= ch - ('a' - 10);    
    }
    return p;
}

void Parser::encode_utf8(Context& c, unsigned u){
    if (u <= 0x7F) 
        c.put_char(u & 0xFF);
    else if (u <= 0x7FF) {
        c.put_char(0xC0 | ((u >> 6) & 0xFF));
        c.put_char(0x80 | ( u       & 0x3F));
    }
    else if (u <= 0xFFFF) {
        c.put_char(0xE0 | ((u >> 12) & 0xFF));
        c.put_char(0x80 | ((u >>  6) & 0x3F));
        c.put_char(0x80 | ( u        & 0x3F));
    }
    else {
        assert(u <= 0x10FFFF);
        c.put_char(0xF0 | ((u >> 18) & 0xFF));
        c.put_char(0x80 | ((u >> 12) & 0x3F));
        c.put_char(0x80 | ((u >>  6) & 0x3F));
        c.put_char(0x80 | ( u        & 0x3F));
    }
}


double Parser::get_number(const Value* v){
    assert(v!=NULL && v->get_type() == JSON_NUMBER);
    return v->get_number();
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


#define STRING_ERROR(ret) do { c._top = head; return ret; } while(0)

int Parser::parse_string(Context& c, Value* v){
    assert(v!=NULL);
    size_t head = c._top, len;
    const char* p;
    unsigned u,u2;
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
                    case 'u' : 
                        if(!(p = parse_hex4(p, u))){
                            STRING_ERROR(PARSE_INVALID_UNICODE_HEX);
                        }
                        if (u >= 0xD800 && u <= 0xDBFF) { /* surrogate pair */
                            if (*p++ != '\\')
                                STRING_ERROR(PARSE_INVALID_UNICODE_SURROGATE);
                            if (*p++ != 'u')
                                STRING_ERROR(PARSE_INVALID_UNICODE_SURROGATE);
                            if (!(p = parse_hex4(p, u2)))
                                STRING_ERROR(PARSE_INVALID_UNICODE_HEX);
                            if (u2 < 0xDC00 || u2 > 0xDFFF)
                                STRING_ERROR(PARSE_INVALID_UNICODE_SURROGATE);
                            u = (((u - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000;
                        }
                        encode_utf8(c, u);
                        break;
                    default:
                        c._top = head;
                        return PARSE_INVALID_STRING_ESCAPE;
                }
                break;
            case '\0':
                STRING_ERROR(PARSE_MISS_QUOTATION_MARK);
            default:
                if((unsigned char)ch < 0x20){
                    STRING_ERROR(PARSE_INVALID_STRING_CHAR);
                }
                c.put_char(ch);
        }
    }
}


int Parser::parse_array(Context& c, Value* v){
    size_t size = 0;
    int ret;
    EXPECT(c, '[');
    parse_whitespace(c);
    if(*c._json==']'){
        c._json++;
        v->set_type(JSON_ARRAY);
        v->_a.size = 0;
        v->_a.e = NULL;
        return PARSE_OK;
    }
    for(;;){
        Value e;
        if((ret = parse_value(c, &e)) != PARSE_OK){
            break;
        }
        memcpy(c.Push(sizeof(Value)), &e, sizeof(Value));
        size++;
        parse_whitespace(c);
        if(*c._json == ','){
            c._json++;
            parse_whitespace(c);
        }
        else if(*c._json == ']'){
            c._json++;
            v->_type = JSON_ARRAY;
            v->_a.size = size;
            size *= sizeof(Value);
            Value* tmp = (Value*)c.Pop(size);
            memcpy(v->_a.e = (Value*)malloc(size), (void*)tmp, size);
            return PARSE_OK;
        }else{
            ret = PARSE_MISS_COMMA_ORSQUARE_BRACKET;
        }
    }
    for(size_t i = 0;i < size;i++){
        Value* tmpV = (Value*)c.Pop(sizeof(Value));
        tmpV->Free();
    }
    return ret;
}

MZTKNJSON_NAMESPACE_END