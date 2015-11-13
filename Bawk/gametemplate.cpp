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

GameTemplate::GameTemplate() {
    pos = fvec3(0, 0, 0);   // unlike any other objects, this is alwas 0-aligned then
    entity_class = 5;
}

int GameTemplate::get_chunk(block_type to_arr[CX][CY][CZ], int x, int y, int z) {
    get_empty_chunk(to_arr);
    return 0;
}

int GameTemplate::save_chunk(block_type from_arr[CX][CY][CZ], int x, int y, int z) {
    // do nothing, but this should not be called
    return 0;
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
    if (PlaceableSuperObject::set_blocks(game)) {
        game->world->remove_entity(this);
    }
}

void GameTemplate::unpublish(World* world) {
    world->remove_entity(this);
}

void GameTemplate::render(fmat4* transform) {
    //set_shader_intensity(1.0f);
    RenderableSuperObject::render(transform);
}

