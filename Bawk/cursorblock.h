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
#include "placeableobject.h"
#include "cursoritem.h"
#include "block.h"

// Represents a block that can be put down

class CursorBlock: public PlaceableObject, public CursorItem {
    bool show_item;
    void render_block(fmat4* transform);
public:
    block_type block;
    fvec3 pos;
    CursorBlock(block_type type);
    
    // --- PlaceableObject
    bool set_blocks(Player* player, World* world, SuperObject* object) override;
    
    // --- CursorItem ---
    bool clicked(Game* game, Action mouse) override;
    bool confirmed(Game* game) override;
    void step() override;
    void render_item() override;
    void render_in_world(fmat4* transform) override;
    void render_light_in_world(fmat4* transform, fvec3 player_pos) override;

    cursor_item_identifier get_identifier() override;
};

#endif /* defined(__Bawk__cursorblock__) */
