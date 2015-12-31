//
//  Represents the game
//  - Main function is to handle inputs from the game and pass them to the right entities
//  - Holds the World (which has the BaseWorld and other objects)
//  - Holds a reference to the current Player playing this game
//  - Makes calls to lads resources and free them
//
//  Used by:
//  - display.cpp (displays this)
//  - program.cpp (sets this to be displayed)
//
//  Uses:
//  - world.h, player.h
//
//  Notes
//  - Move this into a Screen subclass then run another global instance on top of this
//  - That will allow us to have loading screen/menu/etc
//  - If we do multiplayer later, this will have to be split into a client-side and server-side
//    handling (and we would override world with a dummy reference that would query)
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__game__
#define __Bawk__game__

#include <stdio.h>
#include "importopengl.h"
#include <vector>
#include <string>
#include "world.h"
#include "player.h"
#include "displayable.h"
#include "cursoritem.h"
#include "item_bar.h"
#include "parent_widget.h"
#include "inventory_widget.h"

const int VERSION = 4;

class GameTemplate;

class Game : public Displayable {
    std::string world_name;
    uint32_t pid;
    
    // used for updating player pos/chunk loading
    fvec3 last_player_pos;
    
    // block placement related objects
    
    ItemBar* bar;
    MainInventoryWidget* inventory_ui;
    ParentWidget* story;
    
    bool in_game;
    
    void render_geometry();
    void render_ui();
    void check_need_update();
    int load_game_data();
    int save_game_data();
public:
    // game data objects
    World* world;
    Player* player;
    // template holding current template we're working on (if any)
    GameTemplate* game_template;
    void set_parameters(std::string wn, uint32_t p);
    int init() override;
    // TODO refactor the rendering pipeline into its own class
    void render() override;
    void render_shadows() override;
    void render_lights() override;
    void frame() override;
    void switch_current_item(int to_index);
    
    void key_callback_default(int key);
    void mouse_callback_default(Action key, Entity* on);
    
    void key_callback(int key, int scancode, int action, int mods) override;
    void mouse_move_callback(double xdiff, double ydiff) override;
    void mouse_button_callback(int button, int action, int mods) override;
    void scroll_callback(double xoffset, double yoffset) override;
    ~Game();
};

#endif /* defined(__Bawk__game__) */