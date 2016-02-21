//
//  client.h
//  Bawk
//
//  Created by Eric Oh on 2/16/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__client__
#define __Bawk__client__

#include <stdio.h>
#include "displayable.h"

// UI imports
#include "item_bar.h"
#include "parent_widget.h"
#include "inventory_widget.h"

// input/output
#include "input_multiplexer.h"
#include "mount_input_receiver.h"

// sprites
#include "spritemanager.h"
#include "player.h"

class CursorItem;

class Client: public Displayable {
    
    // current player playing this game!
    uint32_t pid;
    // used for updating player pos/chunk loading
    fvec3 last_player_pos;
    
    
    InputMultiplexer client_receiver;
    
    bool ui_open;

protected:
    Player* player;
    void render_geometry();
    void render_ui();
    void check_need_update();
public:
    
    // sprites
    SpriteManager sprite_manager;
    
    // UI widgets
    ItemBar* bar;
    MainInventoryWidget* inventory_ui;
    ParentWidget* story;
    
    CursorItem* default_item;
    
    MountInputReceiver* mount_receiver;
    
    // --- Client ---
    virtual int init(uint32_t p);
    virtual void cleanup();
    
    Player* get_player();
    void set_first_available(CursorItem* obj);
    void toggle_mount_ui(bool mounted);
    
    virtual Player* register_player(uint32_t pid) = 0;       // get the player, tells the engine to add it to world if not present
    virtual void deregister_player(uint32_t pid) = 0;
    virtual void update_render(fvec3 player_pos) = 0;
    virtual World* get_world() = 0;
    virtual void toggle_action(Action do_this, int ms) = 0;
    virtual void toggle_player_movement(double x, double y) = 0;
    
    // --- Displayable ---
    void render() override;
    void render_lights() override;
    void render_shadows() override;
    void frame(int ms) override;
    void key_callback(int key, int scancode, int action, int mods);
    void mouse_move_callback(double xdiff, double ydiff);
    void mouse_button_callback(int button, int action, int mods);
    void scroll_callback(double xoffset, double yoffset);
};

#endif /* defined(__Bawk__client__) */
