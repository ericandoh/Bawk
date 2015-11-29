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

class ModelEntity: public Entity {
    bool in_range;
    RenderableModel* model;
    uint16_t model_id;
public:
    ModelEntity(uint32_t p, uint32_t v);
    ModelEntity(uint32_t p, uint32_t v, uint16_t model_id);
    
    // --- ModelEntity ---
    void set_model(uint16_t m);
    
    // --- Entity ---
    // Entity* poke(float x, float y, float z) override;
    void render(fmat4* transform) override;
    void update_chunks(fvec3* player_pos) override;
    // void calculate_moving_bounding_box() override;
    
    // bool collides_with(Entity* other) override;
    // int get_collision_level() override;
    // bool collides_with(Entity* other, bounding_box* my_bounds, bounding_box* other_bounds, int my_collision_lvl, int other_collision_level);
    
    std::string get_save_path() override;
    int load_self(IODataObject* obj) override;
    void remove_self(IODataObject* obj) override;
};

#endif /* defined(__Bawk__modelentityrender__) */
