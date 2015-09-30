/*
 * Use this link for basic basic setup!
 * http://www.glfw.org/docs/latest/quick.html
 *
 */

//
//  Entrypoint for game
//  - Handles resizing, viewport width/height
//  - Main render loop here
//  -
//
//  Used by:
//  - A human! :D
//
//  Uses:
//  - shader_loader.cpp (to load shaders)
//  - game.cpp (to run the game, duh!)
//  -
//
//  Notes
//  - Later convert this entirely into a viewport and put start point somewhere else
//  - Read up on https://sites.google.com/site/letsmakeavoxelengine/home/basic-block-rendering
//  - OpenGL https://en.wikibooks.org/wiki/OpenGL_Programming

#include "display.h"

int main(void)
{
    int status = run();
    if (status != 0) {
        printf("Program had an exception while running\n");
    }
    return status;
}