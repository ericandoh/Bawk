//
//  modelentityrender.cpp
//  Bawk
//
//  Created by Eric Oh on 11/29/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "modelentityrender.h"
#include "game_info_loader.h"

ModelEntity::ModelEntity(uint16_t mid) {
    entity_class = EntityType::MODELENTITY;
    in_range = true;
    
    // some permissions/flags
    can_collide = true;
    can_rotate = true;
    
    set_model(mid);
}

ModelEntity::ModelEntity(uint32_t p, uint32_t v) {
    vid = v;
    pid = p;
    entity_class = EntityType::MODELENTITY;
    in_range = true;
    
    // some permissions/flags
    can_collide = true;
    can_rotate = true;
    load_selfs();
}

ModelEntity::ModelEntity(uint32_t p, uint32_t v, uint16_t mid) {
    vid = v;
    pid = p;
    model_id = mid;
    entity_class = EntityType::MODELENTITY;
    in_range = true;
    
    // some permissions/flags
    can_collide = true;
    can_rotate = true;
    
    set_model(mid);
}

// --- ModelEntity ---
void ModelEntity::set_model(uint16_t m) {
    model_id = m;
    model = get_game_model(model_id);
    bounds = model->bounds;
    center_pos = model->center_pos;
}

// --- Entity ---
// Entity* ModelEntity::poke(float x, float y, float z) {}

bool ModelEntity::block_keyboard_callback(Game* game, Action key) {
    return model->model_keyboard_callback(game, this, ivec3(pos.x, pos.y, pos.z), key);
}

bool ModelEntity::block_mouse_callback(Game* game, Action button) {
    return model->model_mouse_callback(game, this, button);
}

void ModelEntity::render(fmat4* transform) {
    if (!in_range)
        return;
    fmat4 view;
    get_mvp(&view);
    fmat4 mvp = *transform * view;
    
    // TODO don't render if out of bounds of screen (see code in chunkrender)
    
    set_transform_matrix(mvp, view);
    
    model->render();
}

void ModelEntity::update_chunks(fvec3* player_pos) {
    // fvec3 rcp = pos + center_pos;
    // TODO implement this
    in_range = true;
}

// void ModelEntity::calculate_moving_bounding_box() {}
// bool ModelEntity::collides_with(Entity* other) {}
// int ModelEntity::get_collision_level() {}
// bool ModelEntity::collides_with(Entity* other, bounding_box* my_bounds, bounding_box* other_bounds, int my_collision_lvl, int other_collision_level);

std::string ModelEntity::get_save_path() {
    return get_path_to_superobj(pid, vid);
}

int ModelEntity::load_self(IODataObject* obj) {
    if (Entity::load_self(obj))
        return 1;
    set_model(obj->read_value<uint16_t>());
    return 0;
}

void ModelEntity::remove_self(IODataObject* obj) {
    Entity::remove_self(obj);
    obj->save_value(model_id);
}

