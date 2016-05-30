//
//  engine_accessor.cpp
//  Bawk
//
//  Created by Eric Oh on 2/18/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "engine_accessor.h"

Engine* current_engine;

void set_engine(Engine* engine) {
    current_engine = engine;
}

Engine* get_engine() {
    return current_engine;
}

World* get_engine_world() {
    return current_engine->get_world();
}