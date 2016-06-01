//
//  game_window_holder.h
//  Bawk
//
//  UI element in game that opens
//
//  Created by Eric Oh on 5/29/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__game_window_holder__
#define __Bawk__game_window_holder__

#include <stdio.h>
#include "parent_widget.h"

class MainInventoryWidget;
class ItemBar;

class GameWindowHolder: public ParentWidget {
    
    bool is_cursor_free;
    
    // inventory UI
    MainInventoryWidget* inventory_ui;
    ItemBar* bar;
    
    void validate_cursor_freedom();
public:
    GameWindowHolder();
    
    ItemBar* get_item_bar();
    MainInventoryWidget* get_inventory_widget();
    
    // --- Displayable ---
    void init() override;
    
    bool mouse_move_callback(double xdiff, double ydiff) override;
    bool key_callback(Action do_this, int ms) override;
};

#endif /* defined(__Bawk__game_window_holder__) */
