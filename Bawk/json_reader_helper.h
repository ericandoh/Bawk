//
//  json_reader_helper.h
//  Bawk
//
//  Created by Eric Oh on 12/12/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__json_reader_helper__
#define __Bawk__json_reader_helper__

#include <stdio.h>
#include <string>

namespace Json {
    class Value;
}

int json_read_int_or_zero(Json::Value value);
float json_read_float_or_zero(Json::Value value);
std::string json_read_string_or_empty(Json::Value value);

void json_read_int_safe(int* dst, Json::Value value, std::string const &key);
void json_read_float_safe(float* dst, Json::Value value, std::string const &key);
std::string json_read_string_safe(Json::Value value, std::string const &key);


#endif /* defined(__Bawk__json_reader_helper__) */
