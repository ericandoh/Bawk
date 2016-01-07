//
//  cursormodelobject.cpp
//  Bawk
//
//  Created by Eric Oh on 11/29/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "cursormodelobject.h"
#include "block_orientation.h"
#include "blocktracer.h"
#include "superobject.h"
#include "player.h"
#include "game.h"

CursorModelObject::CursorModelObject(uint16_t mid): ModelEntity(mid) {
    locked = false;
    entity_class = EntityType::CURSORMODELENTITY;
}

// --- PlaceableObject
bool CursorModelObject::set_blocks(Player* player, World* world, SuperObject* object) {
    
    // make a new ModelObject and see if it fits
    ModelEntity* model = new ModelEntity(player->pid, player->assignID(), model_id);
    // TOFU for unitary models, set orientation of this model object to match
    //BlockOrientation player_direction = get_player_direction();
    model->pos = pos;
    model->angle = angle;
    model->recalculate_transform();
    
    // assume cursorobject is in the world - that is, there is no need to
    // transform pos/angle into RWC (world coordinates) because they already are
    if (object->collides_with(model))
        return false;
    object->add_entity(model);
    return true;
}

// --- CursorItem ---
void CursorModelObject::reset() {
    locked = false;
    show_item = false;
}

bool CursorModelObject::clicked(Game* game, Action mouse) {
    if (!locked) {
        if (show_item) {
            locked = true;
        }
        return true;
    }
    return false;
}

bool CursorModelObject::confirmed(Game* game) {
    if (!locked) {
        return false;
    }
    else {
        if (set_blocks(game->player, game->world, target)) {
            locked = false;
        }
        return true;
    }
}

bool CursorModelObject::canceled(Game* game) {
    if (locked) {
        locked = false;
        return true;
    }
    return false;
}

bool CursorModelObject::handle_movement(ivec3 dir) {
    if (!locked) {
        return false;
    }
    pos.x += dir.x;
    pos.y += dir.y;
    pos.z += dir.z;
    recalculate_transform();
    return true;
}

bool CursorModelObject::handle_rotation() {
    angle.apply_angles(fvec3(RIGHT_ANGLE, 0, 0));
    return true;
}

void CursorModelObject::step(Game* game) {
    if (!locked) {
        ivec3 upper;
        upper.x = bounds.upper.x - bounds.lower.x;
        upper.y = bounds.upper.y - bounds.lower.y;
        upper.z = bounds.upper.z - bounds.lower.z;
        update_pointing_position(game, upper);
        pos = fvec3(pointing_pos.x, pointing_pos.y, pointing_pos.z);
    }
}

void CursorModelObject::render_item() {
    if (!mvp_set) {
        ivec3 upper;
        upper.x = bounds.upper.x - bounds.lower.x;
        upper.y = bounds.upper.y - bounds.lower.y;
        upper.z = bounds.upper.z - bounds.lower.z;
        calculate_mvp(upper);
    }
    set_mvp();
    fvec3 old_pos = pos;
    // TOFU can we do something better than this?
    set_pos(fvec3(0, 0, 0));
    fmat4 one(1);
    render(&one);
    set_pos(old_pos);
}

void CursorModelObject::render_in_world(fmat4* transform) {
    if (!locked) {
        if (!show_item) {
            return;
        }
    }
    render(transform);
}

void CursorModelObject::render_light_in_world(fmat4* transform, fvec3 player_pos) {
    if (locked) {
        // TODO fix this
        render_lights(transform, player_pos);
    }
}

cursor_item_identifier CursorModelObject::get_identifier() {
    cursor_item_identifier val;
    val.is_blk = false;
    val.is_recipe = true;
    val.bid = 0;
    val.pid = pid;
    val.vid = vid;
    return val;
}

// --- ModelEntity ---
std::string CursorModelObject::get_save_path() {
    return get_path_to_template(pid, vid);
}