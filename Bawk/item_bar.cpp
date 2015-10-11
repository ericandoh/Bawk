//
//  item_bar.cpp
//  Bawk
//
//  Created by Eric Oh on 10/10/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "item_bar.h"

ItemBar::ItemBar(int width, int height): BaseWidget(width, height) {
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
        barlets[i] = new ItemBarlet(x_offset + i*barlet_n_gap, this->y, per_barlet_width, height);
    }
}

ItemBar::~ItemBar() {
    for (int i = 0; i < BAR_ITEMS; i++) {
        delete barlets[i];
    }
}

void ItemBar::render_elements() {
    for (int i = 0; i < 10; i++) {
        barlets[i]->render();
    }
    // TODO draw a box around our current item
}

CursorItem* ItemBar::get_current() {
    return barlets[index]->get_cursor_item();
}

void ItemBar::set_current(CursorItem* item) {
    barlets[index]->set_cursor_item(item);
}

void ItemBar::set_index(int new_index) {
    index = (new_index % BAR_ITEMS + BAR_ITEMS) % BAR_ITEMS;
}

void ItemBar::set_to_left() {
    set_index(index - 1);
}

void ItemBar::set_to_right() {
    set_index(index + 1);
}