//
//  cursorblock.cpp
//  Bawk
//
//  Created by Eric Oh on 10/6/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "cursorblock.h"
#include "worldrender.h"

CursorBlock::CursorBlock(uint16_t type) {
    block = type;
}

// sets the blocks in this representation into the world, and if template is not null, into the
// template as well
void CursorBlock::set_blocks(World* world, TemporaryTemplate* temp) {
    ivec4 looking_at = get_look_at();
    int mx = looking_at.x;
    int my = looking_at.y;
    int mz = looking_at.z;
    int face = looking_at.w;
    if(face == 0)
        mx++;
    if(face == 3)
        mx--;
    if(face == 1)
        my++;
    if(face == 4)
        my--;
    if(face == 2)
        mz++;
    if(face == 5)
        mz--;
    ivec3 block_pos = ivec3(mx, my, mz);
    world->place_block(block_pos, block);
    //printf("Placing at (%d, %d, %d)   (face %d)\n",
    //       mx, my, mz, face);
    if (temp)
        temp->add_block(block_pos, block);
}
// for a single block, this will call set_blocks (above) directly.
// for a template block, this will lock the position of the current cursoritem template
// then a call to set_blocks will be made later
bool CursorBlock::place_blocks(World* world, TemporaryTemplate* temp) {
    set_blocks(world, temp);
    // this gets passed to placed_current_item, that needs to be set to false
    // since this is a single block and is already placed
    return false;
}
// only needed for instances of template. the default does jack shit
void CursorBlock::move_block(ivec3 dir) {
    printf("Warning: Move block called on a singe block entity");
}