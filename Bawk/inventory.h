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
#include <map>
#include "basic_types.h"
#include "block_loader.h"

class CursorItem;

class PlayerInventory {
    // some basics
    int energy_production;
    int money;
    // not so basics
    std::map<uint16_t, int> blocks;
    std::vector<uint16_t> found_blocks;
    // list of discovered, non-playermade templates, numbered by ID
    // when needed, the selection UI will query these IDs, then read in from
    // memory the necessary cursorsuperobjects. Then those superobjects will be
    // retained until the UI is closed and they're not bound on the item bar
    std::map<uint16_t, int> recipes;
    std::vector<uint16_t> found_recipes;
    // list of player-made templates, numbered by player who made them and the id under said player
    std::vector<player_and_id> customs;
    std::vector<cursor_item_distinguisher> cursor_items;
public:
    PlayerInventory();
    
    void new_inv();
    
    int get_block_count();
    int get_recipe_count();
    int get_custom_count();
    
    CursorItem* get_block_at(int index);
    CursorItem* get_recipe_at(int index);
    CursorItem* get_custom_at(int index);
    CursorItem* get_cursoritem_at(int index);
    
    void add_blocks(uint16_t type, int count);
    void add_recipe_at(uint16_t type, int count);
    void add_custom_at(CursorItem* to);
    
    void set_custom_at(CursorItem* to, int index);
    void set_cursoritem_at(CursorItem* to, int index);
    
    // can't unlearn blocks/recipes, but can have 0 count
    void del_custom_at(int index);
    
    int load_self(IODataObject* obj);
    void remove_self(IODataObject* obj);
};

#endif /* defined(__Bawk__inventory__) */
