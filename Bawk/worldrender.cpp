//
//  worldrender.cpp
//  Bawk
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include <cstdlib>
#include "worldrender.h"
#include <GLFW/glfw3.h>

// constructs a world render
WorldRender::WorldRender() {
    for (int x = 0; x < CX; x++) {
        for (int y = 0; y < CY; y++) {
            for (int z = 0; z < CZ; z++) {
                if (rand() % 10 < 3) {
                    chunk.set(x, y, z, (uint8_t)(rand() * sizeof(uint8_t)));
                }
            }
        }
    }
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
void WorldRender::render() {
    //drawRectangle(0.1f, 0.1f, 0.5f, 0.7f, 0.5f, 0.5f, 0.3f);
    chunk.render();
}