//
//  modelactionmount.cpp
//  Bawk
//
//  Created by Eric Oh on 12/9/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "modelactionmount.h"
#include "game.h"
#include "entity.h"

class ModelMountActionMultiplexer: public ModelActionMultiplexer {
public:
    bool model_callback_clicked_secondary(Game *game, Entity *owner, Entity *piece) override;
    bool model_callback_mount(Game *game, Entity *owner, Entity *piece) override;
};

bool ModelMountActionMultiplexer::model_callback_clicked_secondary(Game* game, Entity* owner, Entity* piece) {
    if (owner && owner->entity_class == EntityType::SUPEROBJECT) {
        fvec3 piece_pos = piece->pos + piece->center_pos + fvec3(0.0f, 0.75f, 0.0f);
        if (piece->parent)
            piece->parent->transform_into_world_coordinates(&piece_pos, piece_pos.x, piece_pos.y, piece_pos.z);
        piece_pos = piece_pos - game->player->center_pos;
        game->player->set_mount((SuperObject*)owner, piece_pos);
        return true;
    }
    return false;
}

bool ModelMountActionMultiplexer::model_callback_mount(Game* game, Entity* owner, Entity* piece) {
    game->player->unmount(game->world);
    return true;
}

ModelActionMultiplexer* get_mount_model_multiplexer() {
    return new ModelMountActionMultiplexer();
}