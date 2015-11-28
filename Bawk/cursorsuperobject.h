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
#include "placeablesuperobject.h"

// Represents a template that can be put down

class CursorSuperObject: public PlaceableSuperObject, public CursorItem {
    bool locked;
    bool is_recipe;
public:
    // for recipes
    CursorSuperObject(uint32_t s);
    // for custom cursorsuperobjects
    CursorSuperObject(uint32_t p, uint32_t s);
    
    // --- CursorSuperObject ---
    
    // --- CursorItem ---
    void init() override;
    void cleanup() override;
    void reset() override;
    bool clicked(Game* game, Action mouse, Entity* on) override;
    bool confirmed(Game* game) override;
    bool canceled(Game* game) override;
    bool handle_movement(ivec3 dir) override;
    void render_item() override;
    void render_in_world(fmat4* transform) override;
    
    cursor_item_identifier get_identifier() override;
    
    // --- PlaceableSuperObject ---
    
    // --- SuperObject ---
    int get_chunk(block_type to_arr[CX][CY][CZ], int x, int y, int z) override;
    std::string get_save_path() override;
    std::string get_chunk_save_path(ivec3* pos) override;
    int load_self(IODataObject* obj) override;
    void remove_self(IODataObject* obj) override;
};

#endif /* defined(__Bawk__cursorsuperobject__) */
