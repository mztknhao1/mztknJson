/*
 * @Author: your name
 * @Date: 2021-04-02 20:35:07
 * @LastEditTime: 2021-04-06 17:19:05
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /mztknJson/mztknJson/value.cpp
 */
#include "mztknJson.h"
#include "value.h"
#include "parser.h"

namespace mztknJson{

void Value::set_null(){
    _type = JSON_NULL;
}

ValueType Value::get_type() const{
    return _type;
}

void Value::set_type(ValueType vt){
    _type = vt;
}

void Value::Free(){
    switch(_type){
        case JSON_STRING:
            free(_s.s);
            _s.s = NULL;
            break;
        case JSON_ARRAY:
            for(size_t i = 0;i < _a.size; i++){
                _a.e[i].Free();
            }
            free(_a.e);
            break;
        case JSON_OBJECT:
            for(size_t i = 0;i < _o.size;i++){
                free(_o.m[i].k);        //释放Key
                _o.m[i].v.Free();
            }
            free(_o.m);
            break;
        default: break;
    }
    _type = JSON_NULL;
    _a.e = NULL;
}

int Value::get_boolean(){
    assert(_type == JSON_TRUE || _type==JSON_FALSE);
    if(_type == JSON_TRUE){
        return 1;
    }else if(_type == JSON_FALSE){
        return 0;
    }
    return 0;
}

void Value::set_boolean(int b){
    Free();
    _type = b?JSON_TRUE:JSON_FALSE;
}

double Value::get_number() const {
    assert(_type == JSON_NUMBER);
    double n = _n;
    return n;
}

void Value::set_number(double n){
    Free();
    _n = n;
    _type = JSON_NUMBER;
}

const char* Value::get_string() const{
    assert(_type == JSON_STRING);
    return _s.s;
}
size_t Value::get_string_length() const{
    assert(_type == JSON_STRING);
    size_t len = _s.len;
    return len;
}

size_t Value::get_array_size() const {
    assert(_type == JSON_ARRAY);
    size_t sz = _a.size;
    return sz;
}

Value* Value::get_array_element(size_t index) const{
    assert(_type == JSON_ARRAY);
    assert(index < _a.size);
    return &_a.e[index];
}

Value* Value::get_object_value(size_t index) const{
    assert(index < _o.size);
    return &_o.m[index].v;
}

const char* Value::get_object_key(size_t index) const{
    assert(index < _o.size);
    return (const char*)_o.m[index].k;
}

// 保留长度的原因在于为了支持utf-8, 可能会存在\u0000的空字符，不能直接通过'\0'获得字符长度
size_t Value::get_object_key_length(size_t index) const{
    assert(index < _o.size);
    return _o.m[index].klen;
}

size_t Value::get_object_size() const{
    return _o.size;
}

void Value::set_string(const char* s, size_t len){
    assert(s!=NULL || len==0);
    Free();
    _s.s = (char*)malloc(len+1);
    memcpy(_s.s, s, len);
    _s.s[len] = '\0';
    _s.len = len;
    _type = JSON_STRING;
}


}

