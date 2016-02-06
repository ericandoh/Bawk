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
#include "cursoritem_loader.h"


class CursorItemInfo {
public:
    CursorType type;
    uint32_t pid;
    uint32_t vid;
    int count;
    CursorItemInfo() {
        type = CursorType::CURSOR_DEFAULT;
        pid = vid = 0;
        count = 0;
    }
    CursorItemInfo(CursorType t) {
        type = t;
        pid = vid = 0;
        count = 0;
    }
    CursorItemInfo(uint32_t p, uint32_t v, CursorType t) {
        type = t;
        pid = p;
        vid = v;
        count = 0;
    }
    CursorItemInfo(uint32_t v, CursorType t) {
        type = t;
        pid = 0;
        vid = v;
        count = 0;
    }
    bool operator==(const CursorItemInfo &other) const
    {
        return (type == other.type
                && pid == other.pid
                && vid == other.vid);
    }
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
    CursorItemInfo* info;
    CursorItem(CursorItemInfo* i);
    virtual ~CursorItem() = default;
    
    // methods to initialize/cleanup this entity
    virtual void init() EMPTY_FUNCTION;
    virtual void cleanup();
    virtual void reset() EMPTY_FUNCTION;
    
    // behaviour when this cursor item is clicked
    virtual bool clicked(Game* game, Action mouse);
    virtual bool clicking(Game* game, Action mouse, int ms) EMPTY_BOOL_FUNCTION;
    virtual bool pushed(Game* game, Action key) EMPTY_BOOL_FUNCTION;
    
    // behaviour when this cursor item is entered
    virtual bool confirmed(Game* game);
    virtual bool canceled(Game* game);
    virtual bool handle_movement(ivec3 dir);
    virtual bool handle_rotation();
    
    // behaviour to update at each step
    virtual void step(Game* game, int ms) EMPTY_FUNCTION;
    
    // render the item with transform in a small box or whatnot
    virtual void render_item() = 0;
    // render the item in the world
    virtual void render_in_world(fmat4* transform) = 0;
    virtual void render_light_in_world(fmat4* transform, fvec3 player_pos) EMPTY_FUNCTION;
    
    virtual bool has_count() EMPTY_BOOL_FUNCTION;
    virtual int get_count();
};

#endif /* defined(__Bawk__cursoritem__) */
