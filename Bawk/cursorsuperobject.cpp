//
//  cursorsuperobject.cpp
//  Bawk
//
//  Created by Eric Oh on 10/6/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "cursorsuperobject.h"
#include "blocktracer.h"
#include "game.h"
#include "block.h"
#include "gametemplate.h"

CursorSuperObject::CursorSuperObject(uint32_t s): PlaceableSuperObject(s) {
    locked = false;
    entity_class = 4;
    is_recipe = true;
}

// heavily modify this so that
// 1. it doesn't mark itself as a folder to disk
// 2. it doesn't load/save chunks to disk (superobjectrender has the capacity to hold all the chunks
//    if it wasn't for that pesky loading/unloading)
// 3. override SuperObjectRender directly instead
// 4. give superobjectrender a position attribute to scale stuff
// 5. we'll handle rotation later (fuck that shit mate)
CursorSuperObject::CursorSuperObject(uint32_t p, uint32_t s): PlaceableSuperObject(p, s) {
    locked = false;
    entity_class = 4;
    is_recipe = false;
}

// --- CursorItem ---
void CursorSuperObject::init() {
    this->load_selfs();
    // load in all chunks
    for (auto &i: chunk_bounds) {
        this->load_chunk(i.first.x, i.first.y, i.first.z);
    }
}

void CursorSuperObject::cleanup() {
    // do nothing top kek
    delete this;
}

void CursorSuperObject::reset() {
    locked = false;
}

bool CursorSuperObject::clicked(Game* game, Action mouse, Entity* on) {
    if (!locked) {
        // try locking this object into position
        ivec3 locked_pos;
        BlockOrientation orientation;
        ivec3 upper;
        upper.x = bounds.upper.x - bounds.lower.x;
        upper.y = bounds.upper.y - bounds.lower.y;
        upper.z = bounds.upper.z - bounds.lower.z;
        if (get_pointing_position(&locked_pos, &orientation, upper)) {
            locked = true;
            pos = fvec3(locked_pos.x, locked_pos.y, locked_pos.z);
        }
        return true;
    }
    return false;
}

bool CursorSuperObject::confirmed(Game* game) {
    if (!locked) {
        return false;
    }
    else {
        if (PlaceableObject::set_blocks(game)) {
            locked = false;
        }
        return true;
    }
}

bool CursorSuperObject::canceled(Game* game) {
    if (locked) {
        locked = false;
        return true;
    }
    return false;
}

// only needed for instances of template. the default does jack shit
bool CursorSuperObject::handle_movement(ivec3 dir) {
    if (!locked) {
        return false;
    }
    move_template(dir);
    return true;
}

bool CursorSuperObject::handle_rotation() {
    rotate_template();
    return true;
}

void CursorSuperObject::render_item() {
    if (!mvp_set) {
        ivec3 upper;
        upper.x = bounds.upper.x - bounds.lower.x;
        upper.y = bounds.upper.y - bounds.lower.y;
        upper.z = bounds.upper.z - bounds.lower.z;
        set_mvp(upper);
    }
    fvec3 old_pos = pos;
    pos = fvec3(0, 0, 0);
    render(&mvp);
    pos = old_pos;
}

void CursorSuperObject::render_in_world(fmat4* transform) {
    if (!locked) {
        BlockOrientation orientation;
        ivec3 upper;
        ivec3 locked_pos;
        upper.x = bounds.upper.x - bounds.lower.x;
        upper.y = bounds.upper.y - bounds.lower.y;
        upper.z = bounds.upper.z - bounds.lower.z;
        if (get_pointing_position(&locked_pos, &orientation, upper)) {
            pos = fvec3(locked_pos.x, locked_pos.y, locked_pos.z);
        }
        else {
            // out of bounds, do not render top kek
            return;
        }
    }
    render_blocks(transform);
}

cursor_item_identifier CursorSuperObject::get_identifier() {
    cursor_item_identifier val;
    val.is_blk = false;
    val.is_recipe = is_recipe;
    val.bid = 0;
    val.pid = pid;
    val.vid = vid;
    return val;
}

// --- SuperObject ---
int CursorSuperObject::get_chunk(block_type to_arr[CX][CY][CZ], int x, int y, int z) {
    if (!SuperObject::get_chunk(to_arr, x, y, z)) {
        return 0;
    }
    get_empty_chunk(to_arr);
    return 0;
}

std::string CursorSuperObject::get_save_path() {
    return get_path_to_template(pid, vid);
}

std::string CursorSuperObject::get_chunk_save_path(ivec3* pos) {
    return get_path_to_template_chunk(pid, vid, pos);
}

int CursorSuperObject::load_self(IODataObject* obj) {
    if (SuperObject::load_self(obj))
        return 1;
    makes_vehicle = obj->read_value<int>();
    independent = obj->read_value<bool>();
    return 0;
}

void CursorSuperObject::remove_self(IODataObject* obj) {
    SuperObject::remove_self(obj);
    obj->save_value(makes_vehicle);
    obj->save_value(independent);
    save_all_chunks();
}


