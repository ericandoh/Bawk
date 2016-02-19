//
//  engine.h
//  Bawk
//
//  Created by Eric Oh on 2/16/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__engine__
#define __Bawk__engine__

#include <stdio.h>
#include <vector>
#include "engine_actions.h"
#include "game_actions.h"
#include "basic_types.h"

class World;
class Player;
class Entity;

class Engine {
protected:
    std::vector<Player*> players;
    World* world;
    
    // --- SYNCH ---
    unsigned int update_state;
    virtual void synchronize() EMPTY_FUNCTION;
    virtual void register_action(Entity* ent, EngineAction action) EMPTY_FUNCTION;
    
    int load_game_data();
    int save_game_data();
public:
    int start(std::string world_name);
    void stop();
    
    void step(int ms);
    void react(Action do_this, Player* player);
    
    World* get_world();
    Player* get_player_from(uint32_t pid);
};

#endif /* defined(__Bawk__engine__) */
