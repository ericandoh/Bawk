//
//  A collection of blocks that can be quick selected and placed all at once
//  -
//  -
//  -
//
//  Used by:
//  -
//
//  Uses:
//  -
//  -
//  -
//
//  Notes
//  -
//  -
//  Created by Eric Oh on 10/6/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__cursorsuperobject__
#define __Bawk__cursorsuperobject__

#include <stdio.h>
#include <map>
#include "cursoritem.h"
#include "placeablesuperobject.h"

// Represents a template that can be put down

class CursorSuperObject: public PlaceableSuperObject, public CursorItem {
    std::map<uint16_t, int> block_resource_req;
    std::map<uint16_t, int> model_resource_req;
    bool locked;
    
    bool has_sufficient_resources(Player* player);
    void spend_resources(Player* player);
    
public:
    CursorSuperObject(CursorItemInfo* i);
    
    // --- CursorSuperObject ---
    
    // --- CursorItem ---
    void init() override;
    void init(std::string path);
    void reset() override;
    bool clicked(Game* game, Action mouse) override;
    bool confirmed(Game* game) override;
    bool canceled(Game* game) override;
    bool handle_movement(ivec3 dir) override;
    bool handle_rotation() override;
    void step(Game* game, int ms) override;
    void render_item() override;
    void render_in_world(fmat4* transform) override;
    void render_light_in_world(fmat4* transform, fvec3 player_pos) override;
    
    bool has_count() override;
    
    // --- PlaceableSuperObject ---
    
    // --- SuperObject ---
    void add_entity(Entity* entity) override;
    void remove_entity(Entity* entity) override;
    
    void handle_block_addition(int x, int y, int z, block_type type) override;
    void handle_block_removal(int x, int y, int z, block_type type) override;
    
    std::string get_save_path() override;
    std::string get_chunk_save_path(ivec3* pos) override;
    int load_self(IODataObject* obj) override;
    void save_self(IODataObject* obj) override;
};

#endif /* defined(__Bawk__cursorsuperobject__) */
