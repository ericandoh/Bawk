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
    
    // statistic counters
    int energy_production;
    int money;
    
    // inventory related
    std::map<uint16_t, CursorItemInfo*> blocks;  // the actual block info
    std::vector<uint16_t> found_blocks;         // used for ordering
    
    std::map<uint16_t, CursorItemInfo*> models;  // the actual model info
    std::vector<uint16_t> found_models;         // used for ordering
    
    std::vector<CursorItemInfo*> customs;        // the actual custom model info (which is a reference to disk)
    std::vector<CursorItemInfo*> bar_items;  // pointers to any of the above, to be put on the itembar
public:
    PlayerInventory();
    
    void new_inv();
    
    int get_block_count();
    int get_model_count();
    int get_custom_count();
    
    CursorItem* get_block(int index);
    CursorItem* get_model(int index);
    CursorItem* get_custom(int index);
    CursorItem* get_bar_item(int index);
    CursorItem* get_item(CursorItemInfo* info);
    
    // used to delete cursoritems from memory if the last copy of them was deleted
    bool has_custom(CursorItemInfo* info);
    int find_bar_item(CursorItemInfo* info);
    
    void add_blocks(uint16_t type, int count);
    void add_models(uint16_t type, int count);
    void add_custom(CursorItemInfo* item);
    
    void swap_custom_indices(int index0, int index1);
    void set_bar_item(int index, CursorItemInfo* item);
    
    // can't unlearn blocks/recipes, but can have 0 count
    void del_custom(CursorItem* item);
    
    int load_self(IODataObject* obj);
    void save_self(IODataObject* obj);
};

#endif /* defined(__Bawk__inventory__) */
