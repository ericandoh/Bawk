//
//  cursorblock.h
//  Bawk
//
//  Created by Eric Oh on 10/6/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__cursorblock__
#define __Bawk__cursorblock__

#include <stdio.h>
#include "cursoritem.h"

// Represents a block that can be put down

class CursorBlock: public CursorItem {
    uint16_t block;
public:
    CursorBlock(uint16_t type);
    ~CursorBlock();
    // sets the blocks in this representation into the world, and if template is not null, into the
    // template as well
    void set_blocks(World* world, TemporaryTemplate* temp) override;
    // for a single block, this will call set_blocks (above) directly.
    // for a template block, this will lock the position of the current cursoritem template
    // then a call to set_blocks will be made later
    bool place_blocks(World* world, TemporaryTemplate* temp) override;
    // only needed for instances of template. the default does jack shit
    void move_block(ivec3 dir) override;
    void render_and_position(fmat4* transform);
};

#endif /* defined(__Bawk__cursorblock__) */
