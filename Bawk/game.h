//
//  Represents the game
//  - Main function is to handle inputs from the game and pass them to the right entities
//  - Holds the World (which has the BaseWorld and other objects)
//  - Holds a reference to the current Player playing this game
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
#include <GLFW/glfw3.h>
#include <vector>
#include "world.h"
#include "player.h"
#include "displayable.h"
#include "temporarytemplate.h"
#include "cursoritem.h"

class Game : public Displayable {
    // game data objects
    World* world;
    Player* player;
    
    // used for updating player pos/chunk loading
    fvec3 last_player_pos;
    
    // block placement related objects
    
    // template holding current template we're working on (if any)
    TemporaryTemplate* place_into;
    
    // the item on the cursor right now. Clicking will place this item down
    CursorItem* current_item;
    // all our cursor items
    std::vector<CursorItem*> cursor_items;
    // the index of the current cursor item
    int cursor_item_index;
    // if we've just placed down a cursor item that is a template, and are now in template move mode
    bool placed_current_item;
    
    void check_need_update();
public:
    int init() override;
    void render() override;
    void frame() override;
    void switch_current_item(int to_index);
    void key_callback(int key, int scancode, int action, int mods) override;
    void mouse_move_callback(double xdiff, double ydiff) override;
    void mouse_button_callback(int button, int action, int mods) override;
    ~Game();
};

#endif /* defined(__Bawk__game__) */