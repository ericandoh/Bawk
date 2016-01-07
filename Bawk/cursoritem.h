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
#include "basic_types.h"
#include "game_actions.h"
#include "block_orientation.h"

struct cursor_item_identifier {
    bool is_blk;
    // TODO deprecate is_recipe
    bool is_recipe;
    uint16_t bid;
    uint32_t pid;
    uint32_t vid;
};

class Game;
class Entity;
class SuperObject;

// represents an item (either a single block or a template, pretty much)
class CursorItem {
    
protected:
    // --- Placement related ---
    SuperObject* target;
    bool show_item;
    ivec3 pointing_pos;
    BlockOrientation pointing_orientation;
    
    void update_pointing_position(Game* game, ivec3 dimensions);
    
public:
    CursorItem();
    
    // methods to initialize/cleanup this entity
    virtual void init();
    virtual void cleanup();
    virtual void reset();
    
    // behaviour when this cursor item is clicked
    virtual bool clicked(Game* game, Action mouse);
    virtual bool pushed(Game* game, Action key) EMPTY_BOOL_FUNCTION;
    
    // behaviour when this cursor item is entered
    virtual bool confirmed(Game* game);
    virtual bool canceled(Game* game);
    virtual bool handle_movement(ivec3 dir);
    virtual bool handle_rotation();
    
    // behaviour to update at each step
    virtual void step(Game* game) EMPTY_FUNCTION;
    
    // render the item with transform in a small box or whatnot
    virtual void render_item() = 0;
    // render the item in the world
    virtual void render_in_world(fmat4* transform) = 0;
    virtual void render_light_in_world(fmat4* transform, fvec3 player_pos) EMPTY_FUNCTION;
    
    virtual cursor_item_identifier get_identifier() = 0;
};

#endif /* defined(__Bawk__cursoritem__) */
