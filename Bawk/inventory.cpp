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

PlayerInventory::PlayerInventory() {
    // start with no energy production
    energy_production = 0;
    // and no money...hey at least its a step up from being IN DEBT AFTER YOU GRADUATE...fml
    money = 0;
    
    // but hey, at least let's get you some basic blocks
    // now you can place air! by breathing it out! ...
    blocks.push_back(block_and_count(0, 0));
    // let's get you 3 blocks of 1 as well, why not
    blocks.push_back(block_and_count(1, 3));
    
    // you haven't discovered any templates, fuck off
    // discovered_templates.push(...);
    // you haven't created any either, fuck off
    // created_templates.push(...);
}

int PlayerInventory::get_block_count() {
    return (int)blocks.size();
}

int PlayerInventory::get_recipe_count() {
    return (int)recipes.size();
}

int PlayerInventory::get_custom_count() {
    return (int)customs.size();
}

CursorItem* PlayerInventory::get_block_at(int index) {
    return new CursorBlock(blocks[index].type);
}

CursorItem* PlayerInventory::get_recipe_at(int index) {
    uint16_t recipe_id = recipes[index];
    // TODO fetch recipe by recipe_id
    return 0;
}

CursorItem* PlayerInventory::get_custom_at(int index) {
    uint32_t pid = customs[index].pid;
    uint32_t custom_id = customs[index].sid;
    // TODO fetch custom build by pid and custom_id
    CursorSuperObject* superobject = new CursorSuperObject(pid, custom_id, true, false);
    superobject->read_in_all();
    return superobject;
}