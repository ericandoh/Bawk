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

#include "efros_leung_two.h"

bool efros_started = false;

void debug_action(Game* game) {
    //save_tile();
    //printf("debuggin\n");
    
    /*ModelEntity* entity = new ModelEntity(game->player->pid, game->player->assignID(), 1);
    fvec4 src;
    get_look_at_vehicle(&src);
    entity->pos.x = src.x;
    entity->pos.y = src.y + 3.0f;
    entity->pos.z = src.z;

    game->world->add_entity(entity);*/
    
    
    if (!efros_started) {
        SuperObject* src = game->world->base_world;
        SuperObject* dst = game->world->base_world;
        
        // area we're training on
        ivec3 ltrain(0, 3, 0);
        ivec3 utrain(48, 12, 48);
        // area we output to
        ivec3 lapply(-48,3,-48);
        ivec3 uapply(-16,13,-16);
        
        for (int x = lapply.x; x < uapply.x; x++) {
            for (int z = lapply.z; z < uapply.z; z++) {
                for (int y = lapply.y; y < uapply.y; y++) {
                    game->world->base_world->set_block_integral(x, y, z, block_type());
                }
            }
        }
        setup_efros(src, dst,
                    ltrain.x, ltrain.y, ltrain.z,
                    utrain.x, utrain.y, utrain.z,
                    lapply.x, lapply.y, lapply.z,
                    uapply.x, uapply.y, uapply.z);
        efros_started = true;
    }
    
    if (step_efros()) {
        efros_started = false;
    }
    //train_and_apply_efros_leung(src, dst, 0, 3, 0, 4, 6, 4, -32, 3, -32, -24, 6, -24);
    
    //train_and_apply_efros_leung(src, dst, 0, 4, 0, 32, 10, 32, -32, 4, -32, -16, 20, -16);
}