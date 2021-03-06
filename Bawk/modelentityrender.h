//
//  modelentityrender.h
//  Bawk
//
//  Created by Eric Oh on 11/29/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__modelentityrender__
#define __Bawk__modelentityrender__

#include <stdio.h>
#include "entity.h"
#include "modelrender.h"
#include "modelaction.h"

class ModelEntity: public Entity {
    bool in_range;
public:
    uint16_t model_id;
    RenderableModel* model;
    ModelActionMultiplexer* multiplexer;
    ModelEntity(uint16_t mid);
    ModelEntity(uint32_t p, uint32_t v);
    ModelEntity(uint32_t p, uint32_t v, uint16_t mid);
    
    // --- ModelEntity ---
    void set_model(uint16_t m);
    
    // --- Entity ---
    // Entity* poke(float x, float y, float z) override;
    void drop_loot(Player* slayer) override;
    bool block_keyboard_callback(Player* player, Action key, Entity* ent, int ms) override;
    bool block_mouse_callback(Player* player, Action button, Entity* ent) override;
    void step(int ms) override;
    void render(fmat4* transform) override;
    void render_lights(fmat4* transform, fvec3 player_pos) override;
    void update_render(fvec3* player_pos) override;
    // void calculate_moving_bounding_box() override;
    
    // bool collides_with(Entity* other) override;
    // int get_collision_level() override;
    // bool collides_with(Entity* other, bounding_box* my_bounds, bounding_box* other_bounds, int my_collision_lvl, int other_collision_level);
    bool after_collision() override;
    
    virtual std::string get_save_path() override;
    int load_self(IODataObject* obj) override;
    void save_self(IODataObject* obj) override;
};

#endif /* defined(__Bawk__modelentityrender__) */
