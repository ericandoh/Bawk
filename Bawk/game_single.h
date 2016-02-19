//
//  game_single.h
//  Bawk
//
//  Created by Eric Oh on 2/16/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__game_single__
#define __Bawk__game_single__

#include <stdio.h>
#include "client.h"
#include "engine.h"

// a single player game, shared between client and system
// TOFU

class GameSP: public Client, public Engine {
    
    
public:
    
    
    // --- Client ---
    Player* register_player(uint32_t pid) override;
    void deregister_player(uint32_t pid) override;
    void update_render(fvec3 player_pos) override;
    World* get_world() override;
    void toggle_action(Action do_this) override;
    void toggle_player_movement(double x, double y) override;
};

#endif /* defined(__Bawk__game_single__) */
