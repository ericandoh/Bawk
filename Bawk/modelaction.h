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

class Game;
class Entity;

class ModelActionMultiplexer {
public:
    // copyme
    virtual bool model_callback_default(Game* game, Entity* owner, Entity* piece) EMPTY_BOOL_FUNCTION;
    
    // --- INPUT FROM USER (needs multiplexing) ---
    // movement related
    virtual bool model_callback_move_forward(Game* game, Entity* owner, Entity* piece) EMPTY_BOOL_FUNCTION;
    virtual bool model_callback_move_backward(Game* game, Entity* owner, Entity* piece) EMPTY_BOOL_FUNCTION;
    virtual bool model_callback_move_up(Game* game, Entity* owner, Entity* piece) EMPTY_BOOL_FUNCTION;
    virtual bool model_callback_move_down(Game* game, Entity* owner, Entity* piece) EMPTY_BOOL_FUNCTION;
    virtual bool model_callback_yaw_left(Game* game, Entity* owner, Entity* piece) EMPTY_BOOL_FUNCTION;
    virtual bool model_callback_yaw_right(Game* game, Entity* owner, Entity* piece) EMPTY_BOOL_FUNCTION;
    virtual bool model_callback_roll_left(Game* game, Entity* owner, Entity* piece) EMPTY_BOOL_FUNCTION;
    virtual bool model_callback_roll_right(Game* game, Entity* owner, Entity* piece) EMPTY_BOOL_FUNCTION;
    virtual bool model_callback_pitch_up(Game* game, Entity* owner, Entity* piece) EMPTY_BOOL_FUNCTION;
    virtual bool model_callback_pitch_down(Game* game, Entity* owner, Entity* piece) EMPTY_BOOL_FUNCTION;
    
    // click related
    virtual bool model_callback_click_main(Game* game, Entity* owner, Entity* piece) EMPTY_BOOL_FUNCTION;
    virtual bool model_callback_click_secondary(Game* game, Entity* owner, Entity* piece) EMPTY_BOOL_FUNCTION;
    
    // other
    virtual bool model_callback_mount(Game* game, Entity* owner, Entity* piece) EMPTY_BOOL_FUNCTION;
    
    // --- INPUT FROM GAME (no multiplexing needed) ---
    // when clicked on
    virtual bool model_callback_clicked_main(Game* game, Entity* owner, Entity* piece) EMPTY_BOOL_FUNCTION;
    virtual bool model_callback_clicked_secondary(Game* game, Entity* owner, Entity* piece) EMPTY_BOOL_FUNCTION;
    
    // collision related
    virtual bool model_callback_collision(Game* game, Entity* owner, Entity* piece) EMPTY_BOOL_FUNCTION;
    
    // --- ModelActionMultiplexer ---
    bool model_callback(Game* game, Entity* owner, Entity* piece, Action key);
    virtual ModelActionMultiplexer* copy();
};

typedef bool (ModelActionMultiplexer::*model_callback_func)(Game* game, Entity* owner, Entity* piece);
model_callback_func get_model_action_from(Action key);

namespace Json {
    class Value;
}

ModelActionMultiplexer* get_model_action_multiplexer_from(Json::Value node);

#endif /* defined(__Bawk__modelaction__) */
