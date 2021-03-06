//
//  Represents all world data
//  - Holds information about (all) players, blocks, chunks in this world
//
//  Used by:
//  - game.cpp (main game)
//
//  Uses:
//  -
//  -
//  -
//
//  Notes
//  -
//  -
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__world__
#define __Bawk__world__

#include <stdio.h>
#include <vector>
#include <string>
#include "basic_types.h"
#include "worldrender.h"
#include "worldevent.h"
#include "base_world.h"
#include "worldweather.h"

class Player;

class World {
    std::vector<WorldEvent*> events;
    WorldWeather weather;
public:
    std::string name;
    // how many cycles the world has lived through
    unsigned long age;
    unsigned long id_assign;
    BaseWorld* base_world;
    World(std::string id);
    
    uint32_t assignID();
    
    int load_self();
    void save_self();
    void render(fmat4* transform);
    void render_lights(fmat4* transform, fvec3 player_pos);
    void render_background(fmat4* transform);
    void set_light_camera(Player* player);
    void set_light_camera_for_lighting(Player* player);
    void update_render(fvec3* player_pos);
    
    void get_entity_at(float x, float y, float z, Entity** selected);
    
    void add_player(Player* player);
    void add_entity(Entity* entity);
    void add_event(WorldEvent* event);
    
    void step(int ms);
    
    bool will_collide_with_anything(Entity* superobject);
    SuperObject* create_superobject(Player* player);
    void remove_entity(Entity* entity);
};

#endif /* defined(__Bawk__world__) */
