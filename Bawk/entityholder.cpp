//
//  entityholder.cpp
//  Bawk
//
//  Created by Eric Oh on 10/16/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "entityholder.h"

EntityHolder::~EntityHolder() {
    for (int i = 0; i < entities.size(); i++) {
        entities[i]->remove_selfs();
        delete entities[i];
    }
}

void EntityHolder::set_global_entity(Entity* entity) {
    global_entity = entity;
    entities.push_back(entity);
}

void EntityHolder::add_entity(Entity* entity) {
    entities.push_back(entity);
}

void EntityHolder::remove_entity(Entity* entity) {
    for (unsigned int i = 0; i < entities.size(); i++) {
        if (entities.at(i) == entity) {
            entities.erase(entities.begin() + i);
            break;
        }
    }
}

void EntityHolder::step() {
    // TODO better collision detection than the classic, nested for loop
    fvec3 old_pos;
    bool legal;
    for (unsigned int i = 0; i < entities.size(); i++) {
        if (!entities[i]->has_moved())
            continue;
        old_pos = entities.at(i)->step();
        legal = true;
        for (unsigned int j = 0; j < i; j++) {
            if (entities.at(j)->collides_with(entities.at(i))) {
                legal = false;
                break;
            }
        }
        if (!legal) {
            entities.at(i)->set_pos(old_pos);
        }
    }
    
    //for (auto &i : entities) { }
}

void EntityHolder::render(fmat4* transform) {
    for (int i = 0; i < entities.size(); i++) {
        entities[i]->render(transform);
    }
}

void EntityHolder::update_chunks(fvec3* old_pos, fvec3* new_pos) {
    for (int i = 0; i < entities.size(); i++) {
        entities[i]->update_chunks(old_pos, new_pos);
    }
}

bool EntityHolder::collides_with(Entity* entity) {
    for (unsigned int i = 0; i < entities.size(); i++) {
        if (entities[i] != entity && entities[i]->collides_with(entity)) {
            return true;
        }
    }
    return false;
}

Entity* EntityHolder::poke(float x, float y, float z) {
    for (unsigned int i = 0; i < entities.size(); i++) {
        if (entities[i]->poke(x, y, z)) {
            return entities[i];
        }
    }
    return 0;
}

void EntityHolder::remove_selfs() {
    for (int i = 0; i < entities.size(); i++) {
        entities[i]->remove_selfs();
    }
}