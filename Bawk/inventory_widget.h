//
//  inventory_widget.h
//  Bawk
//
//  Created by Eric Oh on 10/16/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__inventory_widget__
#define __Bawk__inventory_widget__

#include <stdio.h>
#include <vector>
#include "base_widget.h"
#include "inventory.h"
#include "item_barlet.h"
#include "scrolling_widget.h"

const int INVENTORY_BUTTON_ITEMS = 3;

enum InventoryButtonAction {
    SETUP, TO_BLOCKS, TO_RECIPES, TO_CUSTOM,
};

// forward declaration
class MainInventoryWidget;

class SwitchInventoryButton: public BaseWidget {
    MainInventoryWidget* owner;
    InventoryButtonAction bid;
public:
    SwitchInventoryButton(MainInventoryWidget* o, InventoryButtonAction b,
                          int x, int y, int width, int height);
    void onclick() override;
    void render_elements() override;
};

class ScrollInventoryWidget: public ScrollingWidget {
    PlayerInventory* inventory;
    InventoryButtonAction fetch;
public:
    ScrollInventoryWidget(InventoryButtonAction t, PlayerInventory* inv, int rh, int x, int y, int width, int height);
    void set_row(int row, std::vector<BaseWidget*> row_widgets) override;
    void switch_button_action(InventoryButtonAction to);
};

class MainInventoryWidget: public BaseWidget {
    PlayerInventory* inventory;
    SwitchInventoryButton* buttons[INVENTORY_BUTTON_ITEMS];
    ScrollInventoryWidget* scrolling;
    
    InventoryButtonAction current_view;
public:
    MainInventoryWidget(PlayerInventory* inv, int width, int height);
    void switch_view(InventoryButtonAction action);
    void render_elements() override;
};

#endif /* defined(__Bawk__inventory_widget__) */
