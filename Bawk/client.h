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

// input/output
#include "input_multiplexer.h"
#include "mount_input_receiver.h"

// sprites
#include "spritemanager.h"
#include "player.h"

class CursorItem;
class GameWindowHolder;
class GameInputReceiver;

class Client: public Displayable {
    
    // used for updating player pos/chunk loading
    fvec3 last_player_pos;
    
    InputMultiplexer client_receiver;
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
    
    MountInputReceiver* mount_receiver;
    
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
    
    bool key_callback(Action do_this, int ms) override;
    bool mouse_move_callback(double xdiff, double ydiff) override;
    bool mouse_clicked_callback(Action do_this) override;
    bool mouse_clicking_callback(Action do_this, int ms) override;
    bool scroll_callback(double xdiff, double ydiff) override;
};

#endif /* defined(__Bawk__client__) */
