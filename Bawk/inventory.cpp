//
//  inventory.cpp
//  Bawk
//
//  Created by Eric Oh on 10/16/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "inventory.h"
#include "game_info_loader.h"
#include "cursoritem_loader.h"

void insert_ordered(std::vector<uint16_t>* src, uint16_t v) {
    int start = 0;
    int end = (int)src->size();
    int index = -1;
    while (start < end) {
        int mid = (start + end) / 2;
        if (v < src->at(mid)) {
            end = mid;
        }
        else if (v == src->at(mid)) {
            index = mid;
            break;
        }
        else {
            start = mid + 1;
        }
    }
    src->insert(src->begin() + start, v);
}

PlayerInventory::PlayerInventory() {
    // start with no energy production
    energy_production = 0;
    // and no money...hey at least its a step up from being IN DEBT AFTER YOU GRADUATE...fml
    money = 0;
}

void PlayerInventory::new_inv() {
    // TODO load this from the gameinfo.json
    
    // this is what a noob starts out with if he hasn't got jack shit
    // but hey, at least let's get you some basic blocks
    // now you can place air! by breathing it out! ...
    add_blocks(0, 0);
    // let's get you 3 blocks of 1 as well, why not
    add_blocks(1, 3);
    
    // let's get you all the blocks from 2 to 20, but give you only 1 of each
    add_blocks(2, 1);
    add_blocks(3, 1);
    // lucky 7
    add_blocks(7, 3);
    add_blocks(10, 1);
    add_blocks(16, 1);
    add_blocks(20, 1);
    add_blocks(37, 1);
    add_blocks(38, 1);
    add_blocks(94, 1);
    add_blocks(120, 1);
    add_blocks(128, 1);
    
    /*for (int i = 2; i < 20; i++) {
        add_blocks(i, 1);
    }*/
    
    // give you some models
    add_models(2, 3);
    add_models(3, 3);
    add_models(5, 3);
    
    
    // discovered_templates.push(...);
    // you haven't created any either, fuck off
    // created_templates.push(...);
    
    // set block 2 as one item on our cursor, at index 0
    set_bar_item(0, blocks[10]);
}

int PlayerInventory::get_block_count() {
    return (int)found_blocks.size();
}

int PlayerInventory::get_model_count() {
    return (int)found_models.size();
}

int PlayerInventory::get_custom_count() {
    return (int)customs.size();
}

CursorItem* PlayerInventory::get_block(int index) {
    if (index >= found_blocks.size())
        return 0;
    // this is slightly inefficient but whatevers
    if (found_blocks[index] == 0)
        return 0;
    return get_cursor_item_from(blocks[found_blocks[index]]);
}

CursorItem* PlayerInventory::get_model(int index) {
    if (index >= found_models.size())
        return 0;
    if (found_models[index] == 0)
        return 0;
    return get_cursor_item_from(models[found_models[index]]);
    //return get_recipe_cursoritem_from(recipe_id);
}

CursorItem* PlayerInventory::get_custom(int index) {
    if (index >= customs.size())
        return 0;
    return get_cursor_item_from(customs[index]);
}

CursorItem* PlayerInventory::get_bar_item(int index) {
    if (index >= bar_items.size())
        return 0;
    return get_cursor_item_from(bar_items[index]);
}

bool PlayerInventory::has_custom(CursorItemInfo* info) {
    for (auto& i: customs) {
        if (*i == *info) {
            // compare pointers directly, because why not
            return true;
        }
    }
    return false;
}
/*
bool PlayerInventory::has_bar_item(CursorItemInfo* info) {
    for (auto &i: bar_items) {
        if (i->type == info->type && i->pid == info->pid && i->vid == info->vid) {
            // compare pointers directly, because why not
            return true;
        }
    }
    return false;
}*/

void PlayerInventory::add_blocks(uint16_t type, int count) {
    if (blocks.count(type)) {
        blocks[type]->count += count;
        if (blocks[type]->count < 0) {
            blocks[type]->count = 0;
        }
    }
    else {
        blocks[type] = new CursorItemInfo(type, CursorType::CURSOR_BLOCK);
        blocks[type]->count = count;
        insert_ordered(&found_blocks, type);
    }
}

void PlayerInventory::add_models(uint16_t type, int count) {
    if (models.count(type)) {
        models[type]->count += count;
        if (models[type]->count < 0) {
            models[type]->count = 0;
        }
    }
    else {
        models[type] = new CursorItemInfo(type, CursorType::CURSOR_MODEL);
        models[type]->count = count;
        insert_ordered(&found_models, type);
    }
}

// for adding a custom item that previously didn't exist
void PlayerInventory::add_custom(CursorItemInfo* item) {
    if (has_custom(item)) {
        return;
    }
    customs.push_back(item);
}

// assume this is for a reordering
// later modify this to be insert_custom_at and remove the existing from the current index
void PlayerInventory::swap_custom_indices(int index0, int index1) {
    if (index0 >= customs.size() || index1 >= customs.size()) {
        return;
    }
    CursorItemInfo* info0 = customs[index0];
    customs[index0] = customs[index1];
    customs[index1] = info0;
}

void PlayerInventory::set_bar_item(int index, CursorItemInfo* item) {
    if (index >= bar_items.size()) {
        while (index >= bar_items.size()) {
            bar_items.push_back(0);
        }
    }
    bar_items[index] = item;
}

void PlayerInventory::del_custom(CursorItem* item) {
    // see if it's on bar, delete if so
    for (int i = 0; i < bar_items.size(); i++) {
        if (*item->info == *bar_items[i]) {
            set_bar_item(i, 0);
        }
    }
    for (int i = 0; i < customs.size(); i++) {
        if (*customs[i] == *item->info) {
            delete_cursor_item_from_memory(item);
            delete customs[i];
            customs.erase(customs.begin() + i);
            break;
        }
    }
}

int PlayerInventory::load_self(IODataObject* obj) {
    energy_production = obj->read_value<int>();
    money = obj->read_value<int>();
    
    CursorItemInfo* info;
    
    int block_count = obj->read_value<int>();
    for (int i = 0; i < block_count; i++) {
        info = new CursorItemInfo();
        *info = obj->read_value<CursorItemInfo>();
        uint16_t blk = (uint16_t)info->vid;
        blocks[blk] = info;
        found_blocks.push_back(blk);
    }
    
    int model_count = obj->read_value<int>();
    for (int i = 0; i < model_count; i++) {
        info = new CursorItemInfo();
        *info = obj->read_value<CursorItemInfo>();
        uint16_t model = (uint16_t)info->vid;
        models[model] = info;
        found_models.push_back(model);
    }
    
    int custom_count = obj->read_value<int>();
    for (int i = 0; i < custom_count; i++) {
        info = new CursorItemInfo();
        *info = obj->read_value<CursorItemInfo>();
        customs.push_back(info);
    }
    
    CursorItemInfo temp = CursorItemInfo();
    CursorItemInfo* info_ptr = 0;
    
    int bar_items_count = obj->read_value<int>();
    for (int i = 0; i < bar_items_count; i++) {
        temp = obj->read_value<CursorItemInfo>();
        
        if (temp.type == CursorType::CURSOR_BLOCK) {
            // link it to the block cursorinfo
            info_ptr = blocks[temp.vid];
        }
        else if (temp.type == CursorType::CURSOR_MODEL) {
            // link it to the model cursorinfo
            info_ptr = models[temp.vid];
        }
        else if (temp.type == CursorType::CURSOR_SUPEROBJECT) {
            // link it to the custom cursorinfo
            for (auto &i: customs) {
                if (*i == temp) {
                    info_ptr = i;
                    break;
                }
            }
        }
        else {
            // TOFU support loading in other cursors, or not...
            info_ptr = 0;
        }
        bar_items.push_back(info_ptr);
    }
    return 0;
}

void PlayerInventory::save_self(IODataObject* obj) {
    obj->save_value(energy_production);
    obj->save_value(money);
    
    int block_count = (int)found_blocks.size();
    obj->save_value(block_count);
    for (int i = 0; i < block_count; i++) {
        uint16_t blk = found_blocks[i];
        obj->save_value(*blocks[blk]);
    }
    
    int model_count = (int)found_models.size();
    obj->save_value(model_count);
    for (int i = 0; i < model_count; i++) {
        uint16_t model = found_models[i];
        obj->save_value(*models[model]);
    }
    
    int custom_count = (int)customs.size();
    obj->save_value(custom_count);
    for (int i = 0; i < custom_count; i++) {
        obj->save_value(*customs[i]);
    }
    
    // TODO handle saving null items
    int bar_items_count = (int)bar_items.size();
    obj->save_value(bar_items_count);
    for (int i = 0; i < bar_items_count; i++) {
        obj->save_value(*bar_items[i]);
    }
}
