//
//  cursorsuperobject.h
//  Bawk
//
//  Created by Eric Oh on 10/6/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__cursorsuperobject__
#define __Bawk__cursorsuperobject__

#include <stdio.h>
#include "cursoritem.h"
#include "superobjectrender.h"

// Represents a template that can be put down

class CursorSuperObject: public CursorItem, public RenderableSuperObject {
    bool locked;
public:
    CursorSuperObject();
    
    // --- cursoritem methods ---
    
    // sets the blocks in this representation into the world, and if template is not null, into the
    // template as well
    void set_blocks(World* world, TemporaryTemplate* temp) override;
    // for a single block, this will call set_blocks (above) directly.
    // for a template block, this will lock the position of the current cursoritem template
    // then a call to set_blocks will be made later
    bool place_blocks(World* world, TemporaryTemplate* temp) override;
    // only needed for instances of template. the default does jack shit
    void move_block(ivec3 dir) override;
    void unlock();
    
    // --- rendersuperobject methods ---
    void update_chunks(fvec3* old_pos, fvec3* new_pos);
    // called when a request to load a chunk from disk is made
    int get_chunk(uint16_t to_arr[CX][CY][CZ], int x, int y, int z) override;
    // called when a chunk goes out of scope and no longer needs to be rendered
    int save_chunk(uint16_t from_arr[CX][CY][CZ], int x, int y, int z) override;
    bool within_dimensions_chunk(int x, int y, int z) override;
    void remove_self();
};

CursorSuperObject* create_from_template(World* world, TemporaryTemplate* temp);

#endif /* defined(__Bawk__cursorsuperobject__) */
