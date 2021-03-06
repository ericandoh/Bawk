//
//  cursormodelobject.h
//  Bawk
//
//  Created by Eric Oh on 11/29/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__cursormodelobject__
#define __Bawk__cursormodelobject__

#include <stdio.h>
#include "placeableobject.h"
#include "cursoritem.h"
#include "modelentityrender.h"

class CursorModelObject: public PlaceableObject, public CursorItem, public ModelEntity {
    bool locked;
public:
    CursorModelObject(CursorItemInfo* i);
    
    // --- PlaceableObject
    bool set_blocks(Player* player, SuperObject* object) override;
    
    // --- CursorItem ---
    void reset() override;
    bool clicked(Action mouse) override;
    bool confirmed() override;
    bool canceled() override;
    bool handle_movement(ivec3 dir) override;
    bool handle_rotation() override;
    void step(int ms) override;
    void render_item() override;
    void render_in_world(fmat4* transform) override;
    void render_light_in_world(fmat4* transform, fvec3 player_pos) override;
    
    bool has_count() override;
    
    // --- ModelEntity ---
    std::string get_save_path() override;
};

#endif /* defined(__Bawk__cursormodelobject__) */
