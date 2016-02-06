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
#include "item_bar.h"

const int INVENTORY_BUTTON_ITEMS = 3;

enum InventoryButtonAction {
    SETUP, TO_BLOCKS, TO_MODELS, TO_CUSTOM,
};

// forward declaration
class MainInventoryWidget;

class SwitchInventoryButton: public BaseWidget {
    MainInventoryWidget* owner;
    InventoryButtonAction bid;
public:
    SwitchInventoryButton(MainInventoryWidget* o, InventoryButtonAction b,
                          int x, int y, int width, int height);
    bool onclick(int mx, int my, int button) override;
    void render_elements() override;
};

class ScrollInventoryWidget: public ScrollingWidget {
    PlayerInventory* inventory;
    InventoryButtonAction fetch;
    ItemBar* itembar;
    int total_count;
public:
    ScrollInventoryWidget(ItemBar* ib, InventoryButtonAction t, PlayerInventory* inv,
                          int rw, int rh, int mr,
                          int x, int y, int width, int height);
    ~ScrollInventoryWidget();
    BaseWidget* set_row(int index, BaseWidget* current) override;
    void clear_child(BaseWidget* child) override;
    void switch_button_action(InventoryButtonAction to);
    bool onclick(BaseWidget* clicked_child, int mx, int my, int button) override;
    void refresh() override;
};

class MainInventoryWidget: public ParentWidget {
    InventoryButtonAction current_view;
public:
    MainInventoryWidget(ItemBar* ib, PlayerInventory* inv, int width, int height);
    ~MainInventoryWidget();
    void refresh();
    void switch_view(InventoryButtonAction action);
};

#endif /* defined(__Bawk__inventory_widget__) */
