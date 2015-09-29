//
//  world.cpp
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "world.h"

// constructor
World::World() {
    age = 0;
    
    world_render = WorldRender();
}

// renders the world
void World::render() {
    world_render.render();
}

// cycles one timestep for the world
void World::step() {
    age++;
}