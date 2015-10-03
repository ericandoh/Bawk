/*
 * Use this link for basic basic setup!
 * http://www.glfw.org/docs/latest/quick.html
 *
 *
 * Later multithread the input (cycle) stepping
 * http://stackoverflow.com/questions/16560923/how-to-create-a-glfw-thread-in-c
 *
 */

#include "display.h"

GLFWwindow* window;

// input related variables
double xprev, yprev = 0;
double max_mouse_movement = 100;
double mouse_speed = 1.0;

// replace this with current input reading interface
Displayable* current_display;

// used to exit the render loop cleanly
bool should_exit = 0;

void get_window_size(int* width, int* height) {
    glfwGetWindowSize(window, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (current_display) {
        current_display->key_callback(key, scancode, action, mods);
    }
}

void mouse_move_callback(GLFWwindow* window, double xpos, double ypos) {
    if (current_display) {
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
        current_display->mouse_move_callback(xdiff / width * mouse_speed, ydiff / height * mouse_speed);
    }
}

int init_display() {
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
    
    // can set vsync interval later on
    //glfwSwapInterval(1);
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    /* Connect inputs to game */
    glfwSetKeyCallback(window, key_callback);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_move_callback);
    
    return 0;
}

void set_current_game(Displayable* display) {
    current_display = display;
}

void close_render_loop() {
    should_exit = 1;
}

void display_close() {
    glfwTerminate();
}

int display_run()
{
    should_exit = false;
    /* Loop until the user closes the window */
    while (!(glfwWindowShouldClose(window) || should_exit))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_POLYGON_OFFSET_FILL);
        
        /* Render here */
        current_display->render();
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        
        /* Poll for and process events */
        glfwPollEvents();
    }
    
    if (!should_exit) {
        // this was called by a call to exit window. close the program
        return -1;
    }
    
    return 0;
}

