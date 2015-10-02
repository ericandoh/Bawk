//
//  worldrender.cpp
//  Bawk
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include <cstdlib>
#include "shader_loader.h"
#include "worldrender.h"

// shaders and attributes set by shader loading program
GLuint block_attribute_coord;
GLuint block_uniform_mvp;
GLuint program;

// constructs a world render
WorldRender::WorldRender() {
    printf("Adding world render\n");
    SuperObject* world = new SuperObject();
    world->init();
    superobjects.push_back(world);
}

int WorldRender::load_resources() {
    if (set_shaders(&block_attribute_coord,
                    &block_uniform_mvp,
                    &program)) {
        return -1;
    }
    
    glEnable(GL_CULL_FACE);
    glPolygonOffset(1, 1);
    return 0;
}

void WorldRender::free_resources() {
    for (int i = 0; i < superobjects.size(); i++) {
        delete superobjects[i];
    }
    glDeleteProgram(program);
}

void drawRectangle(float x, float y, float width, float height, float r, float g, float b) {
    glColor3f(r,g,b);                   // setting the color to w/e for the rect
    glBegin(GL_POLYGON);                         // draw rectangle
    glVertex3f(x, y , 0.0f);               // bottom left corner of rectangle
    glVertex3f(x, y + height, 0.0f);               // top left corner of rectangle
    glVertex3f(x + width, y + height, 0.0f);               // top right corner of rectangle
    glVertex3f(x + width, y, 0.0f);               // bottom right corner of rectangle
    glEnd();
}

// renders the world
void WorldRender::render(fmat4* transform) {
    //drawRectangle(0.1f, 0.1f, 0.5f, 0.7f, 0.5f, 0.5f, 0.3f);
    for (int i = 0; i < superobjects.size(); i++) {
        superobjects[i]->render(transform);
    }
}