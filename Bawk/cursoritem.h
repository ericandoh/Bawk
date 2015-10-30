//
//  Represents a quick selectable entity that can be placed down. Made of blocks
//  - Either a single block (cursoritem) or multiple blocks
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
//
//  Created by Eric Oh on 10/6/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__cursoritem__
#define __Bawk__cursoritem__

#include <stdio.h>
#include "world.h"

class TemporaryTemplate;

// represents an item (either a single block or a template, pretty much)
class CursorItem {
public:
    bool update_pointing_position(int* tx, int* ty, int* tz, BlockOrientation* orient, bool nonempty);
    // sets the blocks in this representation into the world, and if template is not null, into the
    // template as well
    virtual bool set_blocks(Player* player, World* world, TemporaryTemplate* temp) = 0;
    // for a single block, this will call set_blocks (above) directly.
    // for a template block, this will lock the position of the current cursoritem template
    // then a call to set_blocks will be made later
    virtual bool place_blocks(Player* player, World* world, TemporaryTemplate* temp) = 0;
    // only needed for instances of template. the default does jack shit
    virtual void move_block(ivec3 dir) = 0;
    virtual void get_bounds(ivec3* upper) = 0;
    virtual void render_at_zero(fmat4* transform) = 0;
    virtual void render_and_position(fmat4* transform) = 0;
    virtual bool is_locked_in();
    virtual void unlock();
    
    virtual cursor_item_distinguisher get_distinguisher() = 0;
    virtual void delete_self() = 0;
    
    virtual void load_all();
};

#endif /* defined(__Bawk__cursoritem__) */
