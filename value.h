//
//  value.h
//  Compiler
//
//  Created by Loic Sharma on 3/12/14.
//  Copyright (c) 2014 sharml. All rights reserved.
//

#ifndef __Eloquent_value__
#define __Eloquent_value__

#include <string>
#include <vector>

typedef enum {
    BooleanTypeValue = (1 << 0),
    IntegerTypeValue = (1 << 1),
    DoubleTypeValue  = (1 << 2),
    StringTypeValue  = (1 << 3),
    ArrayTypeValue   = (1 << 4),
    UnknownTypeValue = (1 << 5),
    NullTypeValue    = (1 << 6),
} Type;

class Value {
public:
    Value() : type(NullTypeValue) {}
    Value(bool);
    Value(int);
    Value(long);
    Value(double);
    Value(char);
    Value(const char *);
    Value(std::string);

    Type get_type() const;
    
    bool to_bool() const;
    long to_integer() const;
    double to_double() const;
    std::string to_string() const;

    operator bool () const {
        return to_bool();
    }
    
    operator long () const {
        return to_integer();
    }
    
    operator double () const {
        return to_double();
    }
    
    operator std::string () const {
        return to_string();
    }

private:
	Type type;

	union {
		bool boolean;
		long integer;
		double doublez;
	} value;
    
    std::string string_value;
};

#endif
