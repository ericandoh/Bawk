//
//  The base widget class. A widget is some sort of flat entity on the screen, and:
//  - Knows where it is positioned on the screen
//  - Knows how to render itself
//  -
//
//  Used by:
//  - game.cpp, display.cpp
//
//  Uses:
//  -
//  -
//  -
//
//  Notes
//  - use glViewport(...) a lot
//  - for now, we aren't a fan of window resizing, and assume we're given a size at the start
//  - later we can link it up and dynamically resize, but that's a future thing to do
//    (bc its complicated and we start getting into layout stuff = yuk)
//
//
//  Created by Eric Oh on 10/10/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__base_widget__
#define __Bawk__base_widget__

#include <stdio.h>
#include <GLFW/glfw3.h>
#include "basic_types.h"

void initialize_vbo_for_widgets();
GLuint get_widget_vertex_attribute_vbo();
GLuint get_widget_texture_attribute_vbo();
void clean_vbo_for_widgets();

class BaseWidget {
public:
    int x, y, width, height;
    BaseWidget();
    // makes a widget of the following size
    BaseWidget(int x, int y, int width, int height);
    // makes the widget below, but keeps it centered
    BaseWidget(int width, int height);
    void set_dimensions(int x, int y, int width, int height);
    // renders the widget, making sure to set the viewport
    void render();
    // sees if a press at mx, my clicked this widget
    bool is_clicked(int mx, int my);
    // called when this is scrolled
    virtual bool scrolled(int mx, int my, int px);
    // this should be overriden
    virtual void render_elements() = 0;
    // action to do when clicked
    virtual bool onclick(int mx, int my, int button);
};

#endif /* defined(__Bawk__base_widget__) */
