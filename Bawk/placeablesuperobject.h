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
    fvec3 calculate_center_position(BlockOrientation pointing);
public:
    // for game templates
    PlaceableSuperObject();
    // for recipes
    PlaceableSuperObject(uint32_t s);
    // for custom cursorsuperobjects
    PlaceableSuperObject(uint32_t p, uint32_t s);
    
    // --- PlaceableSuperObject ---
    void move_template(ivec3 dir);
    void rotate_template();
    void render_blocks(fmat4* transform);
    
    // --- SuperObject ---
    void add_entity(Entity* entity) override;
    void remove_entity(Entity* entity) override;
    int get_chunk(block_type to_arr[CX][CY][CZ], int x, int y, int z) override;
    void handle_block_addition(int x, int y, int z, block_type type) override;
    void handle_block_removal(int x, int y, int z, block_type type) override;
    void update_chunks(fvec3* start_pos) override;
    
    // --- PlaceableObject ---
    bool set_blocks(Player* player, World* world, SuperObject* object) override;
};

#endif /* defined(__Bawk__placeablesuperobject__) */
