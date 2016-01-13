//
//  cursorselector.h
//  Bawk
//
//  Created by Eric Oh on 1/6/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__cursorselector__
#define __Bawk__cursorselector__

#include <stdio.h>
#include "cursoritem.h"

class CursorSelector: public CursorItem {
    Entity* selected;
    bool is_selected;
    bool is_block_selected;
    fvec3 selected_pos;
    void unselect();
    void select(Entity* ent);
public:
    CursorSelector();
    
    // methods to initialize/cleanup this entity
    void init() override;
    void reset() override;
    
    // behaviour when this cursor item is clicked
    bool clicked(Game* game, Action mouse) override;
    bool pushed(Game* game, Action key) override;
    
    // behaviour when this cursor item is entered
    bool confirmed(Game* game) override;
    bool canceled(Game* game) override;
    
    // render the item with transform in a small box or whatnot
    void render_item() override;
    // render the item in the world
    void render_in_world(fmat4* transform) override;
    
    cursor_item_identifier get_identifier() override;
};

#endif /* defined(__Bawk__cursorselector__) */
