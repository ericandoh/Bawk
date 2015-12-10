//
//  modeldata.h
//  Bawk
//
//  Created by Eric Oh on 12/9/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__modeldata__
#define __Bawk__modeldata__

#include <stdio.h>

// data object to hold extra info about a model
// this may be stored in either ModelRender (shared between all similar looking models)
// or in RenderableModel (only used by that one instance of that model)
class ModelData {
public:
    virtual void step();
};

// various types of ModelData exposed for implementation
class EngineModelData {
    // TODO expand to include others
    float power;
};

// to implement multiple combinations of model data, simply do multiple inheritance
/*class EngineMountableData: public EngineModelData, public ModelData {
};*/

#endif /* defined(__Bawk__modeldata__) */
