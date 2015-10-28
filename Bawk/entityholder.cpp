//
//  entityholder.cpp
//  Bawk
//
//  Created by Eric Oh on 10/16/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "entityholder.h"
#include "entity_loader.h"

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
        old_pos = entities.at(i)->step();
        if (!entities[i]->has_moved())
            continue;
        legal = true;
        for (unsigned int j = 0; j < i; j++) {
            if (entities.at(j)->collides_with(entities.at(i))) {
                legal = false;
                //legal = true;
                break;
            }
        }
        if (!legal) {
            entities.at(i)->pos = old_pos;
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

int EntityHolder::load_self(IODataObject* obj) {
    int entities_count = obj->read_value<int>();
    for (int i = 0; i < entities_count; i++) {
        // load in pid, vid, entity_class in that order
        uint32_t pid = obj->read_value<uint32_t>();
        uint32_t vid = obj->read_value<uint32_t>();
        int entity_class = obj->read_value<int>();
        Entity* entity = get_entity_from(pid, vid, entity_class);
        if (entity)
            entities.push_back(entity);
    }
    return 0;
}

void EntityHolder::remove_self(IODataObject* obj) {
    
    for (int i = 0; i < entities.size(); i++) {
        entities[i]->remove_selfs();
    }
    
    int entities_count = (int)entities.size();
    for (int i = 0; i < entities_count; i++) {
        // remove from entity count all entities that are baseworld or player
        // because we don't want to save those!
        if (entities[i]->entity_class == 1 || entities[i]->entity_class == 2) {
            entities_count--;
        }
    }
    
    obj->save_value(entities_count);
    for (int i = 0; i < (int)entities.size(); i++) {
        if (entities[i]->entity_class == 1 || entities[i]->entity_class == 2)
            continue;
        obj->save_value(entities[i]->pid);
        obj->save_value(entities[i]->vid);
        obj->save_value(entities[i]->entity_class);
    }
}