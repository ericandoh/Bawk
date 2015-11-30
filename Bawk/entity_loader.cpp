//
//  entity_loader.cpp
//  Bawk
//
//  Created by Eric Oh on 10/26/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "entity_loader.h"
#include "superobject.h"
#include "player.h"
#include "base_world.h"
#include "gametemplate.h"
#include "cursorsuperobject.h"
#include "world.h"
#include "modelentityrender.h"

Entity* get_entity_from(uint32_t pid, uint32_t vid, EntityType entity_class) {
    Entity* val;
    if (entity_class == EntityType::ENTITY) {
        val = new Entity();
        val->pid = pid;
        val->vid = vid;
        printf("Warning: Initializing a generic Entity object (%d,%d) of class %d\n", pid, vid, entity_class);
    }
    else if (entity_class == EntityType::BASEWORLD) {
        val = new BaseWorld();
    }
    else if (entity_class == EntityType::PLAYER) {
        val = new Player(pid);
    }
    else if (entity_class == EntityType::SUPEROBJECT) {
        val = new SuperObject(pid, vid);
    }
    else if (entity_class == EntityType::CURSORSUPEROBJECT) {
        val = new CursorSuperObject(pid, vid);
        printf("Warning: Attempted to load a cursorsuperobject from this pathway (%d,%d) of class %d\n", pid, vid, entity_class);
    }
    else if (entity_class == EntityType::GAMETEMPLATE) {
        val = new GameTemplate();
        printf("Warning: Attempted to load a gameTemplate from this pathway (%d,%d) of class %d\n", pid, vid, entity_class);
    }
    else if (entity_class == EntityType::MODELENTITY) {
        val = new ModelEntity(pid, vid);
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

Entity* copy_entity(Player* player, Entity* src) {
    EntityType entity_class = src->entity_class;
    if (entity_class == EntityType::BASEWORLD) {
        // copy world
        printf("Copying world not yet supported");
        BaseWorld* world = new BaseWorld();
        ((SuperObject*)src)->copy_into(player, world);
        return world;
    }
    else if (entity_class == EntityType::PLAYER) {
        // copy player and player inventory
        // do NOT implement - no need to copy player
        printf("Copying player not supported\n");
    }
    else if (entity_class == EntityType::SUPEROBJECT) {
        // copy superobject
        // copy all blocks/chunks over
        // copy all subentities
        SuperObject* result;
        if (player)
            result = new SuperObject(player->getID(), player->assignID());
        else
            result = new SuperObject(0, player->assignID());
        result->pos = src->pos;
        result->angle = src->angle;
        result->center_pos = src->center_pos;
        ((SuperObject*)src)->copy_into(player, result);
        return result;
    }
    else if (entity_class == EntityType::CURSORSUPEROBJECT) {
        // do NOT implement - no need to copy cursorobject
        printf("Copying cursorobject not supported\n");
    }
    else if (entity_class == EntityType::GAMETEMPLATE) {
        // implement later?
        printf("Copying game templates not supported");
    }
    else {
        printf("Couldn't copy entity (%d,%d) of class %d\n", src->pid, src->vid, entity_class);
    }
    return 0;
}

void delete_entity_from_memory(Entity* entity) {
    EntityType entity_class = entity->entity_class;
    // we should not erase entity class 0 => undefined
    if (entity_class == EntityType::BASEWORLD) {
        // delete world
        printf("Deleting a baseworld entity object, are you sure you want to do this?\n");
        delete_at_path(get_path_to_world_folder());
    }
    else if (entity_class == EntityType::PLAYER) {
        // delete player
        printf("Deleting a player, are you sure you want to do this?\n");
        delete_at_path(get_path_to_player_folder(entity->pid));
    }
    else if (entity_class == EntityType::SUPEROBJECT) {
        // delete superobject
        printf("Deleting a superobject\n");
        for (Entity* ent: ((SuperObject*)entity)->entities) {
            delete_entity_from_memory(ent);
        }
        delete_at_path(get_path_to_superobj_folder(entity->pid, entity->vid));
    }
    else if (entity_class == EntityType::SUPEROBJECT) {
        printf("Deleting a cursorsuperobject...\n");
        delete_at_path(get_path_to_template_folder(entity->pid, entity->vid));
    }
    else if (entity_class == EntityType::GAMETEMPLATE) {
        // do not delete game templates, there is nothing to delete
    }
    else {
        printf("Couldn't delete entity (%d,%d) of class %d\n", entity->pid, entity->vid, entity_class);
    }
}

/*
bool is_entity_superobject(Entity* entity) {
    return entity->entity_class == 3 || entity->entity_class == 5;
}

bool is_entity_supersuperobject(Entity* entity) {
    return entity->entity_class == 4 || entity->entity_class == 6;
}*/