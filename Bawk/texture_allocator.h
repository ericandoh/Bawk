//
//  texture_allocator.h
//  Bawk
//
//  reserving offsets for glActiveTexture(...)
//
//  Created by Eric Oh on 12/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__texture_allocator__
#define __Bawk__texture_allocator__

#include <stdio.h>

int reserve_n_active_textures(int n);
void set_active_texture(int n);

#endif /* defined(__Bawk__texture_allocator__) */
