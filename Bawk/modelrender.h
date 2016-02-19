//
//  modelrender.h
//  Bawk
//
//  Created by Eric Oh on 11/29/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__modelrender__
#define __Bawk__modelrender__

#include <stdio.h>
#include "modeldata.h"
#include <string>
#include <map>
#include "basic_types.h"
#include "lightrender.h"
#include "modelaction.h"

class RenderableModel: public ModelData {
public:
    std::string name;
    int resistance;
    int weight;
    int vehicle;
    RenderableLight light;
    ModelActionMultiplexer* multiplexer;
    
    bounding_box bounds;
    fvec3 center_pos;
    
    RenderableModel();
    
    // --- RenderableModel ---
    void refresh();
};

#endif /* defined(__Bawk__modelrender__) */
