//
//  item_barlet.h
//  Bawk
//
//  Created by Eric Oh on 10/10/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__item_barlet__
#define __Bawk__item_barlet__

#include <stdio.h>
#include "base_widget.h"
#include "cursoritem.h"

class ItemBarlet: public BaseWidget {
    CursorItem* entity;
public:
    ItemBarlet(int x, int y, int width, int height);
    void render_elements() override;
    CursorItem* get_cursor_item();
    void set_cursor_item(CursorItem* item);
};

#endif /* defined(__Bawk__item_barlet__) */
