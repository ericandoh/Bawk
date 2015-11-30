//
//  debug_action.cpp
//  Bawk
//
//  Created by Eric Oh on 11/4/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "debug_action.h"
#include "game.h"
#include "texture_loader.h"

#include "blocktracer.h"
#include "modelentityrender.h"

void debug_action(Game* game) {
    save_tile();
    printf("debuggin\n");
    
    /*ModelEntity* entity = new ModelEntity(game->player->pid, game->player->assignID(), 1);
    fvec4 src;
    get_look_at_vehicle(&src);
    entity->pos.x = src.x;
    entity->pos.y = src.y + 3.0f;
    entity->pos.z = src.z;

    game->world->add_entity(entity);*/
}