//
//  engine_accessor.h
//  Bawk
//
//  Created by Eric Oh on 2/18/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__engine_accessor__
#define __Bawk__engine_accessor__

#include <stdio.h>
#include "engine.h"

class World;

void set_engine(Engine* engine);
Engine* get_engine();
World* get_engine_world();

#endif /* defined(__Bawk__engine_accessor__) */
