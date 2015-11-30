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

// deleteme
#include "cursormodelobject.h"

// THIS IS USEDF OR LIGHTING< MOVE TO A CUSTOM LIGHT RENDERING METHOD
// TODO FROG
#include <glm/gtc/matrix_transform.hpp>

int toggleable_keys[] = {GLFW_KEY_SPACE,
                            GLFW_KEY_C, GLFW_KEY_A, GLFW_KEY_D,
                            GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_TAB, GLFW_KEY_LEFT_SHIFT,
                            GLFW_KEY_Q, GLFW_KEY_E};

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
    last_player_pos = player->pos;
    
    world->add_player(player);
    world->update_chunks(&(player->pos));
    
    // set key mappings
    // movement key mappings
    key_to_action[GLFW_KEY_SPACE] = MOVE_UP;
    key_to_action[GLFW_KEY_C] = MOVE_DOWN;
    key_to_action[GLFW_KEY_A] = MOVE_LEFT;
    key_to_action[GLFW_KEY_D] = MOVE_RIGHT;
    key_to_action[GLFW_KEY_W] = MOVE_FORWARD;
    key_to_action[GLFW_KEY_S] = MOVE_BACKWARD;
    
    key_to_action[GLFW_KEY_TAB] = PITCH_UP;
    key_to_action[GLFW_KEY_LEFT_SHIFT] = PITCH_DOWN;
    key_to_action[GLFW_KEY_Q] = ROLL_LEFT;
    key_to_action[GLFW_KEY_E] = ROLL_RIGHT;
    
    key_to_action[GLFW_KEY_ENTER] = CONFIRM;
    key_to_action[GLFW_KEY_ESCAPE] = CANCEL;
    
    key_to_action[GLFW_KEY_PERIOD] = MOVE_BLOCK_UP;
    key_to_action[GLFW_KEY_SLASH] = MOVE_BLOCK_DOWN;
    key_to_action[GLFW_KEY_LEFT] = MOVE_BLOCK_LEFT;
    key_to_action[GLFW_KEY_RIGHT] = MOVE_BLOCK_RIGHT;
    key_to_action[GLFW_KEY_UP] = MOVE_BLOCK_FORWARD;
    key_to_action[GLFW_KEY_DOWN] = MOVE_BLOCK_BACKWARD;
    key_to_action[GLFW_KEY_SEMICOLON] = MOVE_BLOCK_ROTATE;
    
    key_to_action[GLFW_KEY_I] = OPEN_INV;
    key_to_action[GLFW_KEY_B] = SAVE_TEMPLATE;
    
    key_to_action[GLFW_KEY_M] = DEBUG_ACTION;
    
    key_to_action[GLFW_KEY_Y] = MOUNTING;
    
    mouse_to_action[GLFW_MOUSE_BUTTON_LEFT] = CLICK_DESTROY;
    mouse_to_action[GLFW_MOUSE_BUTTON_RIGHT] = CLICK_CREATE;
    
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
    
    bar->set_index(0);
    
    // for testing, TODO delete
    bar->set_current(new CursorModelObject(1));
        
    return 0;
}
    
// Called in the main render loop. Pass the rendering to the appropriate entries
void Game::render() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_CULL_FACE);
    // get transform from player's perspective
    fmat4* transform = player->set_camera();
    // render world in player's perspective
    
    // TODO fix this hack
    if (game_template) {
        set_shader_intensity(0.3f);
    }
    else {
        set_shader_intensity(0.6f);
    }
    
    world->render(transform);
    // get depth coordinates
    player->query_depth(world);
    // render current item based on those depth coordinates
    glDisable(GL_CULL_FACE);
    if (bar->get_current()) {
        bar->get_current()->render_in_world(transform);
    }
    
    glDisable(GL_POLYGON_OFFSET_FILL);
    
    glDisable(GL_DEPTH_TEST);
    // render the cursor
    player->render();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);
    // Render UI elements here
    // always render the item bar (for now)
    story->render();
}

void Game::render_lights() {
    // render a light around the player for now
    
    fmat4* transform = player->set_camera();
    
    // Render a box around the block we are pointing at
    GLbyte box[36][3] = {
        {0,0,0},
        {0,0,1},
        {0,1,0},
        
        {0,1,0},
        {0,0,1},
        {0,1,1},
        
        {1,0,0},
        {1,1,0},
        {1,0,1},
        
        {1,1,0},
        {1,1,1},
        {1,0,1},
        
        {0,0,0},
        {1,0,0},
        {0,0,1},
        
        {0,0,1},
        {1,0,0},
        {1,0,1},
        
        {0,1,0},
        {0,1,1},
        {1,1,0},
        
        {0,1,1},
        {1,1,1},
        {1,1,0},
        
        {0,0,0},
        {1,0,0},
        {0,1,0},
        
        {0,1,0},
        {1,0,0},
        {1,1,0},
        
        {0,0,1},
        {0,1,1},
        {1,0,1},
        
        {0,1,1},
        {1,1,1},
        {1,0,1},
    };
    for (int i = 0; i < 36; i++) {
        for (int j = 0; j < 3; j++) {
            box[i][j] *= 10;
        }
    }
    
    fmat4 view = glm::translate(fmat4(1), player->pos);
    fmat4 mvp = *transform * view;
    set_lighting_transform_matrix(mvp);
    set_lighting_block_draw_mode(1);
    set_lighting_val(player->pos);
    
    glBindBuffer(GL_ARRAY_BUFFER, get_vertex_attribute_vbo());
    glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(lighting_coord, 3, GL_BYTE, GL_FALSE, 0, 0);
    
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

// runs one frame of the game
void Game::frame() {
    for (auto& key : key_toggled) {
        if (key.second) {
            Action do_this = key_to_action[key.first];
            // see if we're in any vehicle of any sort, then see if it'll accept the key i send
            SuperObject* mount = player->get_mount();
            if (mount) {
                if (mount->block_keyboard_callback(this, do_this)) {
                    continue;
                }
            }
            switch (do_this) {
                case MOVE_UP:
                    player->move_up(5.0f);
                    break;
                case MOVE_DOWN:
                    player->move_down(5.0f);
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
    check_need_update();
    world->step();
}

float get_dst(fvec3* a, fvec3* b) {
    return sqrtf(powf(a->x - b->x, 2) + powf(a->y - b->y, 2) + powf(a->z - b->z, 2));
}

void Game::check_need_update() {
    fvec3 player_pos = player->pos;
    if (get_dst(&last_player_pos, &player_pos) >= CHUNK_UPDATE_TRIGGER_DISTANCE) {
        world->update_chunks(&player_pos);
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
            // we're done making this template.
            // publish it to either the baseworld or to a new superobject
            // do we need to free? what if its already in inventory
            // let's not free this l8 on, since we still need to render if its in inventory
            // then l8 we can have a separate memory scheme for if this is in memory or not
            bar->set_current(game_template->create_from_template(player, world));
            GameTemplate* reference = game_template;
            game_template = 0;
            reference->publish(this);
            delete reference;
            game_template = 0;
        }
        else {
            printf("Making new template\n");
            // create a new place_info
            game_template = new GameTemplate();
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
    else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_9) {
        int to_index = key - GLFW_KEY_1;
        switch_current_item(to_index);
    }
    else if (key == GLFW_KEY_0) {
        switch_current_item(9);
    }
}

void Game::mouse_callback_default(Action key, Entity* on) {
    if (key == CLICK_DESTROY) {
        // destroy blocks, for all click destroy commands
        world->break_block();
    }
    else if (bar->get_current()) {
        bar->get_current()->clicked(this, key, on);
    }
}


// Calls an action depending on key pressed
// Not all key presses may correspond to player so this method belongs here
void Game::key_callback(int key, int scancode, int action, int mods) {
    // http://www.glfw.org/docs/latest/input.html#input_keyboard
    // http://www.glfw.org/docs/latest/quick.html
    
    if (key == GLFW_KEY_DELETE or key == GLFW_KEY_P) {
        exit_game();
    }

    // if toggleable key
    if (key_toggled.count(key)) {
        if (action == GLFW_PRESS) {
            key_toggled[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            key_toggled[key] = false;
        }
    }
    else if (action == GLFW_PRESS) {
        // see if we're in any vehicle of any sort, then see if it'll accept the key i send
        SuperObject* mount = player->get_mount();
        if (mount) {
            Action do_this = key_to_action[key];
            if (mount->block_keyboard_callback(this, do_this)) {
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
    if (action == GLFW_PRESS) {
        if (in_game) {
            // see if we're in any vehicle of any sort, then see if it'll accept the mouse button i send
            SuperObject* mount = player->get_mount();
            Action do_this = mouse_to_action[button];
            if (mount) {
                if (mount->block_keyboard_callback(this, do_this)) {
                    return;
                }
            }
            Entity* src = get_look_at();
            if (src) {
                if (src->block_mouse_callback(this, do_this)) {
                    return;
                }
                else {
                    // using cursor to click
                    mouse_callback_default(do_this, src);
                }
            }
        }
        else {
            double mx, my;
            display_get_cursor_position(&mx, &my);
            story->onclick((int)mx, (int)my, button);
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
    world->remove_self();
    if (game_template)
        delete game_template;
    // player deleted as part of world...?
    delete world;
    delete story;
    delete bar;
    world_free_resources();
}

