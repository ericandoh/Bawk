//
//  common_accessor.cpp
//  Bawk
//
//  Created by Eric Oh on 2/18/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "common_accessor.h"
#include "engine_accessor.h"
#include "client_accessor.h"

bool is_client;

void set_is_client() {
    is_client = true;
}

void set_is_server() {
    is_client = false;
}

// TODO modify this so it returns the world for the client from the client side
// and the world for the engine from the engine (server) side

// i.e. this code is shared between the client + server but the world reference is called through the same pathway (here)
World* get_world() {
    if (is_client) {
        return get_client_world();
    } else {
        return get_engine_world();
    }
}