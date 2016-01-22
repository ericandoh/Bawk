//
//  cursorweapon.h
//  Bawk
//
//  Created by Eric Oh on 1/6/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__cursorweapon__
#define __Bawk__cursorweapon__

#include <stdio.h>
#include "cursoritem.h"

class CursorWeapon: public CursorItem {
    float dmg;
public:
    CursorWeapon();
    
    bool clicked(Game* game, Action mouse) override;
    bool clicking(Game* game, Action mouse, int ms) override;
    
    //void step(Game* game, int ms) override;
    
    // render the item with transform in a small box or whatnot
    void render_item() override EMPTY_FUNCTION;
    // render the item in the world
    void render_in_world(fmat4* transform) override EMPTY_FUNCTION;
    
    cursor_item_identifier get_identifier();
};

#endif /* defined(__Bawk__cursorweapon__) */
