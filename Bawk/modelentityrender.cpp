//
//  modelentityrender.cpp
//  Bawk
//
//  Created by Eric Oh on 11/29/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "modelentityrender.h"
#include "game_info_loader.h"
#include "worldrender.h"

#include "player.h"

ModelEntity::ModelEntity(uint16_t mid) {
    entity_class = EntityType::MODELENTITY;
    in_range = true;
    
    multiplexer = 0;
    
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
    
    multiplexer = 0;
    
    // some permissions/flags
    can_collide = true;
    can_rotate = true;
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
    if (model->multiplexer) {
        multiplexer = model->multiplexer->copy();
    }
    else {
        multiplexer = 0;
    }
    bounds = model->bounds;
    center_pos = model->center_pos;
}

// --- Entity ---
// Entity* ModelEntity::poke(float x, float y, float z) {}

void ModelEntity::drop_loot(Player* slayer) {
    slayer->inventory->add_models(model_id, 1);
}

bool ModelEntity::block_keyboard_callback(Player* player, Action key, Entity* ent, int ms) {
    if (multiplexer)
        return multiplexer->model_callback(player, ent, this, key, ms);
    return false;
}

bool ModelEntity::block_mouse_callback(Player* player, Action button, Entity* ent) {
    if (multiplexer) {
        if (button == Action::CLICK_MAIN) {
            return multiplexer->model_callback_clicked_main(player, ent, this, 0);
        }
        else if (button == Action::CLICK_SECONDARY) {
            return multiplexer->model_callback_clicked_secondary(player, ent, this, 0);
        }
    }
    return false;
}

void ModelEntity::step(int ms) {
    if (multiplexer)
        multiplexer->model_callback_step(0, this, this, ms);
}

void ModelEntity::render(fmat4* transform) {
    if (!in_range)
        return;
    fmat4 view;
    get_mvp(&view);
    view = *transform * view;
    // TODO don't render if out of bounds of screen (see code in chunkrender)
    OGLAttr::current_shader->set_transform_matrix(&view);
    
    model->render();
}

void ModelEntity::render_lights(fmat4* transform, fvec3 player_pos) {
    if (!in_range)
        return;
    if (model->light.should_render()) {
        fmat4 view;
        get_mvp(&view);
        view = *transform * view;
        // TODO don't render if out of bounds of screen (see code in chunkrender)
        model->light.render_light(&view, center_pos, player_pos);
    }
}

void ModelEntity::update_render(fvec3* player_pos) {
    // fvec3 rcp = pos + center_pos;
    // TODO implement this
    in_range = true;
}

// void ModelEntity::calculate_moving_bounding_box() {}
// bool ModelEntity::collides_with(Entity* other) {}
// int ModelEntity::get_collision_level() {}
// bool ModelEntity::collides_with(Entity* other, bounding_box* my_bounds, bounding_box* other_bounds, int my_collision_lvl, int other_collision_level);

bool ModelEntity::after_collision() {
    if (multiplexer)
        return multiplexer->model_callback_collision(0, this, this, 0);
    return false;
}

std::string ModelEntity::get_save_path() {
    return get_path_to_superobj(pid, vid);
}

int ModelEntity::load_self(IODataObject* obj) {
    if (Entity::load_self(obj))
        return 1;
    set_model(obj->read_value<uint16_t>());
    return 0;
}

void ModelEntity::save_self(IODataObject* obj) {
    Entity::save_self(obj);
    obj->save_value(model_id);
}

