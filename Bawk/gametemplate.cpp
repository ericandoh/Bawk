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
#include "modelentityrender.h"

GameTemplate::GameTemplate() {
    entity_class = EntityType::GAMETEMPLATE;
}

Rotation GameTemplate::get_vehicle_orientation() {
    Rotation result = Rotation();
    for (Entity* ent: entities) {
        if (ent->entity_class == EntityType::MODELENTITY) {
            ModelEntity* model_ent = (ModelEntity*)ent;
            if (model_ent->model->vehicle) {
                // TODO handle if we have multiple vehicle blocks - maybe average the rotations?
                // then round out the rotation to nearest axis at end?
                result = ent->angle;
            }
        }
    }
    return result;
}

CursorSuperObject* GameTemplate::create_from_template(Player* player, World* world) {
    printf("Publishing template!\n");
    if (!block_counter && entities.size() <= 1) {
        printf("no blocks/entities to publish...\n");
        return 0;
    }
    // package our blocks into a cursorsuperobject
    CursorSuperObject* object = new CursorSuperObject(player->getID(),
                                                      player->assignID());// all templates are made on the bar
    // TODO set angle needed?
    if (makes_vehicle) {
        fvec3 rwc_center_pos = calculate_center_position() + bounds.lower;
        // set angle depending on where we've been pointing, and set pos differently
        Rotation vehicle_pointing = get_vehicle_orientation();
        // find bounds rotated into my rotation, from the world, rotating around rwc center pos
        bounding_box oac_bounding_box;
        oac_bounding_box.lower = vehicle_pointing.transform_into_my_coordinates(bounds.lower - rwc_center_pos);
        oac_bounding_box.upper = vehicle_pointing.transform_into_my_coordinates(bounds.upper - rwc_center_pos);
        oac_bounding_box.refit_for_rotation();
        object->center_pos = -oac_bounding_box.lower;
        object->pos = rwc_center_pos + oac_bounding_box.lower;
        object->angle = vehicle_pointing;
    }
    else {
        object->pos = bounds.lower;
        object->center_pos = calculate_center_position();
    }
    object->recalculate_transform();
    
    if (set_blocks(player, world, object)) {
        // save the object to disk
        // this doesn't actually delete the object from memory
        object->remove_selfs();
        // tell object to publish to the world, the position/orientation should already be good
        object->set_blocks(player, world, world->base_world);
        return object;
    }
    delete object;
    return 0;
}

void GameTemplate::publish(Game* game) {
    // TODO clean code
    //if (PlaceableObject::set_blocks(game)) {
    game->world->remove_entity(this);
    //}
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

