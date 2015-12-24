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
#include "gbuffer.h"
#include "shader_loader.h"
#include "worldrender.h"
#include "shadow_mapper.h"

#include "opengl_debug.h"

// 30 fps
const double FRAME_RATE=30.0;
const double TIME_PER_FRAME = 1.0 / FRAME_RATE;

GLFWwindow* window;
// wow talk about inefficient
GBuffer g_buffer;
GBuffer g_buffer_transparent;

ShadowMapper shadow_mapper;

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
    
    /* Create the G-Buffer */
    // VERSION 1.1 NOT SUPPORTED
    int wwidth, wheight;
    // note: window width, height NOT equal to pixel width we give to GLFW window
    get_window_size(&wwidth, &wheight);
    printf("0 %d\n",glGetError());
    // 0        => tile_texture
    // 1,2,3    => G_BUFFER textures
    // 4,5,6    => G_BUFFER transparent textures (for depth peeling)
    // 7        => SHADOW MAP texture(s)
    g_buffer.init(wwidth, wheight, 1);
    g_buffer_transparent.init(wwidth, wheight, 4);
    
    shadow_mapper.init(wwidth, wheight, 7);
    
    /* Connect inputs to game */
    glfwSetKeyCallback(window, key_callback);
    
    display_disable_cursor();
    
    glfwSetCursorPosCallback(window, mouse_move_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    check_for_error();
    if (version <= 1) {
        // VERSION 1.1
        glEnable(GL_TEXTURE_2D);
    }
    else {
        
    }
    
    // THIS IS DEPRECATED CODE FOR
    // VERSION 1.1
    /*printf("2 %d\n",glGetError());
    glGenTextures(1, &depth_peeling_texture);
    glBindTexture(GL_TEXTURE_2D, depth_peeling_texture);
    get_window_size(&width, &height);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    printf("3 %d\n",glGetError());*/
    
    if (set_shaders()) {
        return 1;
    }
    
    check_for_error();
    
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
    glDeleteProgram(OGLAttr::geometry_shader.program);
    glDeleteProgram(OGLAttr::lighting_shader.program);
    glDeleteProgram(OGLAttr::shadow_shader.program);
    glfwTerminate();
}

// DEPRECATED THIS IS DEPRECATED
// VERSION 1.1
void show_depth_peeler() {
    // DEPRECATED
    //glViewport(0, 0, width, height);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, OGLAttr::tile_texture);
    glDisable(GL_DEPTH_TEST);
    
    OGLAttr::current_shader->set_block_draw_mode(BlockDrawMode::UV);
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
    
    set_unitary_transform_matrix();
    
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof vertex, vertex, GL_DYNAMIC_DRAW);
    OGLAttr::current_shader->set_coord_attribute(GL_FLOAT);
    
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_texture_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof texture, texture, GL_DYNAMIC_DRAW);
    OGLAttr::current_shader->set_texture_coord_attribute(GL_FLOAT);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void render_geometry() {
    check_for_error();
    // VERSION 1.1 NOT SUPPORTED
    set_geometry_as_current_shader();
    
    // disable blending. we have only RGB (not RGBA)
    glDisable(GL_BLEND);
    
    // bind TEXTURES0 to our uniform
    glUniform1i(OGLAttr::geometry_shader.tile_texture, 0);
    
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glEnable(GL_DEPTH_TEST);
    
    // first, write to our regular G-buffer with a low alpha-cutoff
    g_buffer.bind_for_write();
    check_for_error();
    OGLAttr::current_shader->set_alpha_cutoff(0.3f);
    check_for_error();
    
    // VERSION 1.1
    /*// reset viewport to window width, assume we're rendering on the whole screen
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    set_alpha_set(1.0f);
    bind_to_tiles();
    OGLAttr::current_shader->set_alpha_cutoff(1.0f/3.0f);
    current_display->render();
    glFlush();
    glBindTexture(GL_TEXTURE_2D, depth_peeling_texture);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height);*/
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // clear the viewport, screen
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    OGLAttr::current_shader->set_shader_intensity(0.5f); // this needs to be deprecated/changed
    check_for_error();
    current_display->render();
    check_for_error();
    glFinish();
    
    // now, write to our depth-peeled, 2nd G-buffer which will NOT render medium alpha components
    g_buffer_transparent.bind_for_write();
    check_for_error();
    OGLAttr::current_shader->set_alpha_cutoff(0.7f);
    check_for_error();
    
    // clear the viewport, screen
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    OGLAttr::current_shader->set_shader_intensity(0.5f); // this needs to be deprecated/changed
    check_for_error();
    current_display->render();
    check_for_error();
    glFinish();
    check_for_error();
}

void render_shadowmetry() {
    check_for_error();
    set_shadow_as_current_shader();
    
    glDisable(GL_BLEND);
    
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glEnable(GL_DEPTH_TEST);
    check_for_error();
    // tell our shader to actually
    shadow_mapper.bind_for_write();
    check_for_error();
    
    // clear the viewport, screen
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    check_for_error();
    current_display->render();
    check_for_error();
    glFinish();
    check_for_error();
}

void render_g_buffer() {
    check_for_error();
    
    int wwidth, wheight;
    glfwGetFramebufferSize(window, &wwidth, &wheight);
    
    // VERSION 1.1 NOT SUPPORTED
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    check_for_error();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, wwidth, wheight);
    check_for_error();
    g_buffer.bind_for_readg();
    
    GLsizei halfwwidth = (GLsizei)(wwidth / 2.0f);
    GLsizei halfwheight = (GLsizei)(wheight / 2.0f);
    
    check_for_error();
    
    g_buffer.set_read_buffer(GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
    glBlitFramebuffer(0, 0, wwidth, wheight,
                      0, 0, halfwwidth, halfwheight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    g_buffer.set_read_buffer(GBuffer::GBUFFER_TEXTURE_TYPE_COLOR);
    glBlitFramebuffer(0, 0, wwidth, wheight,
                      0, halfwheight, halfwwidth, wheight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    g_buffer.set_read_buffer(GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);
    glBlitFramebuffer(0, 0, wwidth, wheight,
                      halfwwidth, 0, wwidth, halfwheight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    
    g_buffer_transparent.bind_for_readg();
    // top right is normals
    g_buffer_transparent.set_read_buffer(GBuffer::GBUFFER_TEXTURE_TYPE_COLOR);
    glBlitFramebuffer(0, 0, wwidth, wheight,
                      halfwwidth, halfwheight, wwidth, wheight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    
    check_for_error();
    glFinish();
    check_for_error();
}

void render_shadowmap() {
    int wwidth, wheight;
    glfwGetFramebufferSize(window, &wwidth, &wheight);
    
    float vertex[6][3] = {
        {-1, -1, 0},
        {0, -1, 0},
        {-1, 0, 0},
        {-1, 0, 0},
        {0, -1, 0},
        {0, 0, 0},
    };
    
    glDisable(GL_BLEND);
    set_lighting_block_draw_mode(3);
    set_lighting_screen_size(wwidth, wheight);
    set_unitary_lighting_transform_matrix();
    //glUniform1i(OGLAttr::lighting_shader.shadow_map, 0);
    check_for_error();
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof vertex, vertex, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(OGLAttr::lighting_shader.coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    check_for_error();
    glEnable(GL_BLEND);
}

void render_lights() {
    check_for_error();
    glUseProgram(OGLAttr::lighting_shader.program);
    check_for_error();
    glEnableVertexAttribArray(OGLAttr::lighting_shader.coord);
    
    check_for_error();
    
    int wwidth, wheight;
    glfwGetFramebufferSize(window, &wwidth, &wheight);
    // VERSION 1.1 NOT SUPPORTED
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    check_for_error();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, wwidth, wheight);
    check_for_error();
    
    g_buffer.bind_for_read();
    g_buffer_transparent.bind_for_read_color_map_only();
    check_for_error();
    
    shadow_mapper.bind_for_read();
    
    check_for_error();
    
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
   	glBlendFunc(GL_ONE, GL_ONE);
    
    // drawing ambient lighting here
    set_lighting_block_draw_mode(0);
    set_lighting_screen_size(wwidth, wheight);
    float vertex[6][3] = {
        {-1, -1, 0},
        {1, -1, 0},
        {-1, 1, 0},
        {-1, 1, 0},
        {1, -1, 0},
        {1, 1, 0},
    };
    
    set_unitary_lighting_transform_matrix();
    
    check_for_error();
    
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof vertex, vertex, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(OGLAttr::lighting_shader.coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    check_for_error();
    
    // draw more lights here, by drawing a box, then sending coordinates to the renderer above
    // this will mainly be just point lights top kek
    
    current_display->render_lights();
    
    // for debugging only
    // render_shadowmap();
}

int display_run()
{
    should_exit = false;
    need_refresh_time = glfwGetTime() + TIME_PER_FRAME;
    /* Loop until the user closes the window */
    while (!(glfwWindowShouldClose(window) || should_exit))
    {
        
        //printf("4 %d\n",glGetError());
        
        // change to while to make it catch up to events
        if (glfwGetTime() > need_refresh_time) {
            need_refresh_time += TIME_PER_FRAME;
            current_display->frame();
        }
        
        render_geometry();
        render_shadowmetry();
        //render_g_buffer();
        render_lights();
        
        check_for_error();
        
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

