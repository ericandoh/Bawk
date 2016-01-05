//
//  uirenderhelper.h
//  Bawk
//
//  Created by Eric Oh on 1/5/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__uirenderhelper__
#define __Bawk__uirenderhelper__

#include <stdio.h>
#include "basic_types.h"

namespace UIHelper {
    void draw_square_color(fvec3 color);
    void draw_square_voxel(int voxel);
    void draw_square_texture(fvec3 uv_coords[4]);
}

#endif /* defined(__Bawk__uirenderhelper__) */
