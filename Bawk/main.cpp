/*
 * Use this link for basic basic setup!
 * http://www.glfw.org/docs/latest/quick.html
 *
 */

//
//  Entrypoint for game, also represents viewport for this game
//  - Handles resizing, viewport width/height
//  - Main render loop here
//  -
//
//  Used by:
//  -
//
//  Uses:
//  -
//  -
//  -
//
//  Notes
//  - Later convert this entirely into a viewport and put start point somewhere else
//  -

#include <GLFW/glfw3.h>
#include "game.h"

class Viewport {
public:
    int w, h;
};

Viewport viewport;

// reshapes window. NOT IN USE
void reshape(int w, int h) {
    viewport.w = w;
    viewport.h = h;
    
    glViewport(0, 0, viewport.w, viewport.h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, 1, -1);
}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    reshape(viewport.w, viewport.h);
}

int main(void)
{
    GLFWwindow* window;
    
    /* Initialize the library */
    if (!glfwInit())
        return -1;
    
    viewport.w = 640;
    viewport.h = 480;
    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(viewport.w, viewport.h, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    /* Initializes OpenGL necessities */
    ::init();
    
    /* Initializes the game. This might need changing in the future */
    game::init();
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        /* Render here */
        game::render();
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        
        /* Poll for and process events */
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}