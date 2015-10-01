/*
 * Use this link for basic basic setup!
 * http://www.glfw.org/docs/latest/quick.html
 *
 */

//
//  Entrypoint for game
//  -
//  -
//
//  Used by:
//  - A human! :D
//
//  Uses:
//  - program.h (the actual program)
//  -
//
//  Notes
//  - Read up on https://sites.google.com/site/letsmakeavoxelengine/home/basic-block-rendering
//  - OpenGL https://en.wikibooks.org/wiki/OpenGL_Programming

#include "program.h"

int main(void)
{
    int status = run_game();
    if (!status) {
        printf("Program exitted with error code %d\n", status);
    }
    printf("End of program\n");
    return 0;
}