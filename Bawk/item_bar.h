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
    ItemBar(int width, int height);
    ~ItemBar();
    CursorItem* get_current();
    bool can_set_current();
    void set_current(CursorItem* item);
    void set_at_index(int i, CursorItem* item);
    void set_index(int new_index);
    void set_to_left();
    void set_to_right();
    
    void set_first_available(CursorItem* item);
    
    // --- BaseWidget ---
    bool scrolled(int mx, int my, int px) override;
    
    // --- InputReceiver ---
    bool key_callback(Action do_this, int ms) override;
};

#endif /* defined(__Bawk__item_bar__) */
