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
    CursorModelObject(uint16_t mid);
    
    // --- PlaceableObject
    bool set_blocks(Player* player, World* world, SuperObject* object) override;
    
    // --- CursorItem ---
    void reset() override;
    bool clicked(Game* game, Action mouse, Entity* on) override;
    bool confirmed(Game* game) override;
    bool canceled(Game* game) override;
    bool handle_movement(ivec3 dir) override;
    bool handle_rotation() override;
    void render_item() override;
    void render_in_world(fmat4* transform) override;
    
    cursor_item_identifier get_identifier() override;
    
    // --- ModelEntity ---
    std::string get_save_path() override;
};

#endif /* defined(__Bawk__cursormodelobject__) */
