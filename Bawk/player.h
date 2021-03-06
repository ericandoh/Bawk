//
//  Class holding information about a player
//  - Holds location + orientation
//  -
//  -
//
//  Used by:
//  - world.cpp
//  - game.cpp
//
//  Uses:
//  - math.h
//  -
//  -
//
//  Notes
//  - Later separate into a Player object (for other players) and MyPlayer (for player playing the game)
//  -
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__player__
#define __Bawk__player__

#include <stdio.h>
#include "math.h"
#include "playerrender.h"
#include "basic_types.h"
#include "inventory.h"

class SuperObject;
class GameTemplate;

class Player : public RenderablePlayer {
    unsigned long id_assign;
    SuperObject* mount;
    fvec3 offset_to_mount;
public:
    // template holding current template we're working on (if any)
    GameTemplate* game_template;
    PlayerInventory* inventory;
    Player(uint32_t p);
    ~Player();
    
    // --- Player ---
    uint32_t getID();
    uint32_t assignID();
    void update_direction(double xdiff, double ydiff);
    ivec3 get_rounded_left();
    ivec3 get_rounded_forward();
    
    void set_mount(SuperObject* m, fvec3 rwc);
    SuperObject* get_mount();
    bool unmount();
    
    // --- RenderablePlayer
    bounding_box get_bounds_for_viewing() override;
    
    // --- Entity ---
    void step(int ms) override;
    std::string get_save_path() override;
    int load_selfs() override;
    int load_self(IODataObject* obj) override;
    void save_self(IODataObject* obj) override;
    
    void debug();
};

#endif /* defined(__Bawk__player__) */