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
#include "cursoritem.h"

// TOFU consider making a cursorobject manager, so that we don't keep making new objects (even if we have them loaded in already)
// TODO some of the delete_path in this file are wrong - cursorobjects might refer to other internal objects that are saved elsewhere,
// so those must be deleted as well...in addition, 
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
    std::vector<cursor_item_identifier> cursor_items;
public:
    PlayerInventory();
    
    void new_inv();
    
    int get_block_count();
    int get_recipe_count();
    int get_custom_count();
    
    CursorItem* get_block_at(int index);
    CursorItem* get_recipe_at(int index);
    CursorItem* get_custom_at(int index);
    CursorItem* get_item_from(cursor_item_identifier distinguish);
    CursorItem* get_cursoritem_at(int index);
    
    bool has_custom(uint32_t pid, uint32_t vid);
    bool has_cursor_item(uint32_t pid, uint32_t vid);
    
    void add_blocks(uint16_t type, int count);
    void add_recipe_at(uint16_t type, int count);
    void add_custom_at(CursorItem* to);
    
    void set_custom_at(CursorItem* to, int index);
    void set_cursoritem_at(CursorItem* to, int index);
    
    // can't unlearn blocks/recipes, but can have 0 count
    void del_custom_at(CursorItem* item);
    
    int load_self(IODataObject* obj);
    void save_self(IODataObject* obj);
};

#endif /* defined(__Bawk__inventory__) */
