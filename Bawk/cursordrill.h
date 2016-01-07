//
//  cursordrill.h
//  Bawk
//
//  Created by Eric Oh on 1/6/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__cursordrill__
#define __Bawk__cursordrill__

#include <stdio.h>
#include "cursoritem.h"

class CursorDrill: public CursorItem {
    float dps;
public:
    
    CursorDrill();
    
    // methods to initialize/cleanup this entity
    //virtual void init();
    //virtual void cleanup();
    //virtual void reset();
    
    // behaviour when this cursor item is clicked
    bool clicked(Game* game, Action mouse) override;
    //virtual bool pushed(Game* game, Action key) { return false; };
    
    // behaviour when this cursor item is entered
    //virtual bool confirmed(Game* game);
    //virtual bool canceled(Game* game);
    //virtual bool handle_movement(ivec3 dir);
    //virtual bool handle_rotation();
    //void step(Game* game) override;
    
    // render the item with transform in a small box or whatnot
    void render_item() override;
    // render the item in the world
    void render_in_world(fmat4* transform) override;
    //virtual void render_light_in_world(fmat4* transform, fvec3 player_pos) { };
    
    cursor_item_identifier get_identifier() override;
};

#endif /* defined(__Bawk__cursordrill__) */
