//
//  base_widget.cpp
//  Bawk
//
//  Created by Eric Oh on 10/10/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "base_widget.h"
#include "display.h"

GLuint widget_vertex_vbo;
GLuint widget_texture_vbo;

void initialize_vbo_for_widgets() {
    glGenBuffers(1, &widget_vertex_vbo);
    glGenBuffers(1, &widget_texture_vbo);
}

GLuint get_widget_vertex_attribute_vbo() {
    return widget_vertex_vbo;
}

GLuint get_widget_texture_attribute_vbo() {
    return widget_texture_vbo;
}

void clean_vbo_for_widgets() {
    glDeleteBuffers(1, &widget_vertex_vbo);
    glDeleteBuffers(1, &widget_texture_vbo);
}

BaseWidget::BaseWidget() {
    x = 0;
    y = 0;
    width = 1;
    height = 1;
    printf("Making widget at (%d,%d) by (%d, %d)\n", x, y, width, height);
}

BaseWidget::BaseWidget(int x, int y, int width, int height) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    printf("Making widget at (%d,%d) by (%d, %d)\n", x, y, width, height);
}

BaseWidget::BaseWidget(int width, int height) {
    int swidth, sheight;
    get_window_size(&swidth, &sheight);
    
    int xgap = swidth - width;
    int ygap = sheight - height;
    
    this->x = xgap / 2;
    this->y = ygap / 2;
    this->width = width;
    this->height = height;
    printf("Making widget at (%d,%d) by (%d, %d)\n", x, y, width, height);
}

void BaseWidget::set_dimensions(int x, int y, int width, int height) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
}

void BaseWidget::render() {
    glViewport(x, y, width, height);
    render_elements();
}

bool BaseWidget::is_clicked(int mx, int my) {
    if (mx >= x && my >= y && x <= (x + width) && y <= (y + height)) {
        printf("Click triggered at %d %d\n", mx, my);
        return true;
    }
    return false;
}

bool BaseWidget::scrolled(int mx, int my, int px) {
    // do nothing
    return false;
}

bool BaseWidget::onclick(int mx, int my, int button) {
    // do nothing
    return false;
}