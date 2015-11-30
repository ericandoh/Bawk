//
//  temporarytemplate.cpp
//  Bawk
//
//  Created by Eric Oh on 10/6/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "gametemplate.h"
#include "game.h"
#include "cursorsuperobject.h"
#include "world.h"

GameTemplate::GameTemplate() {
    pos = fvec3(0, 0, 0);   // unlike any other objects, this is alwas 0-aligned then
    entity_class = EntityType::GAMETEMPLATE;
}

CursorSuperObject* GameTemplate::create_from_template(Player* player, World* world) {
    printf("Publishing template!\n");
    if (!block_counter) {
        printf("no blocks to publish...\n");
        return 0;
    }
    // package our blocks into a cursorsuperobject
    // TODO do not call this if we have no blocks here
    CursorSuperObject* object = new CursorSuperObject(player->getID(),
                                                      player->assignID());// all templates are made on the bar
    object->pos = bounds.lower;
    object->center_pos = calculate_center_position();
    
    if (set_blocks(player, world, object)) {
        // save the object to disk
        // this doesn't actually delete the object from memory
        object->remove_selfs();
        return object;
    }
    delete object;
    return 0;
}

void GameTemplate::publish(Game* game) {
    if (PlaceableObject::set_blocks(game)) {
        game->world->remove_entity(this);
    }
}

void GameTemplate::unpublish(World* world) {
    world->remove_entity(this);
}

void GameTemplate::render(fmat4* transform) {
    set_shader_intensity(0.6f);
    // do any special rendering you'd like if you want to make this shine
    PlaceableSuperObject::render(transform);
    set_shader_intensity(0.3f);
}

