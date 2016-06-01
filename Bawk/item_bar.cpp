//
//  item_bar.cpp
//  Bawk
//
//  Created by Eric Oh on 10/10/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "item_bar.h"

#include "cursorweapon.h"
#include "cursordrill.h"
#include "cursorscantool.h"

#include "client_accessor.h"
#include "game_window_holder.h"
#include "inventory_widget.h"

const int STATIC_BAR_ELEMENTS = 3;

ItemBar::ItemBar(int width, int height): ParentWidget(width, height) {
    inventory = get_player()->inventory;
    // pixel distance between each barlet
    int gaps_per_barlet = 1;
    // distance sum of all gaps we need
    int total_gap_length = (BAR_ITEMS - 1)*gaps_per_barlet;
    // the actual width of the barlet
    int per_barlet_width = (width - total_gap_length) / 10;
    // width of one barlet and one gap
    int barlet_n_gap = gaps_per_barlet + per_barlet_width;
    // evenly distribute the remaining width between the left and right
    int x_offset = (width - (per_barlet_width * BAR_ITEMS + total_gap_length)) / 2 + this->x;
    
    // set the bar to be at the bottom of the screen
    // bottom bitch
    this->y = 10;
    
    for (int i = 0; i < BAR_ITEMS; i++) {
        ItemBarlet* barlet = new ItemBarlet(x_offset + i*barlet_n_gap, this->y, per_barlet_width, height);
        if (i >= STATIC_BAR_ELEMENTS) {
            barlet->set_cursor_item(inventory->get_bar_item(i - STATIC_BAR_ELEMENTS));
        }
        else if (i == 0) {
            barlet->set_cursor_item(0); //new CursorWeapon()
        }
        else if (i == 1) {
            barlet->set_cursor_item(new CursorDrill(0));
        }
        else if (i == 2) {
            barlet->set_cursor_item(new CursorScanTool(0));
        }
        add_child(barlet);
    }
    index = 0;
    set_index(0);
}

ItemBar::~ItemBar() {
    for (int i = 0; i < BAR_ITEMS; i++) {
        delete (ItemBarlet*)children[i];
    }
}

CursorItem* ItemBar::get_current() {
    return ((ItemBarlet*)children[index])->get_cursor_item();
}

bool ItemBar::can_set_current() {
    return index >= STATIC_BAR_ELEMENTS;
}

void ItemBar::set_current(CursorItem* item) {
    if (index < STATIC_BAR_ELEMENTS)
        return;
    if (item) {
        inventory->set_bar_item(index - STATIC_BAR_ELEMENTS, item->info);
    }
    else {
        inventory->set_bar_item(index - STATIC_BAR_ELEMENTS, 0);
    }
    ((ItemBarlet*)children[index])->set_cursor_item(item);
}

void ItemBar::set_at_index(int i, CursorItem* item) {
    i += STATIC_BAR_ELEMENTS;
    if (item) {
        inventory->set_bar_item(i - STATIC_BAR_ELEMENTS, item->info);
    }
    else {
        inventory->set_bar_item(i - STATIC_BAR_ELEMENTS, 0);
    }
    ((ItemBarlet*)children[i])->set_cursor_item(item);
}

void ItemBar::set_index(int new_index) {
    ((ItemBarlet*)children[index])->set_current(false);
    index = (new_index % BAR_ITEMS + BAR_ITEMS) % BAR_ITEMS;
    ((ItemBarlet*)children[index])->set_current(true);
}

void ItemBar::set_to_left() {
    set_index(index - 1);
}

void ItemBar::set_to_right() {
    set_index(index + 1);
}

void ItemBar::set_first_available(CursorItem* item) {
    for (int i = STATIC_BAR_ELEMENTS; i < BAR_ITEMS; i++) {
        if (!((ItemBarlet*)children[i])->get_cursor_item()) {
            inventory->set_bar_item(i - STATIC_BAR_ELEMENTS, item->info);
            ((ItemBarlet*)children[i])->set_cursor_item(item);
            return;
        }
    }
    // all bar elements are full, simply override the first one I guess
    int i = STATIC_BAR_ELEMENTS;
    inventory->set_bar_item(0, item->info);
    ((ItemBarlet*)children[i])->set_cursor_item(item);
}

bool ItemBar::scrolled(int mx, int my, int px) {
    if (px > 0) {
        set_to_left();
    } else {
        set_to_right();
    }
    return true;
}

bool ItemBar::key_callback(Action do_this, int ms) {
    CursorItem* current = get_current();
    if (do_this >= KEY0 && do_this <= KEY9) {
        int to_index = do_this - KEY0;
        set_index(to_index);
    }
    else if (current) {
        // TODO move this. this should be handled in the game
        if (do_this == CONFIRM) {
            return current->confirmed();
        }
        else if (do_this == CANCEL) {
            return current->canceled();
        }
        else if (do_this == MOVE_BLOCK_UP) {
            printf("Moving placed template\n");
            current->handle_movement(ivec3(0, 1, 0));
        }
        else if (do_this == MOVE_BLOCK_DOWN) {
            printf("Moving placed template\n");
            current->handle_movement(ivec3(0, -1, 0));
        }
        else if (do_this == MOVE_BLOCK_LEFT) {
            printf("Moving placed template\n");
            ivec3 left = get_player()->get_rounded_left();
            current->handle_movement(ivec3(-left.x, 0, -left.z));
        }
        else if (do_this == MOVE_BLOCK_RIGHT) {
            printf("Moving placed template\n");
            current->handle_movement(get_player()->get_rounded_left());
        }
        else if (do_this == MOVE_BLOCK_FORWARD) {
            printf("Moving placed template\n");
            current->handle_movement(get_player()->get_rounded_forward());
        }
        else if (do_this == MOVE_BLOCK_BACKWARD) {
            printf("Moving placed template\n");
            ivec3 forward = get_player()->get_rounded_forward();
            current->handle_movement(ivec3(-forward.x, 0, -forward.z));
        }
        else if (do_this == MOVE_BLOCK_ROTATE) {
            current->handle_rotation();
        }
        else if (do_this == SAVE_TEMPLATE) {
            get_player()->inventory->add_custom(current->info);
            if (get_client()->game_window_holder->has_child(get_inventory_widget())) {
                get_inventory_widget()->refresh();
            }
        }
        else if (current->pushed(do_this)) {
            return true;
        }
        return false;
    }
    return false;
}
