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
    
    game->player->set_pos(game->player->get_world_pos() + fvec3(0, 20, 0));
    
}