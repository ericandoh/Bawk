//
//  modelmobai.cpp
//  Bawk
//
//  Created by Eric Oh on 2/1/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "modelmobai.h"
#include "json/json.h"
#include "json_reader_helper.h"
#include "entity.h"
#include "superobject.h"
#include "game.h"

class ModelMobAiSimpleActionMultiplexer: public ModelActionMultiplexer {
    bool targetting;
    Entity* target;
    
    Entity* get_target(SuperObject* world, Entity* owner);
public:
    float radius;
    
    ModelMobAiSimpleActionMultiplexer();
    bool model_callback_step(MODEL_FUNCTION_ARGS) override;
};

ModelMobAiSimpleActionMultiplexer::ModelMobAiSimpleActionMultiplexer() {
    targetting = false;
}

Entity* ModelMobAiSimpleActionMultiplexer::get_target(SuperObject* world, Entity* owner) {
    std::vector<Entity*> candidates;
    bounding_box range(owner->get_world_pos(), radius);
    world->get_entities_in_range(candidates, range);
    for (Entity* candidate: candidates) {
        if (candidate->pid && candidate->pid != owner->pid) {
            // this is an enemy
            return candidate;
        }
    }
    return 0;
}

bool ModelMobAiSimpleActionMultiplexer::model_callback_step(MODEL_FUNCTION_ARGS) {
    // if not initialized see if owner has any guns attached right above it
    if (targetting) {
        // see if our target is still in range
        fvec3 target_pos = target->get_world_pos();
        bounding_box range(owner->get_world_pos(), radius);
        bool in_range = range.hits(target_pos);  // TODO
        if (!in_range) {
            targetting = false;
            target = 0;
        }
    }
    else {
        // scan for a target
        target = get_target(game->world->base_world, owner);
        if (target) {
            // if target found, set it
            targetting = true;
        }
        else {
            targetting = false;
        }
    }
    
    if (targetting) {
        // do we need to move? if so do so
        
        // else, we're in an optimal position. fire away!
        owner->block_keyboard_callback(game, Action::CLICK_MAIN, owner, ms);
    }
    
    
    return true;
}

ModelActionMultiplexer* get_mob_ai_model_multiplexer(Json::Value node) {
    
    ModelMobAiSimpleActionMultiplexer* mult = new ModelMobAiSimpleActionMultiplexer();
    //json_read_float_safe(&mult->radius, node, "radius");
    //json_read_float_safe(&mult->damage, node, "damage");
    return mult;
    
}