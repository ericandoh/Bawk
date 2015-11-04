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
#include "worldrender.h"

// REMOVEME
#include "texture_loader.h"

// 30 fps
const double FRAME_RATE=30.0;
const double TIME_PER_FRAME = 1.0 / FRAME_RATE;

GLFWwindow* window;

GLuint depth_peeling_texture;

// if current time is bigger than this, run the frame()
double need_refresh_time = 0.0;

// input related variables
double xprev, yprev = 0;
double max_mouse_movement = 100;
double mouse_speed = 1.0;

// replace this with current input reading interface
Displayable* current_display;

// used to exit the render loop cleanly
bool should_exit = 0;

void get_window_size(int* width, int* height) {
    glfwGetFramebufferSize(window, width, height);
}

void display_enable_cursor() {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void display_disable_cursor() {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void display_get_cursor_position(double* x, double* y) {
    int wwidth, wheight, fwidth, fheight;
    glfwGetWindowSize(window, &wwidth, &wheight);
    glfwGetFramebufferSize(window, &fwidth, &fheight);
    glfwGetCursorPos(window, x, y);
    *y = wheight - *y;
    *x = (*x * fwidth) / wwidth;
    *y = (*y * fheight) / wheight;
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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (current_display)
        current_display->mouse_button_callback(button, action, mods);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (current_display)
        current_display->scroll_callback(xoffset, yoffset);
}

int init_display() {
    /* Initialize the library */
    if (!glfwInit())
        return -1;
    
    int width = 720;   //1080
    int height = 480;   //640
    
    int version;
    //version = 1;
    version = 3;
    
    // change for different versions, or well this does the versioning actually lol
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // later change this to support 1.1 as well, if we have time/are okay with this
    // then we can use/load in frag_shader_old and vertex_shader_old instead
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Bawk", NULL, NULL);
    if (!window)
    {
        printf("Could not create GLFW context window...\n");
        glfwTerminate();
        return -1;
    }
    
    std::cout<<"OpenGL "<<glGetString(GL_VERSION)<<" (GLSL "<<glGetString(GL_SHADING_LANGUAGE_VERSION)<<')'<<std::endl;
    
    // can set vsync interval later on
    //glfwSwapInterval(1);
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    /* Connect inputs to game */
    glfwSetKeyCallback(window, key_callback);
    
    display_disable_cursor();
    
    glfwSetCursorPosCallback(window, mouse_move_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    printf("1 %d\n",glGetError());
    
    if (version <= 1) {
        // VERSION 1.1
        glEnable(GL_TEXTURE_2D);
    }
    else {
        
    }
    
    printf("2 %d\n",glGetError());
    glGenTextures(1, &depth_peeling_texture);
    glBindTexture(GL_TEXTURE_2D, depth_peeling_texture);
    get_window_size(&width, &height);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    printf("3 %d\n",glGetError());
    return 0;
}

void set_current_game(Displayable* display) {
    current_display = display;
}

void close_render_loop() {
    should_exit = 1;
}

void display_close() {
    glDeleteTextures(1, &depth_peeling_texture);
    glfwTerminate();
}

void show_depth_peeler() {
    glBindTexture(GL_TEXTURE_2D, depth_peeling_texture);
    glDisable(GL_DEPTH_TEST);
    
    set_block_draw_mode(0);
    float vertex[6][3] = {
        {-1, -1, 0},
        {1, -1, 0},
        {-1, 1, 0},
        {-1, 1, 0},
        {1, -1, 0},
        {1, 1, 0},
    };
    
    float texture[6][3] = {
        {0, 0, 0},
        {1, 0, 0},
        {0, 1, 0},
        {0, 1, 0},
        {1, 0, 0},
        {1, 1, 0},
    };
    
    glm::mat4 one(1);
    set_transform_matrix(one);
    
    glBindBuffer(GL_ARRAY_BUFFER, get_vertex_attribute_vbo());
    glBufferData(GL_ARRAY_BUFFER, sizeof vertex, vertex, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(block_attribute_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, get_texture_attribute_vbo());
    glBufferData(GL_ARRAY_BUFFER, sizeof texture, texture, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(texture_attribute_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

int display_run()
{
    should_exit = false;
    need_refresh_time = glfwGetTime() + TIME_PER_FRAME;
    /* Loop until the user closes the window */
    int width, height;
    while (!(glfwWindowShouldClose(window) || should_exit))
    {
        // change to while to make it catch up to events
        if (glfwGetTime() > need_refresh_time) {
            need_refresh_time += TIME_PER_FRAME;
            current_display->frame();
        }
        
        glfwGetFramebufferSize(window, &width, &height);
        
        // reset viewport to window width, assume we're rendering on the whole screen
        /*glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        set_alpha_set(1.0f);
        bind_to_tiles();
        set_alpha_cutoff(1.0f/3.0f);
        current_display->render();
        glFlush();
        
        glBindTexture(GL_TEXTURE_2D, depth_peeling_texture);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height);*/
        
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        set_alpha_set(1.0f);
        bind_to_tiles();
        set_alpha_cutoff(2.0f/3.0f);
        current_display->render();
        // render the transparency texture
        glViewport(0, 0, width, height);
        set_alpha_set(0.5f);
        //show_depth_peeler();
        
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

