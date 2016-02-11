//
//  modeldata.h
//  Bawk
//
//  Created by Eric Oh on 2/10/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__modeldata__
#define __Bawk__modeldata__

#include <stdio.h>
#include <vector>
#include "basic_types.h"

class ModelData {
public:
    int texture;
    std::vector<fvec3> model_vertices;
    std::vector<fvec3> model_normals;
    std::vector<fvec3> model_uvs;
    
    void render();
};



#endif /* defined(__Bawk__modeldata__) */
