//
//  modelactionturret.cpp
//  Bawk
//
//  Created by Eric Oh on 1/8/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "modelactionturret.h"
#include "json/json.h"
#include "json_reader_helper.h"

class ModelTurretActionMultiplexer: public ModelActionMultiplexer {
    bool setup;
    Entity* weapon;
public:
    float radius;
    float damage;
    ModelTurretActionMultiplexer();
    bool model_callback_step(MODEL_FUNCTION_ARGS) override;
};

ModelTurretActionMultiplexer::ModelTurretActionMultiplexer() {
    setup = false;
}

bool ModelTurretActionMultiplexer::model_callback_step(MODEL_FUNCTION_ARGS) {
    // if not initialized see if owner has any guns attached right above it
    if (!setup) {
        // look for a gun right above us in position in owner
        // bind gun to Entity
        setup = true;
    }
    // see if any enemies within a radius around us
    
    // if enemy found, get closest enemy + shoot at it
    
    return true;
}

ModelActionMultiplexer* get_turret_model_multiplexer(Json::Value node) {
    ModelTurretActionMultiplexer* mult = new ModelTurretActionMultiplexer();
    json_read_float_safe(&mult->radius, node, "radius");
    json_read_float_safe(&mult->damage, node, "damage");
    return mult;
}