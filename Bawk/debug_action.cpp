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

#include "efros_leung.h"

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
    
    SuperObject* src = game->world->base_world;
    SuperObject* dst = game->world->base_world;
    
    // train on (0,3,0)to(4,6,4) area of world
    // output to (-32,3,-32)to(-24,6,24)
    
    for (int x = -32; x < -16; x++) {
        for (int z = -32; z < -16; z++) {
            for (int y = 4; y < 20; y++) {
                game->world->base_world->set_block_integral(x, y, z, block_type());
            }
        }
    }
    //train_and_apply_efros_leung(src, dst, 0, 3, 0, 4, 6, 4, -32, 3, -32, -24, 6, -24);
    
    train_and_apply_efros_leung(src, dst, 0, 4, 0, 32, 10, 32, -32, 4, -32, -16, 20, -16);
}