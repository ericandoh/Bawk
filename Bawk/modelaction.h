//
//  modelaction.h
//  Bawk
//
//  Created by Eric Oh on 12/14/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__modelaction__
#define __Bawk__modelaction__

#include <stdio.h>
#include "game_actions.h"
#include "basic_types.h"

#define MODEL_FUNCTION_ARGS Game* game, Entity* owner, Entity* piece, int ms

class Game;
class Entity;

class ModelActionMultiplexer {
public:
    // copyme
    virtual bool model_callback_default(MODEL_FUNCTION_ARGS) EMPTY_BOOL_FUNCTION;
    
    // --- INPUT FROM USER (needs multiplexing) ---
    // movement related
    virtual bool model_callback_move_forward(MODEL_FUNCTION_ARGS) EMPTY_BOOL_FUNCTION;
    virtual bool model_callback_move_backward(MODEL_FUNCTION_ARGS) EMPTY_BOOL_FUNCTION;
    virtual bool model_callback_move_up(MODEL_FUNCTION_ARGS) EMPTY_BOOL_FUNCTION;
    virtual bool model_callback_move_down(MODEL_FUNCTION_ARGS) EMPTY_BOOL_FUNCTION;
    virtual bool model_callback_yaw_left(MODEL_FUNCTION_ARGS) EMPTY_BOOL_FUNCTION;
    virtual bool model_callback_yaw_right(MODEL_FUNCTION_ARGS) EMPTY_BOOL_FUNCTION;
    virtual bool model_callback_roll_left(MODEL_FUNCTION_ARGS) EMPTY_BOOL_FUNCTION;
    virtual bool model_callback_roll_right(MODEL_FUNCTION_ARGS) EMPTY_BOOL_FUNCTION;
    virtual bool model_callback_pitch_up(MODEL_FUNCTION_ARGS) EMPTY_BOOL_FUNCTION;
    virtual bool model_callback_pitch_down(MODEL_FUNCTION_ARGS) EMPTY_BOOL_FUNCTION;
    
    // click related
    virtual bool model_callback_click_main(MODEL_FUNCTION_ARGS) EMPTY_BOOL_FUNCTION;
    virtual bool model_callback_click_secondary(MODEL_FUNCTION_ARGS) EMPTY_BOOL_FUNCTION;
    
    // other
    virtual bool model_callback_mount(MODEL_FUNCTION_ARGS) EMPTY_BOOL_FUNCTION;
    
    // --- INPUT FROM GAME (no multiplexing needed) ---
    // when clicked on
    virtual bool model_callback_clicked_main(MODEL_FUNCTION_ARGS) EMPTY_BOOL_FUNCTION;
    virtual bool model_callback_clicked_secondary(MODEL_FUNCTION_ARGS) EMPTY_BOOL_FUNCTION;
    
    // collision related
    virtual bool model_callback_collision(MODEL_FUNCTION_ARGS) EMPTY_BOOL_FUNCTION;
    
    // step
    virtual bool model_callback_step(MODEL_FUNCTION_ARGS) EMPTY_BOOL_FUNCTION;
    
    // --- ModelActionMultiplexer ---
    bool model_callback(Game* game, Entity* owner, Entity* piece, Action key, int ms);
    virtual ModelActionMultiplexer* copy();
};

typedef bool (ModelActionMultiplexer::*model_callback_func)(MODEL_FUNCTION_ARGS);
model_callback_func get_model_action_from(Action key);

namespace Json {
    class Value;
}

ModelActionMultiplexer* get_model_action_multiplexer_from(Json::Value node);

#endif /* defined(__Bawk__modelaction__) */
