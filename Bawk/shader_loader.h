//
//  Loads shaders from files and then binds them to OpenGL
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
//  - File reading shamelessly copied from http://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
//  - Binding to OpenGL shamelessly copied from https://www.opengl.org/wiki/OpenGL_Shading_Language#Building_shaders
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__shader_loader__
#define __Bawk__shader_loader__

#include <stdio.h>
#include <GLFW/glfw3.h>

GLuint set_shaders();

#endif /* defined(__Bawk__shader_loader__) */
