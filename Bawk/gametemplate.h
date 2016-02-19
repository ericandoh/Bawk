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
#include "placeablesuperobject.h"

class CursorSuperObject;

// represents a template in the making
class GameTemplate: public PlaceableSuperObject {
    Rotation get_vehicle_orientation();
public:
    GameTemplate();
    
    // --- SuperObject ---
    // void render(fmat4* transform) override;
    RenderableChunk* get_chunk(int x, int y, int z) override;
    
    // --- GameTemplate ---
    CursorSuperObject* create_from_template();
};

#endif /* defined(__Bawk__gametemplate__) */
