/*
 * @Author: your name
 * @Date: 2021-04-03 09:59:49
 * @LastEditTime: 2021-04-06 17:29:12
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /mztknJson/mztknJson/generator.h
 */
#ifndef MZTKN_GENERATOR_H__
#define MZTKN_GENERATOR_H__

#include "mztknJson.h"
#include "value.h"
#include "context_stack.hpp"

MZTKNJSON_NAMESPACE_BEGIN

class Generator{

public:
    static char* stringify(const Value* v, size_t* length=NULL);


private:
    static void stringnify_value(Context& c, const Value* v);
    static void stringnify_string(Context& c, const char* str, size_t len);
};

MZTKNJSON_NAMESPACE_END

#endif