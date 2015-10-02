//
//  playerrender.cpp
//  Bawk
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "display.h"    // for get_window_size
#include "playerrender.h"

PlayerRender::PlayerRender() {
    // initialize things
}

fmat4* PlayerRender::set_camera(fvec3* pos, fvec3* forward, fvec3* up, fvec3* dir) {
    
    int width, height;
    get_window_size(&width, &height);
    
    view = glm::lookAt(*pos, *pos + *dir, *up);
    projection = glm::perspective(45.0f, 1.0f*width/height, 0.01f, 1000.0f);
    mvp = projection * view;
    return &mvp;
}