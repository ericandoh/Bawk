//
//  display.h
//  Bawk
//
//  Created by Eric Oh on 9/29/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__display__
#define __Bawk__display__

#include <stdio.h>
#include <GLFW/glfw3.h>

extern GLuint block_attribute_coord;
extern GLuint block_uniform_mvp;
extern GLuint program;

void get_window_size(int* width, int* height);

int run();

#endif /* defined(__Bawk__display__) */