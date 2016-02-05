//
//  basicrender.h
//  Bawk
//
//  Created by Eric Oh on 2/4/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__basicrender__
#define __Bawk__basicrender__

#include <stdio.h>
#include "basic_types.h"

#define COLOR_RED fvec3(1,0,0)
#define COLOR_GREEN fvec3(0,1,0)
#define COLOR_BLUE fvec3(0,0,1)

void draw_colored_cube(fmat4* transform, bounding_box* box, fvec3 color, bool transparent);
void draw_colored_wire_cube(fmat4* transform, bounding_box* box, fvec3 color, bool transparent);

#endif /* defined(__Bawk__basicrender__) */
