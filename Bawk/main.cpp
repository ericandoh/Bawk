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
//  -

#include "display.h"

int main(void)
{
    run();
    return 0;
}