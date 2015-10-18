//
//  inventory_widget.cpp
//  Bawk
//
//  Created by Eric Oh on 10/16/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "inventory_widget.h"

const int INVENTORY_ELEMENTS_PER_ROW = 8;

SwitchInventoryButton::SwitchInventoryButton(MainInventoryWidget* o,
                                             InventoryButtonAction b,
                                             int x, int y,
                                             int width, int height): BaseWidget(x, y, width, height) {
    owner = o;
    bid = b;
}

void SwitchInventoryButton::onclick() {
    owner->switch_view(bid);
}

void SwitchInventoryButton::render_elements() {
    // first, fill with black box
    set_block_draw_mode(2);
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
    else if (bid == InventoryButtonAction::TO_RECIPES) {
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
    
    glm::mat4 one(1);
    set_transform_matrix(one);
    
    glBindBuffer(GL_ARRAY_BUFFER, get_widget_vertex_attribute_vbo());
    glBufferData(GL_ARRAY_BUFFER, sizeof vertex, vertex, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(block_attribute_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, get_widget_texture_attribute_vbo());
    glBufferData(GL_ARRAY_BUFFER, sizeof texture, texture, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(texture_attribute_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

ScrollInventoryWidget::ScrollInventoryWidget(InventoryButtonAction t, PlayerInventory* inv, int rh, int x, int y, int width, int height): ScrollingWidget(rh, x, y, width, height) {
    inventory = inv;
    fetch = t;
    maxrows = 0;
    int total_num = 0;
    if (fetch == InventoryButtonAction::TO_BLOCKS) {
        total_num = inventory->get_block_count();
    }
    else if (fetch == InventoryButtonAction::TO_RECIPES) {
        total_num = inventory->get_recipe_count();
    }
    else if (fetch == InventoryButtonAction::TO_CUSTOM) {
        total_num = inventory->get_custom_count();
    }
    maxrows = int(ceil(total_num / INVENTORY_ELEMENTS_PER_ROW));
}

void ScrollInventoryWidget::set_row(int row, std::vector<BaseWidget*> row_widgets) {
    if (row_widgets.size() < INVENTORY_ELEMENTS_PER_ROW) {
        int perwidth = (width - (INVENTORY_ELEMENTS_PER_ROW - 1) * 2) / INVENTORY_ELEMENTS_PER_ROW;
        int pertotalwidth = perwidth + 2;
        int xoffset = (width - perwidth) / 2 + x;
        for (int i = 0; i < INVENTORY_ELEMENTS_PER_ROW; i++) {
            // y will be set later
            row_widgets.push_back(new ItemBarlet(xoffset + pertotalwidth * i, 0,
                                                 perwidth, rowheight - 2));
        }
    }
    
    if (fetch == InventoryButtonAction::TO_BLOCKS) {
        for (int i = 0; i < INVENTORY_ELEMENTS_PER_ROW; i++) {
            int index = row * INVENTORY_ELEMENTS_PER_ROW + i;
            ((ItemBarlet*)row_widgets[i])->set_cursor_item(inventory->get_block_at(index));
        }
    }
    else if (fetch == InventoryButtonAction::TO_RECIPES) {
        for (int i = 0; i < INVENTORY_ELEMENTS_PER_ROW; i++) {
            int index = row * INVENTORY_ELEMENTS_PER_ROW + i;
            ((ItemBarlet*)row_widgets[i])->set_cursor_item(inventory->get_recipe_at(index));
        }
    }
    else if (fetch == InventoryButtonAction::TO_CUSTOM) {
        for (int i = 0; i < INVENTORY_ELEMENTS_PER_ROW; i++) {
            int index = row * INVENTORY_ELEMENTS_PER_ROW + i;
            ((ItemBarlet*)row_widgets[i])->set_cursor_item(inventory->get_custom_at(index));
        }
    }
}

void ScrollInventoryWidget::switch_button_action(InventoryButtonAction to) {
    if (fetch == to)
        return;
    fetch = to;
    refresh();
}

MainInventoryWidget::MainInventoryWidget(PlayerInventory* inv,
                                         int width, int height): BaseWidget(width, height) {
    inventory = inv;
    current_view = InventoryButtonAction::SETUP;
    
    int bx1, bx2, bx3;
    int by;
    int bw, bh;
    
    bh = height / 10 - 4;
    by = height - (height / 10) + 2;
    bw = (width - 8) / 3;
    bx1 = x;
    bx2 = x + bw + 4;
    bx3 = x + bw * 2 + 8;
    buttons[0] = new SwitchInventoryButton(this, InventoryButtonAction::TO_BLOCKS, bx1, by, bw, bh);
    buttons[1] = new SwitchInventoryButton(this, InventoryButtonAction::TO_BLOCKS, bx2, by, bw, bh);
    buttons[2] = new SwitchInventoryButton(this, InventoryButtonAction::TO_BLOCKS, bx3, by, bw, bh);
    
    // scroll height
    int scrh = height - (height / 10);
    // row height
    int rh = scrh / 5;
    
    scrolling = new ScrollInventoryWidget(InventoryButtonAction::SETUP, inventory, rh, x, y, width, scrh);
    
}

void MainInventoryWidget::switch_view(InventoryButtonAction action) {
    if (current_view == action)
        return;
    // do stuff
    current_view = action;
    scrolling->switch_button_action(current_view);
}

void MainInventoryWidget::render_elements() {
    scrolling->render();
    for (int i = 0; i < INVENTORY_BUTTON_ITEMS; i++) {
        buttons[i]->render();
    }
}