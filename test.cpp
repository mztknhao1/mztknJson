/*
 * @Author: your name
 * @Date: 2021-03-30 18:57:45
 * @LastEditTime: 2021-03-30 20:07:32
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /mztknJson/test.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./mztknJson/mztknJson.h"
#include "./mztknJson/parser.h"

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

using namespace mztknJson;

#define EXPECT_EQ_BASE(equality, expect, actual, format)  \
    do{\
        test_count++; \
        if(equality) \
            test_pass++; \
        else{\
            fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual);\
        }\
    } while(0)

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d")

static void test_parse_null(){
    Parser p;
    Value v;
    v._type = JSON_FALSE;
    EXPECT_EQ_INT(PARSE_OK, p.parse(&v, "null"));
    EXPECT_EQ_INT(JSON_NULL, p.get_type(&v));
}

static void test_parse_true(){
    Parser p;
    Value v;
    v._type = JSON_FALSE;
    EXPECT_EQ_INT(PARSE_OK, p.parse(&v, "true"));
    EXPECT_EQ_INT(JSON_TRUE, p.get_type(&v));
}

static void test_parse_false(){
    Parser p;
    Value v;
    v._type = JSON_FALSE;
    EXPECT_EQ_INT(PARSE_OK, p.parse(&v, "false"));
    EXPECT_EQ_INT(JSON_FALSE, p.get_type(&v));
}



static void test_parse_invalid_value(){
    Parser p;
    Value v;
    v._type = JSON_FALSE;
    EXPECT_EQ_INT(PARSE_INVALID_VALUE, p.parse(&v, "nul"));
    EXPECT_EQ_INT(JSON_NULL, p.get_type(&v));

    v._type = JSON_FALSE;
    EXPECT_EQ_INT(PARSE_INVALID_VALUE, p.parse(&v, "?"));
    EXPECT_EQ_INT(JSON_NULL, p.get_type(&v));
}

static void test_parse_expect_value(){
    Parser p;
    Value v;
    v._type = JSON_FALSE;
    EXPECT_EQ_INT(PARSE_EXPECT_VALUE, p.parse(&v, " "));
    EXPECT_EQ_INT(JSON_NULL, p.get_type(&v));

    v._type = JSON_FALSE;
    EXPECT_EQ_INT(PARSE_EXPECT_VALUE, p.parse(&v, "  "));
    EXPECT_EQ_INT(JSON_NULL, p.get_type(&v));
}


static void test_parse_root_not_singular(){
    Parser p;
    Value v;
    v._type = JSON_FALSE;
    EXPECT_EQ_INT(PARSE_ROOT_NOT_SINGULAR, p.parse(&v, "null x"));
    EXPECT_EQ_INT(JSON_NULL, p.get_type(&v));
}

static void test_parse(){
    test_parse_null();
    test_parse_true();
    test_parse_false();
    test_parse_expect_value();
    test_parse_invalid_value();
    test_parse_root_not_singular();
}

int main(){
    test_parse();
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}