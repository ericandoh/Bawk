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
    
    SuperObject* world = new SuperObject();
    world->fill();
    superobjects.push_back(world);

}

World::~World() {
    for (int i = 0; i < superobjects.size(); i++) {
        delete superobjects[i];
    }
}

int World::load_resources() {
    return world_load_resources();
}

void World::free_resources() {
    world_free_resources();
}

// renders the world
void World::render(fmat4* transform) {
    for (int i = 0; i < superobjects.size(); i++) {
        superobjects[i]->render(transform);
    }
}

// cycles one timestep for the world
void World::step() {
    age++;
}