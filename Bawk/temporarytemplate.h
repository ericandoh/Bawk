//
//  A holding class used to save data about what blocks have been placed while in create template mode
//  -
//  -
//  -
//
//  Used by:
//  -
//
//  Uses:
//  -
//  -
//  -
//
//  Notes
//  -
//  -
//  Created by Eric Oh on 10/6/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__temporarytemplate__
#define __Bawk__temporarytemplate__

#include <stdio.h>
#include <vector>
#include "basic_types.h"
#include "world.h"
#include "superobjectrender.h"

struct block_data {
    ivec3 position;
    block_type block;
    block_data(ivec3 pos, block_type b) {
        position= pos;
        block = b;
    }
};

// represents a template in the making
class TemporaryTemplate: public RenderableSuperObject {
    std::vector<block_data> blocks;
    // if this is toggled, when finalizing add blocks not to the world
    // but to its own independent superobject
    int will_be_independent;
public:
    TemporaryTemplate();
    void add_block(ivec3 position, block_type block);
    void remove_block(ivec3 position);
    std::vector<block_data> publish(Player* player, World* world);
    void unpublish(World* world);
    
    void render(fmat4* transform) override;
    
    void update_chunks(fvec3* old_pos, fvec3* new_pos) override;
    int get_chunk(block_type to_arr[CX][CY][CZ], int x, int y, int z) override;
    int save_chunk(block_type from_arr[CX][CY][CZ], int x, int y, int z) override;
};

#endif /* defined(__Bawk__temporarytemplate__) */
