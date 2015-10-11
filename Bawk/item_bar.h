//
//  item_bar.h
//  Bawk
//
//  Created by Eric Oh on 10/10/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__item_bar__
#define __Bawk__item_bar__

#include <stdio.h>
#include "base_widget.h"
#include "item_barlet.h"
#include "cursoritem.h"

const int BAR_ITEMS = 10;

class ItemBar: public BaseWidget {
    ItemBarlet* barlets[BAR_ITEMS];
    int index;
public:
    ItemBar(int width, int height);
    ~ItemBar();
    void render_elements() override;
    CursorItem* get_current();
    void set_current(CursorItem* item);
    void set_index(int new_index);
    void set_to_left();
    void set_to_right();
};

#endif /* defined(__Bawk__item_bar__) */
