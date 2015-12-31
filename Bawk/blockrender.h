//
//  Modifies block data for rendering
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
//
//  Created by Eric Oh on 10/16/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__blockrender__
#define __Bawk__blockrender__

#include <stdio.h>
#include "importopengl.h"
#include "block.h"
#include "block_orientation.h"

void set_coord_and_texture(GLbyte coord[][3],
                           GLbyte texture[][3],
                           int index,
                           int x, int y, int z,
                           block_type block, BlockOrientation face);

#endif /* defined(__Bawk__blockrender__) */
