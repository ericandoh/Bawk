//
//  collision_detection.h
//  Bawk
//
//  Created by Eric Oh on 11/30/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__collision_detection__
#define __Bawk__collision_detection__

#include <stdio.h>

class Entity;

// IF I had cleaner collision detection code this is where that would go
// IF <=== big emphasis on IF

bool has_collision_between_entities(Entity* a, Entity* b);

#endif /* defined(__Bawk__collision_detection__) */
