/*
 * @Author: your name
 * @Date: 2021-04-02 20:34:49
 * @LastEditTime: 2021-04-03 08:46:48
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /mztknJson/mztknJson/value.h
 */
#ifndef MZTKN_VALUE_H__
#define MZTKN_VALUE_H__

#include "mztknJson.h"

namespace mztknJson{

class member;

enum ValueType {JSON_NULL, JSON_FALSE, JSON_TRUE, JSON_NUMBER, JSON_STRING, JSON_ARRAY, JSON_OBJECT} ;

class Value{
public:
    friend class Parser;

    Value(){_type = JSON_NULL;}
    ~Value(){}

    void set_null();

    ValueType get_type() const;
    void set_type(ValueType vt);
    void Free();
    int get_boolean();
    void set_boolean(int b);
    double get_number() const;
    void set_number(double n);
    const char* get_string();
    size_t get_string_length() const;
    size_t get_array_size() const;
    Value* get_array_element(size_t index);
    Value* get_object_value(size_t index);
    const char* get_object_key(size_t index);
    size_t get_object_key_length(size_t index);
    size_t get_object_size();
    void set_string(const char* s, size_t len);

private:
    ValueType _type;
    union{
        struct {member* m; size_t size;}_o;
        struct {Value* e; size_t size;}_a;
        double    _n;
        struct {char* s; size_t len;} _s;
    };

};

class member{
    friend class Value;
    friend class Parser;

    member(){
        k = NULL;
        klen = 0;
    }

    char* k;
    size_t klen;
    Value v;
};

}


#endif