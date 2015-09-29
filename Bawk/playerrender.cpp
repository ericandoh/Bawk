//
//  playerrender.cpp
//  Bawk
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "playerrender.h"
#include <GLFW/glfw3.h>

PlayerRender::PlayerRender() {
    // initialize things
}

void PlayerRender::set_camera(Position* pos, Direction* dir) {
    glMatrixMode(GL_MODELVIEW);                  // indicate we are specifying camera transformations
    glLoadIdentity();                            // make sure transformation is "zero'd"
    
    glRotatef((float) glfwGetTime() * 50.f, 0.f, 3.f, 1.f);
}