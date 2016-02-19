//
//  common_accessor.cpp
//  Bawk
//
//  Created by Eric Oh on 2/18/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "common_accessor.h"

#include "engine_accessor.h"

// TODO modify this so it returns the world for the client from the client side
// and the world for the engine from the engine (server) side

// i.e. this code is shared between the client + server but the world reference is called through the same pathway (here)
World* get_world() {
    return get_engine()->get_world();
}