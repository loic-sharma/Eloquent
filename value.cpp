//
//  value.cpp
//  Compiler
//
//  Created by Loic Sharma on 3/12/14.
//  Copyright (c) 2014 sharml. All rights reserved.
//

#include "value.h"
#include <cassert>

Value::Value(bool v) {
    type = BooleanTypeValue;
    value.boolean = v;
}

Value::Value(int v) {
    type = IntegerTypeValue;
    value.integer = (long)v;
}

Value::Value(long v) {
    type = IntegerTypeValue;
    value.integer = v;
}

Value::Value(double v) {
    type = DoubleTypeValue;
    value.doublez = v;
}

Value::Value(char v) {
    type = StringTypeValue;
    string_value = v;
}

Value::Value(const char *v) {
    type = StringTypeValue;
    string_value = v;
}

Value::Value(std::string v) {
    type = StringTypeValue;
    string_value = v;
}

Type Value::get_type() const {
    return type;
}

bool Value::to_bool() const {
    switch (type) {
        case BooleanTypeValue:
            return value.boolean;
            break;
            
        case IntegerTypeValue:
            return (value.integer != 0);
            break;
            
        case DoubleTypeValue:
            return (value.doublez != 0);
            break;
            
        case StringTypeValue:
            return (string_value.empty() == false);
            break;
            
        case ArrayTypeValue:            
        case UnknownTypeValue:
        case NullTypeValue:
        default:
            return false;
            break;
    }
}

long Value::to_integer() const {
    switch (type) {
        case BooleanTypeValue:
            return value.boolean ? 1 : 0;
            break;
            
        case IntegerTypeValue:
            return value.integer;
            break;
            
        case DoubleTypeValue:
            return (long) value.doublez;
            break;
            
        case StringTypeValue:
            assert(false);
            return 0;
            break;
            
        case ArrayTypeValue:
        case UnknownTypeValue:
        case NullTypeValue:
        default:
            return 0;
            break;
    }
}

double Value::to_double() const {
    return 1.0;
}

std::string Value::to_string() const {
    switch (type) {
        case BooleanTypeValue:
            return (value.boolean == true) ? "true" : "false";
            break;

        case IntegerTypeValue:
            return std::to_string(value.integer);
            break;

        case DoubleTypeValue:
            return std::to_string(value.doublez);
            break;

        case StringTypeValue:
            return string_value;
            break;

        case ArrayTypeValue:
            return "Array not done";

        case UnknownTypeValue:
        case NullTypeValue:
        default:
            return "";
            break;
    }
}
