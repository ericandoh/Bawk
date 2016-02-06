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

CursorSuperObject::CursorSuperObject(CursorItemInfo* i): PlaceableSuperObject((uint32_t)i->pid, (uint32_t)i->vid), CursorItem(i) {
    locked = false;
    entity_class = EntityType::CURSORSUPEROBJECT;
}

// --- CursorItem ---
void CursorSuperObject::init() {
    this->load_selfs();
    // load in all chunks
    for (auto &i: chunk_bounds) {
        this->load_chunk(i.first.x, i.first.y, i.first.z);
    }
}

void CursorSuperObject::init(std::string path) {
    this->load_selfs(get_metadata_path(path));
    // load in all chunks
    for (auto &i: chunk_bounds) {
        ivec3 pos = i.first;
        if (chunks.count(pos)) {
            // chunk is already loaded
            continue;
        }
        // TODO this is ugly code, how can we repackage this? this is copied from
        // parts of load_chunk/get_chunk
        block_type to_arr[CX][CY][CZ];
        IODataObject reader(get_chunk_path(path, &pos));
        if (reader.read(false))
            continue;
        reader.read_pointer(&(to_arr[0][0][0]), sizeof(to_arr[0][0][0])*CX*CY*CZ);
        reader.close();
        
        RenderableChunk* chunk = new RenderableChunk(to_arr);
        if (chunk) {
            chunks[pos] = chunk;
            setup_chunk(chunk, pos.x, pos.y, pos.z);
        }
    }
}

void CursorSuperObject::cleanup() {
    // do nothing top kek
    delete this;
}

void CursorSuperObject::reset() {
    locked = false;
    show_item = false;
    
    // TODO update info->count depending on if we have the resources to make this
    info->count = 3;
}

bool CursorSuperObject::clicked(Game* game, Action mouse) {
    if (!locked) {
        if (show_item) {
            locked = true;
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
        if (set_blocks(game->player, game->world, target)) {
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

void CursorSuperObject::step(Game* game, int ms) {
    // try locking this object into position
    if (!locked) {
        ivec3 upper;
        upper.x = bounds.upper.x - bounds.lower.x;
        upper.y = bounds.upper.y - bounds.lower.y;
        upper.z = bounds.upper.z - bounds.lower.z;
        update_pointing_position(game, upper);
        pos = fvec3(pointing_pos.x, pointing_pos.y, pointing_pos.z);
    }
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
        calculate_mvp(upper);
    }
    set_mvp();
    fvec3 old_pos = pos;
    set_pos(-bounds.lower);
    fmat4 one(1);
    render(&one);
    set_pos(old_pos);
}

void CursorSuperObject::render_in_world(fmat4* transform) {
    if (!locked) {
        if (!show_item) {
            return;
        }
    }
    render_blocks(transform);
}

void CursorSuperObject::render_light_in_world(fmat4* transform, fvec3 player_pos) {
    if (locked || show_item) {
        render_lights(transform, player_pos);
    }
}

bool CursorSuperObject::has_count() {
    return true;
}

// --- SuperObject ---
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

void CursorSuperObject::save_self(IODataObject* obj) {
    SuperObject::save_self(obj);
    obj->save_value(makes_vehicle);
    obj->save_value(independent);
    save_all_chunks();
}


