//
//  modelactionshoot.cpp
//  Bawk
//
//  Created by Eric Oh on 12/9/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "modelactionshoot.h"
#include "game.h"
#include "entity.h"
#include "modelentityrender.h"
#include "json_reader_helper.h"
#include "json/json.h"
#include "game_info_loader.h"

class ModelShootActionMultiplexer: public ModelActionMultiplexer {
    int time_elapsed;           // elapsed time since the last bullet was shot, in MS
public:
    uint16_t projectile_id;     // model ID of the bullet we shoot out
    float firing_speed;         // speed this bullet is ejected out at
    int firerate;                    // rate of fire (how frequently to shoot), in MS
    ModelShootActionMultiplexer();
    bool model_callback_click_main(MODEL_FUNCTION_ARGS) override;
};

ModelShootActionMultiplexer::ModelShootActionMultiplexer() {
    // write constructor here
    projectile_id = 0;
    firing_speed = 5.0f;
    firerate = 1000;
    
    time_elapsed = 0;
}

bool ModelShootActionMultiplexer::model_callback_click_main(MODEL_FUNCTION_ARGS) {
    time_elapsed += ms;
    if (time_elapsed < firerate) {
        return true;
    }
    time_elapsed -= firerate;
    
    printf("pew pew! from %f %f %f\n", piece->pos.x, piece->pos.y, piece->pos.z);
    // TODO this obviously needs to be changed
    ModelEntity* bullet = new ModelEntity(game->player->pid, game->player->assignID(), projectile_id);
    bullet->velocity_decay = 0.98f;
    if (piece->parent) {
        fvec3 shoot_pos = piece->angle.dir * 2.0f + piece->pos + piece->center_pos;
        piece->parent->transform_into_world_coordinates_smooth(&bullet->pos, shoot_pos.x, shoot_pos.y, shoot_pos.z);
        bullet->pos = bullet->pos - bullet->center_pos;
        piece->parent->angle.transform_into_world_rotation(&(bullet->angle), piece->angle);
        bullet->recalculate_transform();
        bullet->move_forward(firing_speed);
    }
    game->world->add_entity(bullet);
    return true;
}

ModelActionMultiplexer* get_shoot_model_multiplexer(Json::Value node) {
    ModelShootActionMultiplexer* mult = new ModelShootActionMultiplexer();
    if (node.isMember("projectile")) {
        mult->projectile_id = get_model_id_from_name(node["projectile"]);
    }
    json_read_float_safe(&mult->firing_speed, node, "pspeed");
    json_read_int_safe(&mult->firerate, node, "firerate");
    return mult;
}