//
//  modelaction.cpp
//  Bawk
//
//  Created by Eric Oh on 12/14/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "modelaction.h"
#include <vector>
#include "json/json.h"
#include "json_reader_helper.h"

// read existing models
#include "modelactionmount.h"
#include "modelactionexplode.h"
#include "modelactionmovement.h"
#include "modelactionshoot.h"
#include "modelmobai.h"

#include "entity.h"

namespace action_multiplexing {
    std::vector<model_callback_func> action_map;
    bool map_set = false;
}

using namespace action_multiplexing;

bool ModelActionMultiplexer::model_callback(Player* player, Entity* owner, Entity* piece, Action key, int ms) {
    model_callback_func func = get_model_action_from(key);
    if (func) {
        return (this->*func)(player, owner, piece, ms);
    }
    return false;
}

ModelActionMultiplexer* ModelActionMultiplexer::copy() {
    // default behaviour is to NOT copy, since we have no state
    // override this for stateful action multiplexers
    // (for example, guns that fire a big one every 5 rounds needs to keep separate state of a counter)
    return this;
}

void init_model_action_multiplexer() {
    action_map.clear();
    action_map.resize(Action::TOTAL_ACTIONS);
    // zero out
    for (int i = 0; i < Action::TOTAL_ACTIONS; i++) {
        action_map[i] = 0;
    }
    
    action_map[Action::DEFAULT] = &ModelActionMultiplexer::model_callback_default;
    
    action_map[Action::MOVE_FORWARD] = &ModelActionMultiplexer::model_callback_move_forward;
    action_map[Action::MOVE_BACKWARD] = &ModelActionMultiplexer::model_callback_move_backward;
    action_map[Action::MOVE_UP] = &ModelActionMultiplexer::model_callback_move_up;
    action_map[Action::MOVE_DOWN] = &ModelActionMultiplexer::model_callback_move_down;
    action_map[Action::MOVE_LEFT] = &ModelActionMultiplexer::model_callback_yaw_left;
    action_map[Action::MOVE_RIGHT] = &ModelActionMultiplexer::model_callback_yaw_right;
    action_map[Action::ROLL_LEFT] = &ModelActionMultiplexer::model_callback_roll_left;
    action_map[Action::ROLL_RIGHT] = &ModelActionMultiplexer::model_callback_roll_right;
    action_map[Action::PITCH_UP] = &ModelActionMultiplexer::model_callback_pitch_up;
    action_map[Action::PITCH_DOWN] = &ModelActionMultiplexer::model_callback_pitch_down;
    
    action_map[Action::CLICK_MAIN] = &ModelActionMultiplexer::model_callback_click_main;
    action_map[Action::CLICK_SECONDARY] = &ModelActionMultiplexer::model_callback_click_secondary;
    
    action_map[Action::MOUNTING] = &ModelActionMultiplexer::model_callback_mount;
    
    action_map[Action::COLLIDE] = &ModelActionMultiplexer::model_callback_collision;
    map_set = true;
}

model_callback_func get_model_action_from(Action key) {
    if (!map_set) {
        init_model_action_multiplexer();
    }
    return action_map[key];
}

ModelActionMultiplexer* get_model_action_multiplexer_from(Json::Value node) {
    if (!node.isObject())
        return 0;
    // read the name
    std::string name = json_read_string_safe(node, "name");
    // now get the appropriate modelactionmultiplexer depending on the node
    if (name.compare("mount") == 0) {
        return get_mount_model_multiplexer();
    }
    else if (name.compare("shoot") == 0) {
        return get_shoot_model_multiplexer(node);
    }
    else if (name.compare("engine") == 0) {
        return get_engine_model_multiplexer(node);
    }
    else if (name.compare("explode") == 0) {
        return get_explode_model_multiplexer(node);
    }
    else if (name.compare("mob1") == 0) {
        return get_mob_ai_model_multiplexer(node);
    }
    return 0;
}