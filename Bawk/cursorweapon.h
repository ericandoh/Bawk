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
    CursorWeapon(CursorItemInfo* i);
    
    bool clicked(Action mouse) override;
    bool clicking(Action mouse, int ms) override;
    
    //void step(int ms) override;
    
    // render the item with transform in a small box or whatnot
    void render_item() override EMPTY_FUNCTION;
    // render the item in the world
    void render_in_world(fmat4* transform) override EMPTY_FUNCTION;
};

#endif /* defined(__Bawk__cursorweapon__) */
