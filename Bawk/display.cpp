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
//
//  Created by Eric Oh on 9/29/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "game.h"
#include "shader_loader.h"
#include "display.h"

class Viewport {
public:
    int w, h;
};

Viewport viewport;
GLuint block_attribute_coord;

// replace this with current input reading interface
Game* current_game;

// reshapes window. NOT IN USE
void reshape(int w, int h) {
    viewport.w = w;
    viewport.h = h;
    
    //GLfloat aspect = (GLfloat)w / (GLfloat)h;
    
    glViewport(0, 0, viewport.w, viewport.h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //glDepthFunc(GL_LESS);
    //glShadeModel(GL_SMOOTH);
    
    //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    
    reshape(viewport.w, viewport.h);
    
    // shader program for blocks
    block_attribute_coord = set_shaders();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (current_game) {
        current_game->key_callback(window, key, scancode, action, mods);
    }
}

int run(void)
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
    Game game;
    game.init();
    
    current_game = &game;
    
    /* Connect inputs to game */
    glfwSetKeyCallback(window, key_callback);
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        /* Render here */
        game.render();
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        
        /* Poll for and process events */
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}
