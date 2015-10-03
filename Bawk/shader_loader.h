//
//  Loads shaders from files and then binds them to OpenGL
//  -
//  -
//  -
//
//  Used by:
//  - worldrender.cpp
//
//  Uses:
//  - OpenGL
//  - File IO libraries
//  -
//
//  Notes
//  - File reading shamelessly copied from http://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
//  - Binding to OpenGL shamelessly copied from https://www.opengl.org/wiki/OpenGL_Shading_Language#Building_shaders
//  - The set_shaders function is sensitive to the contents of the .glsl shader fields
//    due to variable names (attributes)!
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__shader_loader__
#define __Bawk__shader_loader__

#include <stdio.h>
#include <GLFW/glfw3.h>

int set_shaders(GLuint* block_attribute_coord,
                GLuint* texture_attribute_coord,
                GLuint* block_uniform_mvp,
                GLuint* program);

#endif /* defined(__Bawk__shader_loader__) */
