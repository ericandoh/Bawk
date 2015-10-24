//
//  temporarytemplate.cpp
//  Bawk
//
//  Created by Eric Oh on 10/6/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "temporarytemplate.h"
#include "game_info_loader.h"

TemporaryTemplate::TemporaryTemplate() {
    will_be_independent = 0;
}

void TemporaryTemplate::add_block(ivec3 position, block_type block) {
    blocks.push_back(block_data(position, block));
    if (get_block_independence(block.type)) {
        will_be_independent++;
    }
}

void TemporaryTemplate::remove_block(ivec3 position) {
    for (unsigned int i = 0; i < blocks.size(); i++) {
        if (blocks.at(i).position == position) {
            if (get_block_independence(blocks.at(i).block.type)) {
                will_be_independent--;
            }
            blocks.erase(blocks.begin() + i);
            break;
        }
    }
}

std::vector<block_data> TemporaryTemplate::publish(Player* player, World* world) {
    if (will_be_independent) {
        // first, remove all blocks from the baseworld
        unpublish(world);
        
        // then use that to find the translation/other properties of this object
        
        // now package it into a superobject
        SuperObject* superobject = world->create_superobject(player);
        // and add the blocks in this template to the superobject. There should be no conflicts here
        
        for (auto &i : blocks) {
            ivec3 pos = i.position;
            superobject->set_block(pos.x, pos.y, pos.z, i.block);
        }
    }
    return blocks;
}

void TemporaryTemplate::unpublish(World* world) {
    // remove ALL blocks from this world that belong to this template
    for (auto &i : blocks) {
        ivec3 position = i.position;
        // placing a block of 0 effectively "destroys" the block at that location
        world->place_block(position, 0);
    }
}