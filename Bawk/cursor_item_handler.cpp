//
//  cursor_item_handler.cpp
//  Bawk
//
//  Created by Eric Oh on 5/29/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "cursor_item_handler.h"
#include "item_bar.h"
#include "client_accessor.h"

void CursorItemHandler::frame(int ms) {
    CursorItem* cursor_item = get_item_bar()->get_current();
    if (cursor_item) {
        cursor_item->step(ms);
    }
}

bool CursorItemHandler::key_callback(Action do_this, int ms) {
    CursorItem* cursor_item = get_item_bar()->get_current();
    if (cursor_item) {
        return cursor_item->pushed(do_this);
    }
    return false;
}

bool CursorItemHandler::mouse_clicked_callback(Action do_this) {
    CursorItem* cursor_item = get_item_bar()->get_current();
    if (cursor_item) {
        return cursor_item->clicked(do_this);
    }
    return false;
}

bool CursorItemHandler::mouse_clicking_callback(Action do_this, int ms) {
    CursorItem* cursor_item = get_item_bar()->get_current();
    if (cursor_item) {
        return cursor_item->clicking(do_this, ms);
    }
    return false;
}

bool CursorItemHandler::scroll_callback(double xdiff, double ydiff) {
    return get_item_bar()->scroll_callback(xdiff, ydiff);
}
