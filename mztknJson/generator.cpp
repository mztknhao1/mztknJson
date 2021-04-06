/*
 * @Author: your name
 * @Date: 2021-04-03 10:09:33
 * @LastEditTime: 2021-04-06 18:02:51
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /mztknJson/mztknJson/generator.cpp
 */

#include "generator.h"


MZTKNJSON_NAMESPACE_BEGIN

char* Generator::stringify(const Value* v, size_t* length){
    Context c;
    c._stack = (char*)malloc(c._size = PARSE_STRINGIFY_INIT_SIZE);
    c._top = 0;
    stringnify_value(c, v);
    if(length)
        *length = c._top;
    c.put_char('\0');
    char* res = c._stack;
    c._stack = NULL;
    c._top = 0;
    return res;
}


void Generator::stringnify_value(Context& c, const Value* v){
    
    ValueType tp = v->get_type();

    switch(tp){
        case JSON_FALSE: c.put_string("false", 5); break;
        case JSON_TRUE:  c.put_string("true", 4); break;
        case JSON_NULL:  c.put_string("null", 4); break;
        case JSON_NUMBER: c._top -= 32 - sprintf((char*)c.Push(32), "%.17g", v->get_number());break;
        case JSON_STRING: stringnify_string(c, v->get_string(), v->get_string_length()); break;
        case JSON_ARRAY:  
            c.put_char('[');
            for(size_t i = 0; i < v->get_array_size();i++){
                if(i>0){
                    c.put_char(',');
                }
                stringnify_value(c, v->get_array_element(i));
            }
            c.put_char(']');
            break;
        case JSON_OBJECT: 
            c.put_char('{');
            for(size_t i = 0;i < v->get_object_size();i++){
                if(i>0){
                    c.put_char(',');
                }
                stringnify_string(c, v->get_object_key(i), v->get_object_key_length(i));
                c.put_char(':');
                stringnify_value(c, v->get_object_value(i));
            }
            c.put_char('}');
            break;
        default: assert(0 && "invalid type");
    }
}

void Generator::stringnify_string(Context& c, const char* str, size_t len){
    size_t i;
    assert(str != NULL);
    c.put_char('"');
    for(i = 0; i < len; i++){
        unsigned char ch = (unsigned char)str[i];
        switch(ch){
            case '\"': c.put_string("\\\"", 2); break;
            case '\\': c.put_string("\\\\", 2); break;
            case '\b': c.put_string("\\b", 2); break;
            case '\f': c.put_string("\\f", 2); break;
            case '\n': c.put_string("\\n", 2); break;
            case '\r': c.put_string("\\r", 2); break;
            case '\t': c.put_string("\\t", 2); break;
            default:
                if(ch < 0x20){
                    char buffer[7];
                    sprintf(buffer, "\\u%04X", ch);
                    c.put_string(buffer, 6);
                }
                else{
                    c.put_char(str[i]);
                }
        }
    }
    c.put_char('"');
}

MZTKNJSON_NAMESPACE_END