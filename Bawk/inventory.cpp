//
//  inventory.cpp
//  Bawk
//
//  Created by Eric Oh on 10/16/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "inventory.h"
#include "cursorblock.h"
#include "cursorsuperobject.h"
#include "game_info_loader.h"

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
    // this is what a noob starts out with if he hasn't got jack shit
    // but hey, at least let's get you some basic blocks
    // now you can place air! by breathing it out! ...
    add_blocks(0, 0);
    // let's get you 3 blocks of 1 as well, why not
    add_blocks(1, 3);
    
    // let's get you all the blocks from 2 to 20, but give you only 1 of each
    for (int i = 2; i < 20; i++) {
        add_blocks(i, 1);
    }
    
    // lucky 7
    add_blocks(7, 3);
    
    // let's give you ten trees to play around with
    add_recipe_at(1, 10);
    
    // discovered_templates.push(...);
    // you haven't created any either, fuck off
    // created_templates.push(...);
    
    // add block 7 to first item of cursor
    cursor_item_identifier val;
    val.is_blk = true;
    val.is_recipe = false;
    val.bid = 7;
    val.pid = 0;
    val.vid = 0;
    cursor_items.push_back(val);
}

int PlayerInventory::get_block_count() {
    return (int)found_blocks.size();
}

int PlayerInventory::get_recipe_count() {
    return (int)found_recipes.size();
}

int PlayerInventory::get_custom_count() {
    return (int)customs.size();
}

CursorItem* PlayerInventory::get_block_at(int index) {
    if (index >= found_blocks.size())
        return 0;
    // this is slightly inefficient but whatevers
    if (found_blocks[index] == 0)
        return 0;
    return new CursorBlock(found_blocks[index]);
}

CursorItem* PlayerInventory::get_recipe_at(int index) {
    if (index >= found_recipes.size())
        return 0;
    uint16_t recipe_id = found_recipes[index];
    return get_recipe_cursoritem_from(recipe_id);
}

CursorItem* PlayerInventory::get_custom_at(int index) {
    if (index >= customs.size())
        return 0;
    uint32_t pid = customs[index].pid;
    uint32_t custom_id = customs[index].sid;
    CursorSuperObject* superobject = new CursorSuperObject(pid, custom_id);
    superobject->init();
    return superobject;
}

CursorItem* PlayerInventory::get_item_from(cursor_item_identifier distinguish) {
    if (distinguish.is_blk) {
        return new CursorBlock(distinguish.bid);
    }
    else if (distinguish.is_recipe) {
        return get_recipe_cursoritem_from(distinguish.vid);
    }
    else {
        // must be a custom superobject
        CursorSuperObject* superobject = new CursorSuperObject(distinguish.pid, distinguish.vid);
        superobject->init();
        return superobject;
    }
}

CursorItem* PlayerInventory::get_cursoritem_at(int index) {
    if (index >= cursor_items.size())
        return 0;
    cursor_item_identifier item = cursor_items[index];
    return get_item_from(item);
}

bool PlayerInventory::has_custom(uint32_t pid, uint32_t vid) {
    for (auto& i: customs) {
        if (i.pid == pid && i.sid == vid) {
            return true;
        }
    }
    return false;
}

bool PlayerInventory::has_cursor_item(uint32_t pid, uint32_t vid) {
    for (auto &i: cursor_items) {
        if (i.is_blk || i.is_recipe)
            continue;
        if (i.pid == pid && i.vid == vid) {
            return true;
        }
    }
    return false;
}

void PlayerInventory::add_blocks(uint16_t type, int count) {
    if (blocks.count(type)) {
        blocks[type] += count;
    }
    else {
        blocks[type] = count;
        insert_ordered(&found_blocks, type);
    }
}

void PlayerInventory::add_recipe_at(uint16_t type, int count) {
    if (recipes.count(type)) {
        recipes[type] += count;
    }
    else {
        recipes[type] = count;
        insert_ordered(&found_recipes, type);
    }
}

// for adding a custom item that previously didn't exist
void PlayerInventory::add_custom_at(CursorItem* to) {
    cursor_item_identifier distinguish = to->get_identifier();
    if (distinguish.is_blk || distinguish.is_recipe) {
        // this is not a custom item
        return;
    }
    // see if there already exists a custom with the same pid/sid
    if (has_custom(distinguish.pid, distinguish.vid))
        return;
    player_and_id val;
    val.pid = distinguish.pid;
    val.sid = distinguish.vid;
    customs.push_back(val);
}

// assume this is for a reordering
// later modify this to be insert_custom_at and remove the existing from the current index
void PlayerInventory::set_custom_at(CursorItem* to, int index) {
    printf("frog\n");
    cursor_item_identifier distinguish = to->get_identifier();
    if (distinguish.is_blk || distinguish.is_recipe) {
        // this is not a custom item
        return;
    }
    player_and_id val;
    val.pid = distinguish.pid;
    val.sid = distinguish.vid;
    customs.insert(customs.begin() + index, val);
}

void PlayerInventory::set_cursoritem_at(CursorItem* to, int index) {
    if (index >= cursor_items.size()) {
        cursor_item_identifier val;
        val.is_blk = true;
        val.is_recipe = false;
        val.bid = 0;
        val.pid = 0;
        val.vid = 0;
        while (index >= cursor_items.size()) {
            cursor_items.push_back(val);
        }
    }
    if ((!cursor_items[index].is_blk) && (!cursor_items[index].is_recipe)) {
        // not a block, not a recipe, so must be a custom item
        // see if this cursor item is in our inventory
        if (!has_custom(cursor_items[index].pid, cursor_items[index].vid)) {
            // if not in inventory, we're removing this object altogether from life and the universe
            // TODO the delete path below is wrong
            // TODO what if this item exists in another place in the itembar??? HUH??? you think bout that bitch?
            delete_at_path(get_path_to_template_folder(cursor_items[index].pid, cursor_items[index].vid));
        }
    }
    
    if (!to) {
        cursor_item_identifier val;
        val.is_blk = true;
        val.is_recipe = false;
        val.bid = 0;
        val.pid = 0;
        val.vid = 0;
        cursor_items[index] = val;
    } else {
        cursor_items[index] = to->get_identifier();
    }
}

void PlayerInventory::del_custom_at(CursorItem* item) {
    cursor_item_identifier distinguish = item->get_identifier();
    if (distinguish.is_blk || distinguish.is_recipe) {
        // this is not a custom item
        return;
    }
    player_and_id val;
    val.pid = distinguish.pid;
    val.sid = distinguish.vid;
    
    for (int i = 0; i < customs.size(); i++) {
        if (customs[i].pid == distinguish.pid &&
            customs[i].sid == distinguish.vid) {
            customs.erase(customs.begin() + i);
            if (!has_cursor_item(val.pid, val.sid)) {
                // delete from memory as well. we can't find our custom item on the item bar
                delete_at_path(get_path_to_template_folder(distinguish.pid, distinguish.vid));
            }
            break;
        }
    }
}

int PlayerInventory::load_self(IODataObject* obj) {
    energy_production = obj->read_value<int>();
    money = obj->read_value<int>();
    
    int block_count = obj->read_value<int>();
    for (int i = 0; i < block_count; i++) {
        uint16_t blk = obj->read_value<uint16_t>();
        int blkcount = obj->read_value<int>();
        found_blocks.push_back(blk);
        blocks[blk] = blkcount;
    }
    
    int recipe_count = obj->read_value<int>();
    for (int i = 0; i < recipe_count; i++) {
        uint16_t recipe = obj->read_value<uint16_t>();
        int rcpcount = obj->read_value<int>();
        found_recipes.push_back(recipe);
        recipes[recipe] = rcpcount;
    }
    
    int custom_count = obj->read_value<int>();
    for (int i = 0; i < custom_count; i++) {
        customs.push_back(obj->read_value<player_and_id>());
    }
    
    int cursor_items_count = obj->read_value<int>();
    for (int i = 0; i < cursor_items_count; i++) {
        cursor_items.push_back(obj->read_value<cursor_item_identifier>());
    }
    
    return 0;
}

void PlayerInventory::remove_self(IODataObject* obj) {
    obj->save_value(energy_production);
    obj->save_value(money);
    
    int block_count = (int)found_blocks.size();
    obj->save_value(block_count);
    for (int i = 0; i < block_count; i++) {
        uint16_t blk = found_blocks[i];
        obj->save_value(blk);
        obj->save_value(blocks[blk]);
    }
    
    int recipe_count = (int)found_recipes.size();
    obj->save_value(recipe_count);
    for (int i = 0; i < recipe_count; i++) {
        uint16_t recipe = found_recipes[i];
        obj->save_value(recipe);
        obj->save_value(recipes[recipe]);
    }
    
    int custom_count = (int)customs.size();
    obj->save_value(custom_count);
    for (int i = 0; i < custom_count; i++) {
        obj->save_value(customs[i]);
    }
    
    int cursor_items_count = (int)cursor_items.size();
    obj->save_value(cursor_items_count);
    for (int i = 0; i < cursor_items_count; i++) {
        obj->save_value(cursor_items[i]);
    }
}
