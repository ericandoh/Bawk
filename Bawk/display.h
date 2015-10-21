//
//  The viewport and window of the program
//  - Main render loop here
//  - Displays some displayable
//
//  Used by:
//  - program.cpp
//
//  Uses:
//  - OpenGL
//  - displayable.h
//  -
//
//  Notes
//  - This interacts with program.cpp so be careful!
//  -
//
//  Created by Eric Oh on 9/29/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__display__
#define __Bawk__display__

#include <stdio.h>
#include <GLFW/glfw3.h>
#include "displayable.h"

void get_window_size(int* width, int* height);
void display_enable_cursor();
void display_disable_cursor();
void display_get_cursor_position(double* x, double* y);

int init_display();
void set_current_game(Displayable* display);
void display_close();
void close_render_loop();

int display_run();

#endif /* defined(__Bawk__display__) */