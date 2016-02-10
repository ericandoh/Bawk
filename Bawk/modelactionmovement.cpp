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
#include "json/json.h"
#include "json_reader_helper.h"

class ModelEngineActionMultiplexer: public ModelActionMultiplexer {
public:
    float forward_speed, backward_speed;
    float vertical_speed;
    float yaw_speed, roll_speed, pitch_speed;
    ModelEngineActionMultiplexer();
    bool model_callback_move_forward(MODEL_FUNCTION_ARGS) override;
    bool model_callback_move_backward(MODEL_FUNCTION_ARGS) override;
    bool model_callback_move_up(MODEL_FUNCTION_ARGS) override;
    bool model_callback_move_down(MODEL_FUNCTION_ARGS) override;
    bool model_callback_yaw_left(MODEL_FUNCTION_ARGS) override;
    bool model_callback_yaw_right(MODEL_FUNCTION_ARGS) override;
    bool model_callback_roll_left(MODEL_FUNCTION_ARGS) override;
    bool model_callback_roll_right(MODEL_FUNCTION_ARGS) override;
    bool model_callback_pitch_up(MODEL_FUNCTION_ARGS) override;
    bool model_callback_pitch_down(MODEL_FUNCTION_ARGS) override;
};

ModelEngineActionMultiplexer::ModelEngineActionMultiplexer() {
    // apply 500 force during 1 second
    forward_speed = 500.0f / convert_sec_to_milli(1);
    backward_speed = 250.0f / convert_sec_to_milli(1);
    vertical_speed = 250.0f / convert_sec_to_milli(1);
    yaw_speed = 250.0f / convert_sec_to_milli(1);
    roll_speed = 250.0f / convert_sec_to_milli(1);
    pitch_speed = 250.0f / convert_sec_to_milli(1);
}

bool ModelEngineActionMultiplexer::model_callback_move_forward(MODEL_FUNCTION_ARGS) {
    // piece->get_force() or some shizzle...and repeat for all below
    owner->move_forward(forward_speed * ms);
    
    fvec3 rand_offset = fvec3(get_positive_mod(rand(), 100) - 50.0f,
                              get_positive_mod(rand(), 100) - 50.0f,
                              get_positive_mod(rand(), 100) - 50.0f) / 400.0f;
    fvec3 pos = piece->get_world_pos() + rand_offset;
    SpriteRender* sprite = get_sprite_instance(2, game->player);
    sprite->set_pos(pos);
    game->world->base_world->add_entity(sprite);
    return true;
}

bool ModelEngineActionMultiplexer::model_callback_move_backward(MODEL_FUNCTION_ARGS) {
    owner->move_backward(backward_speed * ms);
    return true;
}

bool ModelEngineActionMultiplexer::model_callback_move_up(MODEL_FUNCTION_ARGS) {
    owner->move_up(vertical_speed * ms);
    return true;
}

bool ModelEngineActionMultiplexer::model_callback_move_down(MODEL_FUNCTION_ARGS) {
    owner->move_down(vertical_speed * ms);
    return true;
}

bool ModelEngineActionMultiplexer::model_callback_yaw_left(MODEL_FUNCTION_ARGS) {
    owner->yaw_left(yaw_speed * ms);
    return true;
}

bool ModelEngineActionMultiplexer::model_callback_yaw_right(MODEL_FUNCTION_ARGS) {
    owner->yaw_right(yaw_speed * ms);
    return true;
}

bool ModelEngineActionMultiplexer::model_callback_roll_left(MODEL_FUNCTION_ARGS) {
    owner->roll_left(roll_speed * ms);
    return true;
}

bool ModelEngineActionMultiplexer::model_callback_roll_right(MODEL_FUNCTION_ARGS) {
    owner->roll_right(roll_speed * ms);
    return true;
}

bool ModelEngineActionMultiplexer::model_callback_pitch_up(MODEL_FUNCTION_ARGS) {
    owner->pitch_up(pitch_speed * ms);
    return true;
}

bool ModelEngineActionMultiplexer::model_callback_pitch_down(MODEL_FUNCTION_ARGS) {
    owner->pitch_down(pitch_speed * ms);
    return true;
}

ModelActionMultiplexer* get_engine_model_multiplexer(Json::Value node) {
    ModelEngineActionMultiplexer* mult = new ModelEngineActionMultiplexer();
    
    json_read_float_safe(&mult->forward_speed, node, "forward");
    json_read_float_safe(&mult->backward_speed, node, "backward");
    json_read_float_safe(&mult->vertical_speed, node, "vertical");
    json_read_float_safe(&mult->yaw_speed, node, "yaw");
    json_read_float_safe(&mult->roll_speed, node, "roll");
    json_read_float_safe(&mult->pitch_speed, node, "pitch");
    
    return mult;
}

