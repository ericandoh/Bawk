//
//  placeablesuperobject.cpp
//  Bawk
//
//  Created by Eric Oh on 11/11/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "placeablesuperobject.h"
#include "game_info_loader.h"
#include "world.h"
#include "gametemplate.h"
#include "game.h"
#include "modelentityrender.h"

// for game templates
PlaceableSuperObject::PlaceableSuperObject(): SuperObject() {
    vid = 0;
    pid = 0;
    makes_vehicle = 0;
    can_rotate = true;
}

// for recipes
PlaceableSuperObject::PlaceableSuperObject(uint32_t s): SuperObject() {
    vid = s;
    pid = 0;
    makes_vehicle = 0;
    can_rotate = true;
}

// for custom cursorsuperobjects
PlaceableSuperObject::PlaceableSuperObject(uint32_t p, uint32_t s): SuperObject(p, s) {
    makes_vehicle = 0;
    can_rotate = true;
}

// --- PlaceableSuperObject ---
void PlaceableSuperObject::move_template(ivec3 dir) {
    pos.x += dir.x;
    pos.y += dir.y;
    pos.z += dir.z;
    recalculate_transform();
}

void PlaceableSuperObject::rotate_template() {
    angle.apply_angles(fvec3(RIGHT_ANGLE, 0, 0));
}

void PlaceableSuperObject::render_blocks(fmat4* transform) {
    render(transform);
}

// --- SuperObject ---
void PlaceableSuperObject::add_entity(Entity* entity) {
    SuperObject::add_entity(entity);
    if (entity->entity_class == EntityType::MODELENTITY) {
        ModelEntity* ent = (ModelEntity*)entity;
        if (ent->model->vehicle) {
            makes_vehicle++;
        }
    }
}

void PlaceableSuperObject::remove_entity(Entity* entity) {
    SuperObject::remove_entity(entity);
    if (entity->entity_class == EntityType::MODELENTITY) {
        ModelEntity* ent = (ModelEntity*)entity;
        if (ent->model->vehicle) {
            makes_vehicle--;
        }
    }
}

int PlaceableSuperObject::get_chunk(block_type to_arr[CX][CY][CZ], int x, int y, int z) {
    get_empty_chunk(to_arr);
    return 0;
}

void PlaceableSuperObject::handle_block_addition(int x, int y, int z, block_type type) {
    // TODO deprecate this - all vehicle mounts SHOULD be models
    // (you cant really ride a block!)
    SuperObject::handle_block_addition(x, y, z, type);
    if (get_block_independence(type.type)) {
        makes_vehicle++;
    }
}

void PlaceableSuperObject::handle_block_removal(int x, int y, int z, block_type type) {
    // TODO deprecate this - all vehicle mounts SHOULD be models
    // (you cant really ride a block!)
    SuperObject::handle_block_removal(x, y, z, type);
    if (get_block_independence(type.type)) {
        makes_vehicle--;
    }
}

void PlaceableSuperObject::update_chunks(fvec3* start_pos) {
    // always render all objects in PlaceableSuperObject
    // do nothing
}

// --- PlaceableObject ---
fvec3 PlaceableSuperObject::calculate_center_position(BlockOrientation pointing) {
    // TODO this doesnt keep take into account what direction we're pointing in...
    // calculate the center position of this object
    
    // first, find direction we're pointing in
    // 0 - x, 1 - y, 2 - z
    int pointing_in;
    if (pointing == BlockOrientation::FRONT || pointing == BlockOrientation::BACK) {
        // x
        pointing_in = 0;
    }
    else if (pointing == BlockOrientation::LEFT || pointing == BlockOrientation::RIGHT) {
        // z
        pointing_in = 2;
    }
    else {
        // y
        pointing_in = 1;
    }
    
    fvec3 aligned_upper = fvec3(bounds.upper.x - bounds.lower.x,
                                bounds.upper.y - bounds.lower.y,
                                bounds.upper.z - bounds.lower.z);
    bool align_to_half = false;
    // next, find out if we round to nearest x.5 or to nearest x.0
    if (pointing_in == 0) {
        // use z
        int rounded_val = roundf(aligned_upper.z);
        if (rounded_val % 2 == 1) {
            align_to_half = true;
        }
    }
    else if (pointing_in == 1) {
        // use y
        int rounded_val = roundf(aligned_upper.y);
        if (rounded_val % 2 == 1) {
            align_to_half = true;
        }
    }
    else {
        // use x
        int rounded_val = roundf(aligned_upper.x);
        if (rounded_val % 2 == 1) {
            align_to_half = true;
        }
    }
    
    if (align_to_half) {
        fvec3 result(roundf(aligned_upper.x / 2.0f - 0.5f) + 0.5f,
                     roundf(aligned_upper.y / 2.0f - 0.5f) + 0.5f,
                     roundf(aligned_upper.z / 2.0f - 0.5f) + 0.5f);
        return result;
    }
    else {
        fvec3 result(roundf(aligned_upper.x / 2.0f),
                     roundf(aligned_upper.y / 2.0f),
                     roundf(aligned_upper.z / 2.0f));
        return result;
    }
}

bool PlaceableSuperObject::set_blocks(Player* player, World* world, SuperObject* object) {
    if (world->will_collide_with_anything(this)) {
        return false;
    }
    
    SuperObject* target = object;
    bool create_entity = (makes_vehicle && target->entity_class == EntityType::BASEWORLD) || this->independent;
    if (create_entity) {
        // all placeables will be positioned at 0 so we can simply transpose as so
        target = new SuperObject(player->getID(), player->assignID());
        target->pos = fvec3(this->pos.x + this->bounds.lower.x,
                            this->pos.y + this->bounds.lower.y,
                            this->pos.z + this->bounds.lower.z);
        target->center_pos = calculate_center_position(BlockOrientation::FRONT);
        target->angle = angle;
        target->recalculate_transform();
    }
    this->copy_into(player, target);
    if (create_entity) {
        // now place target into object's entity list
        object->add_entity(target);
    }
    printf("PlaceableSuperObject debug\n");
    this->print_debug();
    target->print_debug();
    return true;
}