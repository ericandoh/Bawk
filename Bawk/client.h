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

// UI imports
#include "item_bar.h"
#include "parent_widget.h"

// input/output
#include "input_multiplexer.h"
#include "mount_input_receiver.h"

// sprites
#include "spritemanager.h"
#include "player.h"

class CursorItem;
class GameWindowHolder;
class GameInputReceiver;
class CursorItemHandler;

class Client: public InputMultiplexer {
    
    // used for updating player pos/chunk loading
    fvec3 last_player_pos;
    GameInputReceiver* game_receiver;

protected:
    void render_geometry();
    void render_ui();
    void check_need_update();
public:
    // game windows/UI
    GameWindowHolder* game_window_holder;
    
    // sprites
    SpriteManager sprite_manager;
    
    // mount input receiver
    MountInputReceiver* mount_receiver;
    
    // cursor item receiver
    CursorItemHandler* cursor_handler;
    
    // --- Client ---
    void toggle_mount_ui(bool mounted);
    void update_render(fvec3 player_pos);
    
    void toggle_action(Action do_this, int ms);
    void toggle_player_movement(double x, double y);
    
    // --- DisplayElement ---
    void init() override;
    void cleanup() override;
    
    void render() override;
    void render_lights() override;
    void render_shadows() override;
    void frame(int ms) override;
};

#endif /* defined(__Bawk__client__) */
