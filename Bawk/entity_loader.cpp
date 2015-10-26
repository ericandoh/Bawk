//
//  entity_loader.cpp
//  Bawk
//
//  Created by Eric Oh on 10/26/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "entity_loader.h"
#include "superobject.h"
//#include "player.h"
//#include "base_world.h"
//#include "cursorsuperobject.h"

Entity* get_entity_from(uint32_t pid, uint32_t vid, int entity_class) {
    Entity* val;
    if (entity_class == 0) {
        val = new Entity();
        val->pid = pid;
        val->vid = vid;
        printf("Warning: Initializing a generic Entity object (%d,%d) of class %d\n", pid, vid, entity_class);
    }
    else if (entity_class == 1) {
        // we need a world name...
        // val = new BaseWorld("unknown");
        // the base world should NOT be loaded from this pathway
        printf("Warning: Attempted to load a base world from this pathway (%d,%d) of class %d\n", pid, vid, entity_class);
        return 0;
    }
    else if (entity_class == 2) {
        // val = new Player(pid);
        printf("Warning: Attempted to load a player from this pathway (%d,%d) of class %d\n", pid, vid, entity_class);
        return 0;
    }
    else if (entity_class == 3) {
        // val = new CursorSuperObject(pid, vid);
        printf("Warning: Attempted to load a cursorsuperobject from this pathway (%d,%d) of class %d\n", pid, vid, entity_class);
        return 0;
    }
    else if (entity_class == 4) {
        val = new SuperObject(pid, vid);
    }
    else {
        printf("Unknown entity class %d for (%d,%d)\n", entity_class, pid, vid);
        return 0;
    }
    
    if (val->load_selfs()) {
        // nonzero error encountered while loading
        printf("Couldn't load entity (%d,%d) of class %d\n", pid, vid, entity_class);
        return 0;
    }
    return val;
}