//
//  A holding class used to save data about what blocks have been placed while in create template mode
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

#ifndef __Bawk__gametemplate__
#define __Bawk__gametemplate__

#include <stdio.h>
#include "basic_types.h"
#include "world.h"
#include "superobjectrender.h"


#include "placeablesuperobject.h"


class CursorSuperObject;

// represents a template in the making
class GameTemplate: public PlaceableSuperObject {
public:
    GameTemplate();
    
    // --- SuperObject ---
    int get_chunk(block_type to_arr[CX][CY][CZ], int x, int y, int z) override;
    int save_chunk(block_type from_arr[CX][CY][CZ], int x, int y, int z) override;
    
    // --- GameTemplate ---
    CursorSuperObject* create_from_template(Player* player, World* world, TemporaryTemplate* temp);
    void publish(Player* player, World* world);
    void unpublish(World* world);
    
    
    void render(fmat4* transform) override;
    
    
};

#endif /* defined(__Bawk__gametemplate__) */
