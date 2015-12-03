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
class World;

class Player : public RenderablePlayer {
    unsigned long id_assign;
    SuperObject* mount;
    fvec3 offset_to_mount;
public:
    PlayerInventory* inventory;
    Player(uint32_t p);
    ~Player();
    
    // --- Player ---
    uint32_t getID();
    uint32_t assignID();
    void update_direction(double xdiff, double ydiff);
    ivec3 get_rounded_left();
    ivec3 get_rounded_forward();
    
    void set_mount(SuperObject* m, fvec3 pos);
    SuperObject* get_mount();
    bool unmount(World* world);
    
    // --- Entity ---
    void step() override;
    std::string get_save_path() override;
    int load_selfs() override;
    int load_self(IODataObject* obj) override;
    void remove_self(IODataObject* obj) override;
    
    void debug();
};

#endif /* defined(__Bawk__player__) */