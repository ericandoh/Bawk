//
//  inventory.h
//  Bawk
//
//  Created by Eric Oh on 10/16/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__inventory__
#define __Bawk__inventory__

#include <stdio.h>
#include <vector>
#include "cursoritem.h"
#include "basic_types.h"

struct block_and_count {
    uint16_t type;
    int count;
    block_and_count(uint16_t t, int c) {
        type = t;
        count = c;
    }
};

class PlayerInventory {
    // some basics
    int energy_production;
    int money;
    // not so basics
    std::vector<block_and_count> blocks;
    // list of discovered, non-playermade templates, numbered by ID
    // when needed, the selection UI will query these IDs, then read in from
    // memory the necessary cursorsuperobjects. Then those superobjects will be
    // retained until the UI is closed and they're not bound on the item bar
    std::vector<uint16_t> recipes;
    // list of player-made templates, numbered by player who made them and the id under said player
    std::vector<player_and_id> customs;
public:
    PlayerInventory();
    
    int get_block_count();
    int get_recipe_count();
    int get_custom_count();
    
    CursorItem* get_block_at(int index);
    CursorItem* get_recipe_at(int index);
    CursorItem* get_custom_at(int index);
    
};

#endif /* defined(__Bawk__inventory__) */
