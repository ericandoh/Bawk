//
//  game.cpp
//  Bawk
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include <map>
#include "game.h"
#include "program.h"
#include "cursorsuperobject.h"
#include "cursorblock.h"
#include "base_widget.h"
#include "block_loader.h"
#include "display.h"
#include "blocktracer.h"
#include "gametemplate.h"
#include "debug_action.h"
#include "game_actions.h"
#include "cursorselector.h"

// deleteme
#include "cursormodelobject.h"
#include "cursorscantool.h"

#include "importsdl.h"

// THIS IS USEDF OR LIGHTING< MOVE TO A CUSTOM LIGHT RENDERING METHOD
// TODO FROG
#include <glm/gtc/matrix_transform.hpp>

/*int toggleable_keys[] = {GLFW_KEY_SPACE,
                            GLFW_KEY_C, GLFW_KEY_A, GLFW_KEY_D,
                            GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_TAB, GLFW_KEY_LEFT_SHIFT,
                            GLFW_KEY_Q, GLFW_KEY_E};*/

int toggleable_keys[] = {SDLK_SPACE,
    SDLK_c, SDLK_a, SDLK_d,
    SDLK_w, SDLK_s, SDLK_TAB, SDLK_LSHIFT,
    SDLK_q, SDLK_e};

std::map<int, Action> key_to_action;
std::map<int, Action> mouse_to_action;
std::map<int, bool> key_toggled;

// if the player moves more than 1 dimension away from previous location, update
float CHUNK_UPDATE_TRIGGER_DISTANCE = 16.0f;

void Game::set_parameters(std::string wn, uint32_t p) {
    world_name = wn;
    pid = p;
}

// initializes all needed game variables. This should be called before render()
int Game::init() {
    in_game = true;
    
    // initialize some rotation pre-calculations
    init_orientation_rotation_calculations();
    
    // load resources for the world
    if (world_load_resources()) {
        return 1;
    }
    
    // load game-related data
    if (load_game_data())
        return 1;
    
    world = new World(world_name);
    // later separate loading the player and the world - this might get complicated!
    player = new Player(pid);
    player->load_selfs();
    last_player_pos = player->get_rwc_pos();
    
    world->add_player(player);
    world->update_render(&(last_player_pos));
    
    // set key mappings
    // movement key mappings
    key_to_action[SDLK_SPACE] = MOVE_UP;
    key_to_action[SDLK_c] = MOVE_DOWN;
    key_to_action[SDLK_a] = MOVE_LEFT;
    key_to_action[SDLK_d] = MOVE_RIGHT;
    key_to_action[SDLK_w] = MOVE_FORWARD;
    key_to_action[SDLK_s] = MOVE_BACKWARD;
    
    key_to_action[SDLK_TAB] = PITCH_UP;
    key_to_action[SDLK_LSHIFT] = PITCH_DOWN;
    key_to_action[SDLK_q] = ROLL_LEFT;
    key_to_action[SDLK_e] = ROLL_RIGHT;
    
    key_to_action[SDLK_RETURN] = CONFIRM;
    key_to_action[SDLK_ESCAPE] = CANCEL;
    
    key_to_action[SDLK_PERIOD] = MOVE_BLOCK_UP;
    key_to_action[SDLK_SLASH] = MOVE_BLOCK_DOWN;
    key_to_action[SDLK_LEFT] = MOVE_BLOCK_LEFT;
    key_to_action[SDLK_RIGHT] = MOVE_BLOCK_RIGHT;
    key_to_action[SDLK_UP] = MOVE_BLOCK_FORWARD;
    key_to_action[SDLK_DOWN] = MOVE_BLOCK_BACKWARD;
    key_to_action[SDLK_SEMICOLON] = MOVE_BLOCK_ROTATE;
    
    key_to_action[SDLK_i] = OPEN_INV;
    key_to_action[SDLK_b] = SAVE_TEMPLATE;
    
    key_to_action[SDLK_m] = DEBUG_ACTION;
    
    key_to_action[SDLK_y] = MOUNTING;
    
    mouse_to_action[SDL_BUTTON_LEFT] = CLICK_MAIN;
    mouse_to_action[SDL_BUTTON_RIGHT] = CLICK_SECONDARY;
    
    for (unsigned int i = 0; i < sizeof(toggleable_keys); i++) {
        key_toggled[toggleable_keys[i]] = false;
    }
    
    game_template = 0;
    
    int width, height;
    get_window_size(&width, &height);
    story = new ParentWidget(0, 0, width, height);
    int bar_width = width * 7 / 100;
    bar = new ItemBar(player->inventory, bar_width*10, bar_width);
    story->add_child(bar);
    
    inventory_ui = new MainInventoryWidget(bar, player->inventory, width / 2, height / 2);
    
    bar->set_index(3);
    
    // for testing, TODO delete
    bar->set_current(new CursorModelObject(1));
    bar->set_index(4);
    bar->set_current(new CursorModelObject(2));
    bar->set_index(5);
    bar->set_current(new CursorModelObject(3));
    
    default_item = new CursorSelector();
    
    return 0;
}
    
// Called in the main render loop. Pass the rendering to the appropriate entries
void Game::render_geometry() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_CULL_FACE);
    // render world in player's perspective
    OGLAttr::current_shader->set_shader_intensity(1.0f);
    
    fmat4 transform(1);
    world->render(&transform, player);
    if (bar->get_current()) {
        bar->get_current()->render_in_world(&transform);
    }
}

void Game::render_ui() {
    glDisable(GL_CULL_FACE);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_DEPTH_TEST);
    // render the cursor
    player->render();
    story->render();
}

void Game::render() {
    // get transform from player's perspective
    player->set_camera();
    OGLAttr::current_shader->set_enable_shadows(true);
    render_geometry();
    OGLAttr::current_shader->set_enable_shadows(false);
    fmat4 transform(1);
    world->render_background(&transform, player);
    render_ui();
}

void Game::render_shadows() {
    // get transform from light's perspective
    world->set_light_camera(player);
    // only render geometry of world
    render_geometry();
    //fmat4 transform(1);
    //world->render_background(&transform, player);
}

void Game::render_lights() {
    
    // --- AMBIENT LIGHTING ---
    // drawing ambient lighting here
    set_lighting_block_draw_mode(LightDrawMode::BASE);
    float vertex[6][3] = {
        {-1, -1, 0},
        {1, -1, 0},
        {-1, 1, 0},
        {-1, 1, 0},
        {1, -1, 0},
        {1, 1, 0},
    };
    set_ambient_lighting_properties(1.0f);
    set_unitary_lighting_transform_matrix();
    world->set_light_camera_for_lighting(player);
    
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof vertex, vertex, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(OGLAttr::lighting_shader.coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    
    // --- OTHER LIGHTING ---
    // render a light around the player for now
    player->set_camera();
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    fmat4 transform(1);
    world->render_lights(&transform, player->get_rwc_pos());
    if (bar->get_current()) {
        bar->get_current()->render_light_in_world(&transform, player->get_rwc_pos());
    }
}

// runs one frame of the game
void Game::frame() {
    for (auto& key : key_toggled) {
        if (key.second) {
            Action do_this = key_to_action[key.first];
            // see if we're in any vehicle of any sort, then see if it'll accept the key i send
            SuperObject* mount = player->get_mount();
            if (mount) {
                // if mounted, always capture all key movements even if not valid
                // this is because all the other below default options should be DISABLE when mounted
                mount->block_keyboard_callback(this, do_this, mount);
            }
            else {
                switch (do_this) {
                    case MOVE_UP:
                        player->move_up_flat(5.0f);
                        break;
                    case MOVE_DOWN:
                        player->move_down_flat(5.0f);
                        break;
                    case MOVE_LEFT:
                        player->move_left(5.0f);
                        break;
                    case MOVE_RIGHT:
                        player->move_right(5.0f);
                        break;
                    case MOVE_FORWARD:
                        player->move_forward_flat(5.0f);
                        break;
                    case MOVE_BACKWARD:
                        player->move_backward_flat(5.0f);
                        break;
                    default:
                        // do nothing
                        ;
                }
            }
        }
    }
    check_need_update();
    
    // get depth coordinates
    player->query_depth(world);
    
    world->step(this);
    if (bar->get_current()) {
        bar->get_current()->step(this);
    }
}

float get_dst(fvec3* a, fvec3* b) {
    return sqrtf(powf(a->x - b->x, 2) + powf(a->y - b->y, 2) + powf(a->z - b->z, 2));
}

void Game::check_need_update() {
    fvec3 player_pos = player->get_rwc_pos();
    if (get_dst(&last_player_pos, &player_pos) >= CHUNK_UPDATE_TRIGGER_DISTANCE) {
        world->update_render(&player_pos);
        last_player_pos = player_pos;
    }
}

void Game::switch_current_item(int to_index) {
    bar->set_index(to_index);
}

void Game::key_callback_default(int key) {
    Action do_this = key_to_action[key];
    if (do_this == CONFIRM) {
        if (bar->get_current() && bar->get_current()->confirmed(this)) {
            ;
        }
        else if (game_template) {
            printf("Exporting template to current cursor!\n");
            if (bar->can_set_current()) {
                bar->set_current(game_template->create_from_template(player, world));
                GameTemplate* reference = game_template;
                game_template = 0;
                reference->publish(this);
                delete reference;
                game_template = 0;
            }
        }
        else {
            printf("Making new template\n");
            // create a new place_info
            game_template = new GameTemplate(world);
            world->add_entity(game_template);
        }
    }
    else if (do_this == CANCEL) {
        if (!in_game) {
            story->close_latest_child();
            if (story->count_children() == 1) {
                display_disable_cursor();
                in_game = true;
            }
        }
        else if (bar->get_current() && bar->get_current()->canceled(this)) {
            printf("Cancelling current template placing 1\n");
        }
        else if (game_template) {
            // we're canceling making this template
            printf("Cancelling current template\n");
            game_template->unpublish(world);
            delete game_template;
            game_template = 0;
        }
        else {
            printf("Escape pressed with no context. Exitting\n");
            exit_game();
        }
    }
    else if (do_this == MOVE_BLOCK_UP) {
        if (bar->get_current()) {
            printf("Moving placed template\n");
            bar->get_current()->handle_movement(ivec3(0, 1, 0));
        }
    }
    else if (do_this == MOVE_BLOCK_DOWN) {
        if (bar->get_current()) {
            printf("Moving placed template\n");
            bar->get_current()->handle_movement(ivec3(0, -1, 0));
        }
    }
    else if (do_this == MOVE_BLOCK_LEFT) {
        if (bar->get_current()) {
            printf("Moving placed template\n");
            ivec3 left = player->get_rounded_left();
            bar->get_current()->handle_movement(ivec3(-left.x, 0, -left.z));
        }
    }
    else if (do_this == MOVE_BLOCK_RIGHT) {
        if (bar->get_current()) {
            printf("Moving placed template\n");
            bar->get_current()->handle_movement(player->get_rounded_left());
        }
    }
    else if (do_this == MOVE_BLOCK_FORWARD) {
        if (bar->get_current()) {
            printf("Moving placed template\n");
            bar->get_current()->handle_movement(player->get_rounded_forward());
        }
    }
    else if (do_this == MOVE_BLOCK_BACKWARD) {
        if (bar->get_current()) {
            printf("Moving placed template\n");
            ivec3 forward = player->get_rounded_forward();
            bar->get_current()->handle_movement(ivec3(-forward.x, 0, -forward.z));
        }
    }
    else if (do_this == MOVE_BLOCK_ROTATE) {
        if (bar->get_current()) {
            bar->get_current()->handle_rotation();
        }
    }
    else if (do_this == SAVE_TEMPLATE) {
        player->inventory->add_custom_at(bar->get_current());
        if (story->has_child(inventory_ui)) {
            inventory_ui->refresh();
        }
    }
    else if (do_this == OPEN_INV) {
        story->toggle_child(inventory_ui);
        if (story->count_children() > 1) {
            inventory_ui->refresh();
            display_enable_cursor();
            in_game = false;
        }
        else {
            display_disable_cursor();
            in_game = true;
        }
    }
    else if (do_this == DEBUG_ACTION) {
        // use this for debugging only
        debug_action(this);
        player->query_depth(world);
    }
    else if (key >= SDLK_1 && key <= SDLK_9) {
        int to_index = key - SDLK_1;
        switch_current_item(to_index);
    }
    else if (key == SDLK_0) {
        switch_current_item(9);
    }
}

// Calls an action depending on key pressed
// Not all key presses may correspond to player so this method belongs here
void Game::key_callback(int key, int scancode, int action, int mods) {
    // http://www.glfw.org/docs/latest/input.html#input_keyboard
    // http://www.glfw.org/docs/latest/quick.html
    
    if (key == SDLK_DELETE or key == SDLK_p) {
        exit_game();
    }

    // if toggleable key
    if (key_toggled.count(key)) {
        if (action == SDL_KEYDOWN) {
            key_toggled[key] = true;
        }
        else if (action == SDL_KEYUP) {
            key_toggled[key] = false;
        }
    }
    else if (action == SDL_KEYDOWN) {
        // see if we're in any vehicle of any sort, then see if it'll accept the key i send
        SuperObject* mount = player->get_mount();
        if (mount) {
            Action do_this = key_to_action[key];
            if (mount->block_keyboard_callback(this, do_this, mount)) {
                return;
            }
        }
        if (bar->get_current()) {
            Action do_this = key_to_action[key];
            if (bar->get_current()->pushed(this, do_this)) {
                return;
            }
        }
        key_callback_default(key);
    }
}

void Game::mouse_move_callback(double xdiff, double ydiff) {
    if (in_game)
        player->update_direction(xdiff, ydiff);
}

void Game::mouse_button_callback(int button, int action, int mods) {
    if (action == SDL_MOUSEBUTTONDOWN) {
        Action do_this = mouse_to_action[button];
        if (in_game) {
            // see if we're in any vehicle of any sort, then see if it'll accept the mouse button i send
            SuperObject* mount = player->get_mount();
            if (mount) {
                if (mount->block_keyboard_callback(this, do_this, mount)) {
                    return;
                }
            }
            if (BlockTracing::show_item) {
                Entity* src = BlockTracing::selected->find_top_level_parent();
                if (BlockTracing::selected->block_mouse_callback(this, do_this, src)) {
                    return;
                }
            }
            if (bar->get_current()) {
                // using mouse
                if (bar->get_current()->clicked(this, do_this)) {
                    return;
                }
            }
            default_item->clicked(this, do_this);
        }
        else {
            double mx, my;
            display_get_cursor_position(&mx, &my);
            story->onclick((int)mx, (int)my, do_this);
        }
    }
}

void Game::scroll_callback(double xoffset, double yoffset) {
    if (!in_game) {
        if (yoffset != 0) {
            double mx, my;
            display_get_cursor_position(&mx, &my);
            story->scrolled((int)mx, (int)my, yoffset * 30);
        }
    }
    else {
        if (yoffset < 0) {
            bar->set_to_right();
        }
        else if (yoffset > 0) {
            bar->set_to_left();
        }
    }
}

int Game::load_game_data() {
    IODataObject read(get_path_to_game());
    if (read.read()) {
        printf("Game data missing\n");
        return 0;
    }
    
    int version = read.read_value<int>();
    printf("Loading game from version %d\n", version);
    printf("Current game version %d\n", VERSION);
    read.close();
    return 0;
}

int Game::save_game_data() {
    IODataObject write(get_path_to_game());
    if (write.save())
        return 1;
    
    write.save_value(VERSION);
    write.close();
    return 0;
}

Game::~Game() {
    printf("Cleaning up game\n");
    player->unmount(world);
    save_game_data();
    world->save_self();
    if (game_template)
        delete game_template;
    // player deleted as part of world...?
    delete world;
    delete story;
    delete bar;
    world_free_resources();
}

