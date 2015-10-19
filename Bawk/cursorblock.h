//
//  Represents a cursoritem holding a single block
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
//  - The only class outside "render" that uses OpenGL stuff
//  - That might change later but too much of a hassle right now
//  - Could have just used a cursorsuperobject with a single block, but I felt like that was a waste :\
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
    block_type block;
    void render_block(fmat4* transform, float bx, float by, float bz);
public:
    CursorBlock(block_type type);
    // sets the blocks in this representation into the world, and if template is not null, into the
    // template as well
    bool set_blocks(Player* player, World* world, TemporaryTemplate* temp) override;
    // for a single block, this will call set_blocks (above) directly.
    // for a template block, this will lock the position of the current cursoritem template
    // then a call to set_blocks will be made later
    bool place_blocks(Player* player, World* world, TemporaryTemplate* temp) override;
    // only needed for instances of template. the default does jack shit
    void move_block(ivec3 dir) override;
    void get_bounds(ivec3* upper) override;
    void render_at_zero(fmat4* transform) override;
    void render_and_position(fmat4* transform) override;
};

void delete_cursorblockvbos();

#endif /* defined(__Bawk__cursorblock__) */
