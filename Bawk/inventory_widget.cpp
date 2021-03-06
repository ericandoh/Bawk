//
//  inventory_widget.cpp
//  Bawk
//
//  Created by Eric Oh on 10/16/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "inventory_widget.h"
#include "worldrender.h"
#include "client_accessor.h"

SwitchInventoryButton::SwitchInventoryButton(MainInventoryWidget* o,
                                             InventoryButtonAction b,
                                             int x, int y,
                                             int width, int height): BaseWidget(x, y, width, height) {
    owner = o;
    bid = b;
}

bool SwitchInventoryButton::onclick(int mx, int my, Action button) {
    owner->switch_view(bid);
    return true;
}

void SwitchInventoryButton::render_elements() {
    // first, fill with black box
    OGLAttr::current_shader->set_block_draw_mode(BlockDrawMode::COLOR);
    float vertex[6][3] = {
        {-1, -1, 0},
        {1, -1, 0},
        {-1, 1, 0},
        {-1, 1, 0},
        {1, -1, 0},
        {1, 1, 0},
    };
    
    // use textures for this later for pretty buttons!
    /*float texture[6][3] = {
     {0, 0, 0},
     {1, 0, 0},
     {0, 1, 0},
     {0, 1, 0},
     {1, 0, 0},
     {1, 1, 0},
     };*/
    float xfill, yfill, zfill;
    xfill = yfill = zfill = 0;
    if (bid == InventoryButtonAction::TO_BLOCKS) {
        xfill = 0.3f;
        yfill = 0.4f;
        zfill = 0.5f;
    }
    else if (bid == InventoryButtonAction::TO_MODELS) {
        xfill = 0.5f;
        yfill = 0.4f;
        zfill = 0.3f;
    }
    else {
        xfill = 0.3f;
        yfill = 0.5f;
        zfill = 0.4f;
    }
    float texture[6][3];
    for (int p = 0; p < 6; p++) {
        texture[p][0] = xfill;
        texture[p][1] = yfill;
        texture[p][2] = zfill;
    }
    
    set_unitary_transform_matrix();
    
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof vertex, vertex, GL_DYNAMIC_DRAW);
    OGLAttr::current_shader->set_coord_attribute(GL_FLOAT);
    
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_texture_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof texture, texture, GL_DYNAMIC_DRAW);
    OGLAttr::current_shader->set_texture_coord_attribute(GL_FLOAT);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

ScrollInventoryWidget::ScrollInventoryWidget(ItemBar* ib, InventoryButtonAction t,
                                             int rw, int rh, int mr,
                                             int x, int y, int width, int height):
ScrollingWidget(rw, rh, mr, x, y, width, height) {
    itembar = ib;
    inventory = get_player()->inventory;
    fetch = t;
    maxrows = 0;
    total_count = 0;
    switch_button_action(InventoryButtonAction::TO_BLOCKS);
}

ScrollInventoryWidget::~ScrollInventoryWidget() {
    for (auto &child: children) {
        delete (ItemBarlet*)child;
    }
}

BaseWidget* ScrollInventoryWidget::set_row(int index, BaseWidget* current) {
    clear_child(current);
    ItemBarlet* barlet;
    if (current) {
        barlet = (ItemBarlet*) current;
    }
    else {
        // make a new widget and return it
        // dimensions will be set by the holding entity, dont worry!
        barlet = new ItemBarlet(0, 0, 1, 1);
    }
    if (index >= total_count) {
        return barlet;
    }
    
    if (fetch == InventoryButtonAction::TO_BLOCKS) {
        barlet->set_cursor_item(inventory->get_block(index));
    }
    else if (fetch == InventoryButtonAction::TO_MODELS) {
        barlet->set_cursor_item(inventory->get_model(index));
    }
    else if (fetch == InventoryButtonAction::TO_CUSTOM) {
        barlet->set_cursor_item(inventory->get_custom(index));
    }
    return barlet;
}

void ScrollInventoryWidget::clear_child(BaseWidget* child) {
    if (!child)
        return;
    ((ItemBarlet*)child)->set_cursor_item(0);
}

void ScrollInventoryWidget::switch_button_action(InventoryButtonAction to) {
    if (fetch == to)
        return;
    fetch = to;
    refresh();
}

bool ScrollInventoryWidget::onclick(BaseWidget* clicked_child, int mx, int my, Action button) {
    // TOFU move this to a mouse binding method...
    // move item that is here onto the item bar
    ItemBarlet* barlet = (ItemBarlet*)clicked_child;
    CursorItem* item = barlet->get_cursor_item();
    if (button == Action::CLICK_MAIN) {
        // TOFU discard this template, later add in a warning button or some shizzle
        if (fetch == InventoryButtonAction::TO_CUSTOM) {
            int bar_index = inventory->find_bar_item(item->info);
            if (bar_index >= 0) {
                itembar->set_at_index(bar_index, 0);
            }
            inventory->del_custom(item);
            // TODO this is incredibly lazy, isn't there a better way to just shift all items up by 1?
            refresh();
        }
    }
    else if (button == Action::CLICK_SECONDARY) {
        // move item to bar
        if (itembar->can_set_current()) {
            if (item) {
                itembar->set_current(inventory->get_item(item->info));
            }
            else {
                itembar->set_current(0);
            }
        }
    }
    return true;
}

void ScrollInventoryWidget::refresh() {
    total_count = 0;
    if (fetch == InventoryButtonAction::TO_BLOCKS) {
        total_count = inventory->get_block_count();
    }
    else if (fetch == InventoryButtonAction::TO_MODELS) {
        total_count = inventory->get_model_count();
    }
    else if (fetch == InventoryButtonAction::TO_CUSTOM) {
        total_count = inventory->get_custom_count();
    }
    set_max_count(total_count);
    ScrollingWidget::refresh();
}

MainInventoryWidget::MainInventoryWidget(ItemBar* ib,
                                         int width, int height): ParentWidget(width, height) {
    current_view = InventoryButtonAction::SETUP;
    
    int bx1, bx2, bx3;
    int by;
    int bw, bh;
    
    bh = height / 10 - 4;
    by = height - (height / 10) + 2 + y;
    bw = (width - 8) / 3;
    bx1 = x;
    bx2 = x + bw + 4;
    bx3 = x + bw * 2 + 8;
    SwitchInventoryButton* bt0 = new SwitchInventoryButton(this, InventoryButtonAction::TO_BLOCKS, bx1, by, bw, bh);
    SwitchInventoryButton* bt1 = new SwitchInventoryButton(this, InventoryButtonAction::TO_MODELS, bx2, by, bw, bh);
    SwitchInventoryButton* bt2 = new SwitchInventoryButton(this, InventoryButtonAction::TO_CUSTOM, bx3, by, bw, bh);
    
    add_child(bt0);
    add_child(bt1);
    add_child(bt2);
    
    // scroll height
    int scrh = height - (height / 10);
    // row height
    int rw = width / 8;
    int rh = rw;
    
    ScrollInventoryWidget* scrolling = new ScrollInventoryWidget(ib, InventoryButtonAction::SETUP, rw, rh, 0, x, y, width, scrh);
    scrolling->setup();
    add_child(scrolling);
}

MainInventoryWidget::~MainInventoryWidget() {
    delete (SwitchInventoryButton*)children[0];
    delete (SwitchInventoryButton*)children[1];
    delete (SwitchInventoryButton*)children[2];
    delete (ScrollInventoryWidget*)children[3];
}

void MainInventoryWidget::refresh() {
    ((ScrollInventoryWidget*)children[3])->refresh();
}

void MainInventoryWidget::switch_view(InventoryButtonAction action) {
    if (current_view == action)
        return;
    // do stuff
    current_view = action;
    ((ScrollInventoryWidget*)children[3])->switch_button_action(current_view);
}