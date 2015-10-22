//
//  A collection of blocks that can be quick selected and placed all at once
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

#ifndef __Bawk__cursorsuperobject__
#define __Bawk__cursorsuperobject__

#include <stdio.h>
#include "cursoritem.h"
#include "superobject.h"

// Represents a template that can be put down

class CursorSuperObject: public SuperObject, public CursorItem {
    // if this object needs to be loaded from memory
    bool loaded;
    bool locked;
    
public:
    CursorSuperObject(uint32_t p, uint32_t s);
    
    // --- cursoritem methods ---
    
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
    bool is_locked_in() override;
    void unlock() override;
    
    // --- rendersuperobject methods ---
    void update_chunks(fvec3* old_pos, fvec3* new_pos) override;
    // called when a request to load a chunk from disk is made
    int get_chunk(block_type to_arr[CX][CY][CZ], int x, int y, int z) override;
    bool within_dimensions_chunk(int x, int y, int z) override;
    
    std::string get_save_path() override;
    std::string get_chunk_save_path(ivec3* pos) override;
    
    
    void load_all() override;
    
    cursor_item_distinguisher get_distinguisher() override;
    void delete_self() override;
};

CursorSuperObject* create_from_template(Player* player, World* world, TemporaryTemplate* temp);

#endif /* defined(__Bawk__cursorsuperobject__) */
