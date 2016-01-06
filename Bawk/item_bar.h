//
//  The item bar shown at the bottom of the screen
//  - Shows items available for quick selection
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
//  Created by Eric Oh on 10/10/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__item_bar__
#define __Bawk__item_bar__

#include <stdio.h>
#include "parent_widget.h"
#include "item_barlet.h"
#include "cursoritem.h"
#include "inventory.h"

const int BAR_ITEMS = 10;

class ItemBar: public ParentWidget {
    int index;
    PlayerInventory* inventory;
public:
    ItemBar(PlayerInventory* inv, int width, int height);
    ~ItemBar();
    CursorItem* get_current();
    bool can_set_current();
    void set_current(CursorItem* item);
    void set_index(int new_index);
    void set_to_left();
    void set_to_right();
};

#endif /* defined(__Bawk__item_bar__) */
