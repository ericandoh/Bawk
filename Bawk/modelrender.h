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
#include <vector>
#include <string>
#include "basic_types.h"

class RenderableModel {
public:
    std::string name;
    int resistance;
    int weight;
    int vehicle;
    
    bounding_box bounds;
    fvec3 center_pos;
    std::vector<fvec3> model_vertices;
    std::vector<fvec3> model_normals;
    std::vector<fvec3> model_uvs;
    
    RenderableModel();
    
    // --- RenderableModel ---
    void render();
    void refresh();
};

#endif /* defined(__Bawk__modelrender__) */
