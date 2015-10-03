//
//  Superobject.h
//  Bawk
//
//  Created by Eric Oh on 10/1/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__Superobject__
#define __Bawk__Superobject__

#include <stdio.h>
#include "superobjectrender.h"

class SuperObject : public RenderableSuperObject {
public:
    // fills superobject with some data. Used only for initial test, might replace later
    void fill();
};

#endif /* defined(__Bawk__Superobject__) */
