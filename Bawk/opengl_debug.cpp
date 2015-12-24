//
//  opengl_debug.cpp
//  Bawk
//
//  Created by Eric Oh on 12/22/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "opengl_debug.h"
#include "includeglfw.h"

void check_for_error() {
    int error = glGetError();
    if (error != 0) {
        printf("Error in OPENGL: %x\n",error);
    }
}