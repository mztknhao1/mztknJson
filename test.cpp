/*
 * @Author: your name
 * @Date: 2021-03-30 18:57:45
 * @LastEditTime: 2021-04-01 10:04:35
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
#define EXPECT_EQ_DOUBLE(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%.17g")

#define TEST_NULL(expect, json) \
        do{\
            Parser p; \
            Value  v; \
            v.set_type(JSON_FALSE); \
            EXPECT_EQ_INT(PARSE_OK, p.parse(&v, json)); \
            EXPECT_EQ_INT(JSON_NULL, p.get_type(&v)); \
        }while(0)


#define TEST_NUMBER(expect, json)\
        do{\
            Parser p;  \
            Value  v;  \
            v.set_type(JSON_FALSE);\
            EXPECT_EQ_INT(PARSE_OK, p.parse(&v, json)); \
            EXPECT_EQ_INT(JSON_NUMBER, p.get_type(&v)); \
            EXPECT_EQ_DOUBLE(expect, p.get_number(&v)); \
        }while(0)

#define TEST_ERROR(error, json)\
        do{ \
            Parser p;\
            Value  v;\
            v.set_type(JSON_FALSE);\
            EXPECT_EQ_INT(error, p.parse(&v, json));\
            EXPECT_EQ_INT(JSON_NULL, p.get_type(&v));\
        }while(0)

#define EXPECT_EQ_STRING(expect, actual, alength) \
    EXPECT_EQ_BASE(sizeof(expect) - 1 == alength && memcmp(expect, actual, alength)==0, expect, actual, "%s")

#define TEST_STRING(expect, json) \
        do{\
            Parser p; \
            Value  v; \
            v.set_type(JSON_FALSE);\
            EXPECT_EQ_INT(PARSE_OK, p.parse(&v, json));\
            EXPECT_EQ_INT(JSON_STRING, p.get_type(&v)); \
            EXPECT_EQ_STRING(expect, v.get_string(), v.get_string_length());\
        }while(0);

#define EXPECT_TRUE(actual) EXPECT_EQ_BASE((actual)!=0, "true", "false", "%s")
#define EXPECT_FALSE(actual) EXPECT_EQ_BASE((actual)==0, "false", "true", "%s")

static void test_parse_null(){
    TEST_NULL(PARSE_OK, "null");
}

static void test_access_boolean(){
    Parser p;
    Value v;
    v.set_type(JSON_FALSE);
    p.parse(&v, "true");
    EXPECT_TRUE(v.get_boolean());

    v.set_boolean(0);
    EXPECT_FALSE(v.get_boolean());
    
    Value v1;
    v1.set_type(JSON_TRUE);
    p.parse(&v, "false");
    EXPECT_FALSE(v.get_boolean());
}

static void test_access_number(){
    Parser p;
    Value v;
    v.set_string("a", 1);
    v.set_number(1234.5);
    EXPECT_EQ_DOUBLE(1234.5, v.get_number());
}

static void test_parse_true(){
    Parser p;
    Value v;
    v.set_type(JSON_FALSE);
    EXPECT_EQ_INT(PARSE_OK, p.parse(&v, "true"));
    EXPECT_EQ_INT(JSON_TRUE, p.get_type(&v));
}

static void test_parse_false(){
    Parser p;
    Value v;
    v.set_type(JSON_FALSE);
    EXPECT_EQ_INT(PARSE_OK, p.parse(&v, "false"));
    EXPECT_EQ_INT(JSON_FALSE, p.get_type(&v));
}

static void test_parse_expect_value() {
    TEST_ERROR(PARSE_EXPECT_VALUE, "");
    TEST_ERROR(PARSE_EXPECT_VALUE, " ");
}

static void test_parse_number(){
    TEST_NUMBER(0.0, "0");
    TEST_NUMBER(0.0, "-0");
    TEST_NUMBER(0.0, "-0.0");
    TEST_NUMBER(1.0, "1");
    TEST_NUMBER(-1.0, "-1");
    TEST_NUMBER(1.5, "1.5");
    TEST_NUMBER(-1.5, "-1.5");
    TEST_NUMBER(3.1416, "3.1416");
    TEST_NUMBER(1E10, "1E10");
    TEST_NUMBER(1e10, "1e10");
    TEST_NUMBER(1E+10, "1E+10");
    TEST_NUMBER(1E-10, "1E-10");
    TEST_NUMBER(-1E10, "-1E10");
    TEST_NUMBER(-1e10, "-1e10");
    TEST_NUMBER(-1E+10, "-1E+10");
    TEST_NUMBER(-1E-10, "-1E-10");
    TEST_NUMBER(1.234E+10, "1.234E+10");
    TEST_NUMBER(1.234E-10, "1.234E-10");
    TEST_NUMBER(0.0, "1e-10000");

    TEST_NUMBER(1.0000000000000002, "1.0000000000000002"); /* the smallest number > 1 */
    TEST_NUMBER( 4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
    TEST_NUMBER(-4.9406564584124654e-324, "-4.9406564584124654e-324");
    TEST_NUMBER( 2.2250738585072009e-308, "2.2250738585072009e-308");  /* Max subnormal double */
    TEST_NUMBER(-2.2250738585072009e-308, "-2.2250738585072009e-308");
    TEST_NUMBER( 2.2250738585072014e-308, "2.2250738585072014e-308");  /* Min normal positive double */
    TEST_NUMBER(-2.2250738585072014e-308, "-2.2250738585072014e-308");
    TEST_NUMBER( 1.7976931348623157e+308, "1.7976931348623157e+308");  /* Max double */
    TEST_NUMBER(-1.7976931348623157e+308, "-1.7976931348623157e+308");
}

static void test_parse_root_not_singular(){
    TEST_ERROR(PARSE_ROOT_NOT_SINGULAR, "null x");

    /* invalid number */
    TEST_ERROR(PARSE_ROOT_NOT_SINGULAR, "0123"); /* after zero should be '.' or nothing */
    TEST_ERROR(PARSE_ROOT_NOT_SINGULAR, "0x0");
    TEST_ERROR(PARSE_ROOT_NOT_SINGULAR, "0x123");
}

static void test_parse_number_too_big() {
    TEST_ERROR(PARSE_NUMBER_TOO_BIG, "1e309");
    TEST_ERROR(PARSE_NUMBER_TOO_BIG, "-1e309");
}

static void test_parse_string(){
    // TEST_STRING("", "\"\"");
    TEST_STRING("Hello", "\"Hello\"");
}

static void test_parse_invalid_value() {
    TEST_ERROR(PARSE_INVALID_VALUE, "nul");
    TEST_ERROR(PARSE_INVALID_VALUE, "?");

    /* invalid number */
    TEST_ERROR(PARSE_INVALID_VALUE, "+0");
    TEST_ERROR(PARSE_INVALID_VALUE, "+1");
    TEST_ERROR(PARSE_INVALID_VALUE, ".123"); /* at least one digit before '.' */
    TEST_ERROR(PARSE_INVALID_VALUE, "1.");   /* at least one digit after '.' */
    TEST_ERROR(PARSE_INVALID_VALUE, "INF");
    TEST_ERROR(PARSE_INVALID_VALUE, "inf");
    TEST_ERROR(PARSE_INVALID_VALUE, "NAN");
    TEST_ERROR(PARSE_INVALID_VALUE, "nan");
}



static void test_parse_invalid_string_char(){
    TEST_ERROR(PARSE_INVALID_STRING_CHAR, "\"\x01\"");
    TEST_ERROR(PARSE_INVALID_STRING_CHAR, "\"\x1F\"");
}

static void test_parse_invalid_string_escape() {
    TEST_ERROR(PARSE_INVALID_STRING_ESCAPE, "\"\\v\"");
    TEST_ERROR(PARSE_INVALID_STRING_ESCAPE, "\"\\'\"");
    TEST_ERROR(PARSE_INVALID_STRING_ESCAPE, "\"\\0\"");
    TEST_ERROR(PARSE_INVALID_STRING_ESCAPE, "\"\\x12\"");
}

static void test_parse_missing_quotation_mark() {
    TEST_ERROR(PARSE_MISS_QUOTATION_MARK, "\"");
    TEST_ERROR(PARSE_MISS_QUOTATION_MARK, "\"abc");
}

static void test_parse(){
    test_parse_null();
    test_parse_true();
    test_parse_false();
    test_parse_number();
    test_parse_number_too_big();
    test_parse_expect_value();
    test_parse_invalid_value();
    test_parse_root_not_singular();
    test_parse_string();

    test_access_number();
    test_access_boolean();
    test_parse_invalid_string_char();
    test_parse_missing_quotation_mark();
    test_parse_invalid_string_escape();
}

int main(){
    test_parse();
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}