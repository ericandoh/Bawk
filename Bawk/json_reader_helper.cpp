//
//  json_reader_helper.cpp
//  Bawk
//
//  Created by Eric Oh on 12/12/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "json_reader_helper.h"
#include "json/json.h"

int json_read_int_or_zero(Json::Value value) {
    if (value.type() == Json::intValue) {
        return value.asInt();
    }
    return 0;
}

float json_read_float_or_zero(Json::Value value) {
    if (value.type() == Json::realValue || value.type() == Json::intValue) {
        return value.asFloat();
    }
    return 0.0f;
}

std::string json_read_string_or_empty(Json::Value value) {
    if (value.type() == Json::stringValue) {
        return value.asString();
    }
    return "";
}

void json_read_int_safe(int* dst, Json::Value value, std::string const &key) {
    if (value.isMember(key)) {
        *dst = json_read_int_or_zero(value[key]);
    }
}

void json_read_float_safe(float* dst, Json::Value value, std::string const &key) {
    if (value.isMember(key)) {
        *dst = json_read_float_or_zero(value[key]);
    }
}

std::string json_read_string_safe(Json::Value value, std::string const &key) {
    if (value.isMember(key)) {
        return json_read_string_or_empty(value[key]);
    }
    return "";
}
