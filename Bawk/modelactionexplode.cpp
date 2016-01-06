//
//  modelactionexplode.cpp
//  Bawk
//
//  Created by Eric Oh on 12/9/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "modelactionexplode.h"
#include "entity.h"
#include "game.h"
#include "superobject.h"
#include "json/json.h"
#include "json_reader_helper.h"

#include "spritegetter.h"

class ModelExplodeActionMultiplexer: public ModelActionMultiplexer {
public:
    float radius;
    float damage;
    ModelExplodeActionMultiplexer();
    bool model_callback_collision(Game *game, Entity *owner, Entity *piece) override;
};

ModelExplodeActionMultiplexer::ModelExplodeActionMultiplexer() {
    radius = 3.0f;
    damage = 500.0f;
}

bool ModelExplodeActionMultiplexer::model_callback_collision(Game* game, Entity* owner, Entity* piece) {
    fvec3 pos = piece->pos + piece->center_pos;
    if (piece->parent) {
        piece->parent->transform_into_world_coordinates_smooth(&pos, pos.x, pos.y, pos.z);
        // remove myself from the world
        piece->parent->remove_entity(piece);
    }
    // now explode everything in a radius of 4 around our target
    int radius = 4;
    fvec3 lower = pos - (float)radius;
    fvec3 upper = pos + (float)radius;
    fvec3 ilower = get_round_from_fvec3(lower);
    fvec3 iupper = get_round_from_fvec3(upper);
    for (int x = ilower.x; x <= iupper.x; x++) {
        for (int y = ilower.y; y <= iupper.y; y++) {
            for (int z = ilower.z; z <= iupper.z; z++) {
                if (get_fvec3_distance(fvec3(x - pos.x, y - pos.y, z - pos.z)) <= radius) {
                    // damage whatever is at this position
                    game->world->base_world->get_hurt(x, y, z, damage, BreakablePolicy::ACTIONED, owner->pid);
                }
            }
        }
    }
    // TODO add sprites to the world here
    // TODO redo this - this is horrible, horrible code...
    SpriteRender* sprite = get_sprite_instance(1, game->player);
    sprite->set_pos(pos);
    game->world->base_world->add_entity(sprite);
    return true;
}

ModelActionMultiplexer* get_explode_model_multiplexer(Json::Value node) {
    ModelExplodeActionMultiplexer* mult = new ModelExplodeActionMultiplexer();
    json_read_float_safe(&mult->radius, node, "radius");
    json_read_float_safe(&mult->damage, node, "damage");
    return mult;
}