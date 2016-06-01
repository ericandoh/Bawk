//
//  game_window_holder.cpp
//  Bawk
//
//  Created by Eric Oh on 5/29/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "game_window_holder.h"

#include "display.h"

#include "item_bar.h"
#include "inventory_widget.h"

// UI that will always show
const int STATIC_UI_ELEMENT_COUNT = 1;

GameWindowHolder::GameWindowHolder() {
    is_cursor_free = false;
}

void GameWindowHolder::validate_cursor_freedom() {
    if (count_children() > STATIC_UI_ELEMENT_COUNT) {
        // we have at least 1 child. cursor should be free
        if (!is_cursor_free) {
            is_cursor_free = true;
            display_enable_cursor();
        }
    } else {
        // we have no children. cursor should be locked
        if (is_cursor_free) {
            is_cursor_free = false;
            display_disable_cursor();
        }
    }
}

ItemBar* GameWindowHolder::get_item_bar() {
    return bar;
}

MainInventoryWidget* GameWindowHolder::get_inventory_widget() {
    return inventory_ui;
}

void GameWindowHolder::init() {
    int width, height;
    get_window_size(&width, &height);
    set_dimensions(0, 0, width, height);
    
    int bar_width = width * 7 / 100;
    bar = new ItemBar(bar_width*10, bar_width);
    bar->set_index(3);
    // we always show the item bar
    add_child(bar);
    
    inventory_ui = new MainInventoryWidget(bar, width / 2, height / 2);
    
}

bool GameWindowHolder::mouse_move_callback(double xdiff, double ydiff) {
    // if cursor is free, capture this event
    return is_cursor_free;
}

bool GameWindowHolder::key_callback(Action do_this, int ms) {
    if (do_this == CANCEL) {
        if (count_children() > STATIC_UI_ELEMENT_COUNT) {
            close_latest_child();
            validate_cursor_freedom();
            return true;
        }
    } else if (do_this == OPEN_INV) {
        toggle_child(inventory_ui);
        validate_cursor_freedom();
        return true;
    }
    
    return ParentWidget::key_callback(do_this, ms);
}