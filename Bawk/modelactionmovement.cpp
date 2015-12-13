//
//  modelactionmovement.cpp
//  Bawk
//
//  Created by Eric Oh on 12/9/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "modelactionmovement.h"
#include "game.h"
#include "entity.h"
#include "spritegetter.h"

bool model_forward_callback(Game* game, Entity* owner, Entity* piece) {
    // piece->get_force() or some shizzle...and repeat for all below
    owner->move_forward(20.0f);
    
    fvec3 rand_offset = fvec3(rand() % 100 - 50.0f,rand() % 100 - 50.0f,rand() % 100 - 50.0f) / 400.0f;
    fvec3 pos = piece->pos + piece->center_pos + rand_offset;
    if (piece->parent) {
        piece->parent->transform_into_world_coordinates_smooth(&pos, pos.x, pos.y, pos.z);
    }
    SpriteRender* sprite = get_sprite_instance(2, game->player);
    sprite->set_pos(pos);
    game->world->base_world->add_entity(sprite);
    return true;
}

bool model_backward_callback(Game* game, Entity* owner, Entity* piece) {
    owner->move_backward(5.0f);
    return true;
}

bool model_turn_left_callback(Game* game, Entity* owner, Entity* piece) {
    owner->yaw_left(4.0f);
    return true;
}

bool model_turn_right_callback(Game* game, Entity* owner, Entity* piece) {
    owner->yaw_right(4.0f);
    return true;
}

bool model_up_callback(Game* game, Entity* owner, Entity* piece) {
    owner->move_up(5.0f);
    return true;
}

bool model_down_callback(Game* game, Entity* owner, Entity* piece) {
    owner->move_down(5.0f);
    return true;
}

bool model_pitch_up_callback(Game* game, Entity* owner, Entity* piece) {
    owner->pitch_up(3.0f);
    return true;
}

bool model_pitch_down_callback(Game* game, Entity* owner, Entity* piece) {
    owner->pitch_down(3.0f);
    return true;
}

bool model_roll_left_callback(Game* game, Entity* owner, Entity* piece) {
    owner->roll_left(3.0f);
    return true;
}

bool model_roll_right_callback(Game* game, Entity* owner, Entity* piece) {
    owner->roll_right(3.0f);
    return true;
}