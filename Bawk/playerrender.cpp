//
//  playerrender.cpp
//  Bawk
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "display.h"
#include "playerrender.h"

PlayerRender::PlayerRender() {
    // initialize things
}

void PlayerRender::set_camera(glm::vec3* pos, glm::vec3* forward, glm::vec3* up, glm::vec3* dir) {
    
    int width, height;
    get_window_size(&width, &height);
    
    glm::mat4 view = glm::lookAt(*pos, *pos + *dir, *up);
    glm::mat4 projection = glm::perspective(45.0f, 1.0f*width/height, 0.01f, 1000.0f);
    
    glm::mat4 mvp = projection * view;
    
    glUniformMatrix4fv(block_uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
}