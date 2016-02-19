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

// TOFU multiplex this input for windows
// SDL/OpenGL imports
#include "importsdl.h"
#include "importopengl.h"

// resources
#include "texture_loader.h"
#include "textrender.h"
#include "threadmanager.h"

// display-related
#include "gbuffer.h"
#include "shadow_mapper.h"

// shaders
#include "shader_loader.h"
#include "worldrender.h"

// debug
#include "opengl_debug.h"

// --- Screen Dimensions ---
// physical px width on screen
int window_width = 720;
int window_height = 480;

// width of the framebuffers
int screen_width = window_width;
int screen_height = window_height;

// OpenGL Context
SDL_GLContext glContext;

// --- FPS Related Globals ---
const double FRAME_RATE=30.0;
// in milliseconds
const double TIME_PER_FRAME = 1000.0f / FRAME_RATE;

// if current time is bigger than this, run the frame()
uint32_t previous_time = 0;

// --- Rendering Buffers ---
SDL_Window* window;
// wow talk about inefficient
GBuffer g_buffer;
GBuffer g_buffer_transparent;
ShadowMapper shadow_mapper;

// --- Input Related ---
double xprev, yprev = 0;
double max_mouse_movement = 100;
double mouse_speed = 1.0;

// --- Current Displayable ---
Displayable* current_display;

// --- Exit Flag ---
bool should_exit = 0;

// --- Display Info Accessors ---
void get_window_size(int* width, int* height) {
    //glfwGetFramebufferSize(window, width, height);
    *width = screen_width;
    *height = screen_height;
}

void display_enable_cursor() {
    SDL_SetRelativeMouseMode(SDL_FALSE);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void display_disable_cursor() {
    SDL_SetRelativeMouseMode(SDL_TRUE);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void display_get_cursor_position(double* x, double* y) {
    //glfwGetWindowSize(window, &wwidth, &wheight);
    //glfwGetFramebufferSize(window, &fwidth, &fheight);
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    printf("Clicked %d %d\n", mx, my);
    my = window_height - my;
    *x = (mx * screen_width) / window_width;
    *y = (my * screen_height) / window_height;
}

// these are deprecated
void key_callback(int key, int scancode, int action, int mods) {
    if (current_display) {
        current_display->key_callback(key, scancode, action, mods);
    }
}

void mouse_move_callback(double xpos, double ypos) {
    if (current_display) {
        //double xdiff = xpos - xprev;
        //double ydiff = ypos - yprev;
        double xdiff = xpos;
        double ydiff = ypos;
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

void mouse_button_callback(int button, int action, int mods) {
    if (current_display)
        current_display->mouse_button_callback(button, action, mods);
}

void scroll_callback(double xoffset, double yoffset) {
    if (current_display)
        current_display->scroll_callback(xoffset, yoffset);
}

// --- INIT ---
int init_display() {
    printf("Initializing display\n");
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Unable to init video");
        return 1;
    }
    
    window = SDL_CreateWindow("Huzzah",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              window_width, window_height,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    
    if (!window) {
        printf("Unable to make SDL window\n");
        return 1;
    }
    
    printf("Setting OpenGL hints\n");
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
    glContext = SDL_GL_CreateContext(window);
    
    if (glContext == NULL) {
        printf("There was an error creating the OpenGL context!\n");
        return 0;
    }
    
    const unsigned char* version = glGetString(GL_VERSION);
    if (version == NULL) {
        printf("There was an error creating the OpenGL context!\n");
        return 1;
    }
    
    printf("OpenGL version: %s\n", version);
    printf("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    SDL_GL_MakeCurrent(window, glContext);
    
    //render_debug();
    
    // --- SETUP BUFFERS ---
    int wwidth, wheight;
    // note: window width, height NOT equal to pixel width we give to GLFW window
    get_window_size(&wwidth, &wheight);
    g_buffer.init(wwidth, wheight);
    g_buffer_transparent.init(wwidth, wheight);
    shadow_mapper.init(wwidth, wheight);
    
    /* Connect inputs to game */
    //glfwSetKeyCallback(window, key_callback);
    
    display_disable_cursor();
    
    //glfwSetCursorPosCallback(window, mouse_move_callback);
    //glfwSetMouseButtonCallback(window, mouse_button_callback);
    //glfwSetScrollCallback(window, scroll_callback);
    check_for_error();
    
    if (set_shaders()) {
        return 1;
    }
    check_for_error();
    // load our textures
    load_textures();
    // initialize our threader
    init_threader();
    // initialize font library resources
    init_string_resources();
    // set up some shaders
    world_load_resources();
    
    return 0;
}

void set_current_displayable(Displayable* display) {
    current_display = display;
}

void close_render_loop() {
    should_exit = 1;
}

void display_close() {
    // free some shader related stuff
    world_free_resources();
    // free font library resources
    clean_string_resources();
    // free textures
    cleanup_textures();
    glDeleteProgram(OGLAttr::geometry_shader.program);
    glDeleteProgram(OGLAttr::lighting_shader.program);
    glDeleteProgram(OGLAttr::shadow_shader.program);
    SDL_GL_DeleteContext(glContext);
    SDL_Quit();
    //glfwTerminate();
}

// --- Rendering Stages ---
void render_geometry() {
    // renders the geometry/colors
    set_geometry_as_current_shader();
    check_for_error();
    
    // disable blending. we have only RGB (not RGBA)
    glDisable(GL_BLEND);
    
    int width, height;
    get_window_size(&width, &height);
    glEnable(GL_DEPTH_TEST);
    
    // first, write to our regular G-buffer with a low alpha-cutoff
    g_buffer.bind_for_write();
    check_for_error();
    OGLAttr::current_shader->set_alpha_cutoff(0.3f);
    check_for_error();
    
    // clear the viewport, screen
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    OGLAttr::current_shader->set_shader_intensity(0.5f); // this needs to be deprecated/changed
    check_for_error();
    current_display->render();
    check_for_error();
    //glFinish();
    
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
    //glFinish();
    check_for_error();
}

void render_shadowmetry() {
    // renders the geometry from light's perspective
    set_shadow_as_current_shader();
    check_for_error();
    
    glDisable(GL_BLEND);
    
    int width, height;
    get_window_size(&width, &height);
    // enable depth obviously, since we're writing light depth...
    glEnable(GL_DEPTH_TEST);
    check_for_error();
    // tell our shader to actually
    shadow_mapper.bind_for_write();
    check_for_error();
    
    // clear the viewport, screen
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    check_for_error();
    current_display->render_shadows();
    //glFinish();
    check_for_error();
}

void render_g_buffer() {
    // test method to render the G-buffer contents directly to screen
    check_for_error();
    
    int wwidth, wheight;
    get_window_size(&wwidth, &wheight);
    
    // set up framebuffers for reading
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    check_for_error();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, wwidth, wheight);
    check_for_error();
    g_buffer.bind_for_readg();
    
    GLsizei halfwwidth = (GLsizei)(wwidth / 2.0f);
    GLsizei halfwheight = (GLsizei)(wheight / 2.0f);
    
    check_for_error();
    
    // copy framebuffer contents over
    g_buffer.set_read_buffer(GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
    glBlitFramebuffer(0, 0, wwidth, wheight,
                      0, 0, halfwwidth, halfwheight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    g_buffer.set_read_buffer(GBuffer::GBUFFER_TEXTURE_TYPE_COLOR);
    glBlitFramebuffer(0, 0, wwidth, wheight,
                      0, halfwheight, halfwwidth, wheight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    g_buffer.set_read_buffer(GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);
    glBlitFramebuffer(0, 0, wwidth, wheight,
                      halfwwidth, 0, wwidth, halfwheight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    
    // copy the transparent framebuffer contents over
    g_buffer_transparent.bind_for_readg();
    g_buffer_transparent.set_read_buffer(GBuffer::GBUFFER_TEXTURE_TYPE_COLOR);
    glBlitFramebuffer(0, 0, wwidth, wheight,
                      halfwwidth, halfwheight, wwidth, wheight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    
    glFinish();
    check_for_error();
}

void render_shadowmap() {
    // test method to render the shadowmap contents directly to screen
    int wwidth, wheight;
    get_window_size(&wwidth, &wheight);
    
    float vertex[6][3] = {
        {-0.5f, -0.5f, 0},
        {0.5f, -0.5f, 0},
        {-0.5f, 0.5f, 0},
        {-0.5f, 0.5f, 0},
        {0.5f, -0.5f, 0},
        {0.5f, 0.5f, 0},
    };
    
    // overwrite our framebuffer
    glDisable(GL_BLEND);
    set_lighting_block_draw_mode(LightDrawMode::LIGHT_TEST);
    set_lighting_screen_size(wwidth, wheight);
    set_unitary_lighting_transform_matrix();
    check_for_error();
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof vertex, vertex, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(OGLAttr::lighting_shader.coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    check_for_error();
    glEnable(GL_BLEND);
}

void render_lights() {
    // renders the lights in the scene using geometry of the scene/lights in the scene
    glUseProgram(OGLAttr::lighting_shader.program);
    check_for_error();
    glEnableVertexAttribArray(OGLAttr::lighting_shader.coord);
    
    check_for_error();
    
    int wwidth, wheight;
    get_window_size(&wwidth, &wheight);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    check_for_error();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, wwidth, wheight);
    check_for_error();
    
    // bind all our buffers for reading
    g_buffer.bind_for_read();
    g_buffer_transparent.bind_for_read_color_map_only();
    shadow_mapper.bind_for_read();
    check_for_error();
    
    // disable depth test - i don't know why but it's needed
    glDisable(GL_DEPTH_TEST);
    // enable blending since we rendering lights
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
   	glBlendFunc(GL_ONE, GL_ONE);
    
    set_lighting_screen_size(wwidth, wheight);
    
    current_display->render_lights();
    
    // for debugging only
    //render_shadowmap();
}

void render_debug() {
    /* Clear our buffer with a red background */
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    /* Swap our back buffer to the front */
    SDL_GL_SwapWindow(window);
    /* Wait 2 seconds */
    SDL_Delay(200);
    
    /* Same as above, but green */
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(window);
    SDL_Delay(200);
    
    /* Same as above, but blue */
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(window);
    SDL_Delay(200);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

int display_run()
{
    should_exit = false;
    previous_time = get_current_time();
    // render loop
    
    while (!should_exit) {
        // change to while to make it catch up to events
        uint32_t current_time = get_current_time();
        if (current_time > previous_time + TIME_PER_FRAME) {
            uint32_t diff_time = std::min((int)(current_time - previous_time), 100);
            current_display->frame(diff_time);
            previous_time += diff_time;
            step_threader();
        }
        
        render_geometry();
        render_shadowmetry();
        //render_g_buffer();
        render_lights();
        
        check_for_error();
        
        /* Swap front and back buffers */
        //glfwSwapBuffers(window);
        SDL_GL_SwapWindow(window);
        
        //SDL_Delay(20);
        /* Poll for and process events */
        //glfwPollEvents();
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                should_exit = true;
            }
            else if (e.type == SDL_KEYUP || e.type == SDL_KEYDOWN) {
                key_callback(e.key.keysym.sym, e.key.keysym.scancode, e.type, e.key.keysym.mod);
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) {
                mouse_button_callback(e.button.button, e.type, 0);
            }
            else if (e.type == SDL_MOUSEMOTION) {
                mouse_move_callback(e.motion.xrel, e.motion.yrel);
            }
            else if (e.type == SDL_MOUSEWHEEL) {
                scroll_callback(e.wheel.x, e.wheel.y);
            }
            else if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_x) {
                should_exit = true;
            }
        }
    }
    
    close_all_threads();
    
    if (!should_exit) {
        // this was called by a call to exit window. close the program
        return -1;
    }
    
    return 0;
}

/*
 
void show_depth_peeler() {
 glViewport(0, 0, width, height);
 
 set_active_texture(OGLAttr::active_tile_texture);
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
 
void deprecated_uncalled() {
 // to make a depth peeling texture
 glEnable(GL_TEXTURE_2D);
 glGenTextures(1, &depth_peeling_texture);
 glBindTexture(GL_TEXTURE_2D, depth_peeling_texture);
 get_window_size(&width, &height);
 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
 
 // to render said depth peeling texture, by copying it over to our buffer
glViewport(0, 0, width, height);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
set_alpha_set(1.0f);
bind_to_tiles();
OGLAttr::current_shader->set_alpha_cutoff(1.0f/3.0f);
current_display->render();
glFlush();
glBindTexture(GL_TEXTURE_2D, depth_peeling_texture);
glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height);
//glEnable(GL_BLEND);
//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}
*/
