//
//  placeablesuperobject.h
//  Bawk
//
//  Created by Eric Oh on 11/11/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__placeablesuperobject__
#define __Bawk__placeablesuperobject__

#include <stdio.h>
#include "placeableobject.h"
#include "superobject.h"

class PlaceableSuperObject: public SuperObject, public PlaceableObject {
protected:
    int makes_vehicle;
    fvec3 calculate_center_position();
public:
    // for game templates
    PlaceableSuperObject();
    // for recipes
    PlaceableSuperObject(uint32_t s);
    // for custom cursorsuperobjects
    PlaceableSuperObject(uint32_t p, uint32_t s);
    
    // --- SuperObject ---
    void set_block(float x, float y, float z, block_type type) override;
    void update_chunks(fvec3* old_pos, fvec3* new_pos) override;
    
    // --- PlaceableObject ---
    bool set_blocks(Game* game) override;
    bool set_blocks(Player* player, World* world, SuperObject* object) override;
    
    // --- PlaceableSuperObject ---
    void move_template(ivec3 dir);
    void rotate_template();
    void render_blocks(fmat4* transform);
};

#endif /* defined(__Bawk__placeablesuperobject__) */
