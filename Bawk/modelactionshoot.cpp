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

bool model_shoot_callback(Game* game, Entity* owner, Entity* piece) {
    printf("pew pew! from %f %f %f\n", piece->pos.x, piece->pos.y, piece->pos.z);
    // TODO this obviously needs to be changed
    ModelEntity* bullet = new ModelEntity(game->player->pid, game->player->assignID(), 4);
    bullet->velocity_decay = 0.98f;
    if (piece->parent) {
        fvec3 shoot_pos = piece->angle.dir * 2.0f + piece->pos + piece->center_pos;
        piece->parent->transform_into_world_coordinates_smooth(&bullet->pos, shoot_pos.x, shoot_pos.y, shoot_pos.z);
        bullet->pos = bullet->pos - bullet->center_pos;
        piece->parent->angle.transform_into_world_rotation(&(bullet->angle), piece->angle);
        bullet->recalculate_transform();
        bullet->move_forward(5.0f);
    }
    game->world->add_entity(bullet);
    return true;
}