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

// shaders and attributes set by shader loading program
GLuint block_attribute_coord;
GLuint block_uniform_mvp;
GLuint program;

GLFWwindow* window;

// input related variables
double xprev, yprev = 0;
double max_mouse_movement = 100;
double mouse_speed = 1.0;

// replace this with current input reading interface
Game* current_game;

int init_resources() {
    // shader program for blocks
    if (!set_shaders()) {
        // shader failed to load, crash program here
        // TODO
        return -1;
    }
    
    glEnable(GL_CULL_FACE);
    glPolygonOffset(1, 1);
    
    return 0;
}

void free_resources() {
    // TODO this needs work
    glDeleteProgram(program);
}

void get_window_size(int* width, int* height) {
    glfwGetWindowSize(window, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (current_game) {
        current_game->key_callback(key, scancode, action, mods);
    }
}

void mouse_move_callback(GLFWwindow* window, double xpos, double ypos) {
    if (current_game) {
        double xdiff = xpos - xprev;
        double ydiff = ypos - yprev;
        if (xdiff < -max_mouse_movement) {
            xdiff = -max_mouse_movement;
        }
        else if (xdiff > max_mouse_movement) {
            xdiff = max_mouse_movement;
        }
        if (ydiff < -max_mouse_movement) {
            ydiff = -max_mouse_movement;
        }
        else if (ydiff > max_mouse_movement) {
            ydiff = max_mouse_movement;
        }
        xprev = xpos;
        yprev = ypos;
        
        int width, height;
        get_window_size(&width, &height);
        current_game->mouse_move_callback(xdiff / width * mouse_speed, ydiff / height * mouse_speed);
    }
    printf("Mouse movement %f %f\n", xpos, ypos);
}

int run()
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;
    
    int width = 640;
    int height = 480;
    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Bawk", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    if (!init_resources())
        return -1;
    
    /* Initializes the game. This might need changing in the future */
    Game game;
    game.init();
    
    /* Initializes OpenGL necessities */
    //::init();
    
    current_game = &game;
    
    /* Connect inputs to game */
    glfwSetKeyCallback(window, key_callback);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_move_callback);
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_POLYGON_OFFSET_FILL);
        
        /* Render here */
        game.render();
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        
        /* Poll for and process events */
        glfwPollEvents();
    }
    
    glfwTerminate();
    
    // TODO make a call to free resources here
    free_resources();
    
    return 0;
}
