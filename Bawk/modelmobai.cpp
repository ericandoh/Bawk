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
#include "common_accessor.h"

class ModelMobAiSimpleActionMultiplexer: public ModelActionMultiplexer {
    bool targetting;
    Entity* target;
    
    Entity* get_target(SuperObject* world, Entity* owner);
public:
    float radius;
    
    ModelMobAiSimpleActionMultiplexer();
    bool model_callback_step(MODEL_FUNCTION_ARGS) override;
    ModelActionMultiplexer* copy() override;
};

ModelMobAiSimpleActionMultiplexer::ModelMobAiSimpleActionMultiplexer() {
    targetting = false;
    
    radius = 80.0f;
}

Entity* ModelMobAiSimpleActionMultiplexer::get_target(SuperObject* world, Entity* owner) {
    std::vector<Entity*> candidates;
    bounding_box range(owner->get_world_pos(), radius);
    world->get_entities_in_range(candidates, range);
    for (Entity* candidate: candidates) {
        
        if (candidate->entity_class == EntityType::PLAYER || candidate->entity_class == EntityType::SUPEROBJECT) {
            if (candidate != owner) {
                return candidate;
            }
        }
        
        /*
        if (candidate->pid && candidate->pid != owner->pid) {
            // this is an enemy
            return candidate;
        }*/
    }
    return 0;
}

bool ModelMobAiSimpleActionMultiplexer::model_callback_step(MODEL_FUNCTION_ARGS) {
    owner = piece->find_top_level_parent();
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
        target = get_target(get_world()->base_world, owner);
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
        bool at_appropriate_height = false;
        
        // first, check if we're on the right height. our target height is 10 above our opponent
        fvec3 target_pos = target->get_world_pos();
        fvec3 my_pos = owner->get_world_pos();
        float height_difference = target_pos.y - my_pos.y;
        if (fabsf(height_difference) > 4.0f) {
            if (height_difference < 0) {
                // my target's under me, descend
                owner->block_keyboard_callback(player, Action::MOVE_DOWN, owner, ms);
            }
            else {
                // else, move up
                owner->block_keyboard_callback(player, Action::MOVE_UP, owner, ms);
            }
        }
        else {
            at_appropriate_height = true;
        }
        
        fvec3 wanted_direction = target_pos - my_pos;
        wanted_direction.y = 0;
        wanted_direction = glm::normalize(wanted_direction);
        fvec3 current_direction = owner->get_world_rotation().get_forward();
        
        // find angle between us and the target
        float wanted_angle = glm::atan(wanted_direction.z, wanted_direction.x);
        float current_angle = glm::atan(current_direction.z, current_direction.x);
        float desired_angle = wanted_angle - current_angle;
        desired_angle = get_positive_fmodf(desired_angle, 2 * M_PI);
        //desired_angle = 2 * M_PI - desired_angle;
        if (desired_angle > M_PI) {
            desired_angle -= 2 * M_PI;
        }
        //desired_angle -= M_PI;
        float dst = get_fvec3_distance(target_pos - my_pos);
        if (dst * fabsf(desired_angle) < 2.5f) {
            if (at_appropriate_height) {
                // we're in an optimal position. fire away!
                owner->block_keyboard_callback(player, Action::CLICK_MAIN, owner, ms);
            }
        }
        else if (desired_angle > 0) {
            // turn right
            owner->block_keyboard_callback(player, Action::MOVE_RIGHT, owner, ms);
        }
        else {
            // turn left
            owner->block_keyboard_callback(player, Action::MOVE_LEFT, owner, ms);
        }
        if (dst > 20.0f) {
            if (fabsf(desired_angle) < M_PI / 8.0f) {
                owner->block_keyboard_callback(player, Action::MOVE_FORWARD, owner, ms);
            }
            else if (fabsf(desired_angle) > M_PI * 7.0f / 8.0f) {
                owner->block_keyboard_callback(player, Action::MOVE_BACKWARD, owner, ms);
            }
        }
    }
    return true;
}

ModelActionMultiplexer* ModelMobAiSimpleActionMultiplexer::copy() {
    ModelMobAiSimpleActionMultiplexer* copy = new ModelMobAiSimpleActionMultiplexer();
    *copy = *this;
    return copy;
}

ModelActionMultiplexer* get_mob_ai_model_multiplexer(Json::Value node) {
    
    ModelMobAiSimpleActionMultiplexer* mult = new ModelMobAiSimpleActionMultiplexer();
    //json_read_float_safe(&mult->radius, node, "radius");
    //json_read_float_safe(&mult->damage, node, "damage");
    return mult;
    
}