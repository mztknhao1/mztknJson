/*
 * @Author: your name
 * @Date: 2021-03-30 16:19:33
 * @LastEditTime: 2021-04-02 11:01:18
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /mztknJson/mztknJson/parser.hpp
 */
#ifndef MZTKN_PARSER_H__
#define MZTKN_PARSER_H__

#include "mztknJson.h"

MZTKNJSON_NAMESPACE_BEGIN

typedef enum {JSON_NULL, JSON_FALSE, JSON_TRUE, JSON_NUMBER, JSON_STRING, JSON_ARRAY, JSON_OBJECT} ValueType;

typedef enum {
    PARSE_OK = 0,
    PARSE_EXPECT_VALUE,
    PARSE_INVALID_VALUE,
    PARSE_ROOT_NOT_SINGULAR,
    PARSE_NUMBER_TOO_BIG,
    PARSE_MISS_QUOTATION_MARK,
    PARSE_INVALID_STRING_ESCAPE,
    PARSE_INVALID_STRING_CHAR,
    PARSE_INVALID_UNICODE_HEX,
    PARSE_INVALID_UNICODE_SURROGATE,
    PARSE_MISS_COMMA_ORSQUARE_BRACKET
} ParseState;

class Value{
public:
    friend class Parser;

    Value(){_type = JSON_NULL;}
    ~Value(){
        // Free();
    }

#if 0
	Value(Value&& v) {
		if (v._type == JSON_STRING) {
			_type = JSON_STRING;
			_s.s = v._s.s;
			_s.len = v._s.len;
			v._s.s = NULL;
			v._type = JSON_NULL;
		}
		else if (v._type == JSON_ARRAY) {
			for (size_t i = 0; i<v._a.size; i++) {
				Value* p = new Value(std::move(v._a.e[i]));
				(_a.e + i) = p;
                v._a.e[i]._type = JSON_NULL;
			}
			_a.size = v._a.size;
			v._type = JSON_NULL;
		}else if(v._type==JSON_NUMBER){
            _n = v._n;
            _type = JSON_NUMBER;
        }else if(v._type==JSON_FALSE || v._type==JSON_TRUE){
            _type = v._type;
        }
	}

	Value& operator=(Value&& v) {
		if (v._type == JSON_STRING) {
			this->_type = JSON_STRING;
			_s.s = v._s.s;
			_s.len = v._s.len;
			v._s.s = NULL;
			v._type = JSON_NULL;
		}
		else if (v._type == JSON_ARRAY) {
			for (int i = 0; i<v._a.size; i++) {
				Value* p = new Value(std::move(v._a.e[i]));
				(_a.e + i) = p;
                v._a.e[i]._type = JSON_NULL;
			}
			_a.size = v._a.size;
			v._type = JSON_NULL;
		}else if(v._type==JSON_NUMBER){
            _n = v._n;
            _type = JSON_NUMBER;
        }else if(v._type==JSON_FALSE || v._type==JSON_TRUE){
            _type = v._type;
        }
		return *this;
	}
#endif

    void set_null(){
        _type = JSON_NULL;
    }
    ValueType get_type() const {
        return _type;
    }
    void set_type(ValueType vt){
        _type = vt;
    }
    void Free(){
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
            default: break;
        }
        _type = JSON_NULL;
        _a.e = NULL;
    }
    
    int get_boolean(){
        assert(_type == JSON_TRUE || _type==JSON_FALSE);
        if(_type == JSON_TRUE){
            return 1;
        }else if(_type == JSON_FALSE){
            return 0;
        }
        return 0;
    }
    void set_boolean(int b){
        Free();
        _type = b?JSON_TRUE:JSON_FALSE;
    }
    double get_number() const {
        assert(_type == JSON_NUMBER);
        double n = _n;
        return n;
    }
    void set_number(double n){
        Free();
        _n = n;
        _type = JSON_NUMBER;
    }
    const char* get_string(){
        assert(_type == JSON_STRING);
        return _s.s;
    }
    size_t get_string_length() const{
        assert(_type == JSON_STRING);
        size_t len = _s.len;
        return len;
    }

    size_t get_array_size() const {
        assert(_type == JSON_ARRAY);
        size_t sz = _a.size;
        return sz;
    }

    Value* get_array_element(size_t index){
        assert(_type == JSON_ARRAY);
        assert(index < _a.size);
        return &_a.e[index];
    }

    void set_string(const char* s, size_t len){
        assert(s!=NULL || len==0);
        Free();
        _s.s = (char*)malloc(len+1);
        memcpy(_s.s, s, len);
        _s.s[len] = '\0';
        _s.len = len;
        _type = JSON_STRING;
    }

private:
    ValueType _type;
    union{
        struct {Value* e; size_t size;}_a;
        double    _n;
        struct {char* s; size_t len;} _s;
    };

};


class Parser{
public:
    int parse(Value* v, const char* json);

    ValueType get_type(const Value* v);

    double    get_number(const Value* v);


private:
    struct Context{
        const char *_json;
        char* _stack;                    //预留的临时栈
        size_t _size, _top;
        explicit Context(const char *json){
            assert(json!=NULL);
            _json = json;
            _size = _top = 0;
            _stack = NULL;
        }
        ~Context(){
            assert(_top == 0);
            free(_stack);
        }
        void *Push(size_t sz){
            void* ret;
            assert(sz > 0);
            if(_top + sz >= _size){
                if(_size == 0){
                    _size = PARSE_STACK_INIT_SIZE;
                }
                while(_top + sz >= _size)
                    _size += _size >> 1;        // _size * 1.5
                _stack = (char*)realloc(_stack, _size);
            }
            ret = _stack + _top;
            _top += sz;
            return ret;
        }
        void put_char(char ch){
            *(char*)Push(sizeof(char)) = ch;
        }

        void *Pop(size_t sz){
            assert(_top >= sz);
            return _stack + (_top -= sz);
        }

    };

    void parse_whitespace(Context& c);

    int  parse_value(Context& c, Value* v);
    int  parse_number(Context& c, Value* v);
    int  parse_string(Context& c, Value* v);
    const char* parse_hex4(const char* p, unsigned& u);
    void encode_utf8(Context& c, unsigned u);
    int  parse_literal(Context& c, Value* v, const char* literal, ValueType type);
    int  parse_array(Context& c, Value* v);

    
};



MZTKNJSON_NAMESPACE_END

#endif