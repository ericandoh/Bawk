//
//  world.cpp
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "world.h"

// constructor
World::World() {
    printf("Adding World\n");
    age = 0;
}

int World::load_resources() {
    return world_render.load_resources();
}

void World::free_resources() {
    world_render.free_resources();
}

// renders the world
void World::render(fmat4* transform) {
    world_render.render(transform);
}

// cycles one timestep for the world
void World::step() {
    age++;
}