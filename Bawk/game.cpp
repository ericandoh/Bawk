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

enum Action {
    MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT, MOVE_FORWARD, MOVE_BACKWARD, CONFIRM, CANCEL,
    MOVE_BLOCK_UP, MOVE_BLOCK_DOWN, MOVE_BLOCK_LEFT, MOVE_BLOCK_RIGHT, MOVE_BLOCK_FORWARD,
    MOVE_BLOCK_BACKWARD, CLICK_CREATE, CLICK_DESTROY
};

int toggleable_keys[] = {GLFW_KEY_SPACE,
                            GLFW_KEY_Z, GLFW_KEY_A, GLFW_KEY_D,
                            GLFW_KEY_W, GLFW_KEY_S};

std::map<int, Action> key_to_action;
std::map<int, Action> mouse_to_action;
std::map<int, bool> key_toggled;

// if the player moves more than 1 dimension away from previous location, update
float CHUNK_UPDATE_TRIGGER_DISTANCE = 16.0f;

// initializes all needed game variables. This should be called before render()
int Game::init() {
    world = new World("testworld");
    
    // load resources for the world
    if (world->load_resources()) {
        return -1;
    }
    
    initialize_vbo_for_widgets();
    
    // later separate loading the player and the world - this might get complicated!
    player = new Player();
    fvec3* player_pos = player->get_pos();
    last_player_pos = fvec3(player_pos->x, player_pos->y, player_pos->z);
    
    world->update_chunks(0, player_pos);
    
    // set key mappings
    key_to_action[GLFW_KEY_SPACE] = MOVE_UP;
    key_to_action[GLFW_KEY_Z] = MOVE_DOWN;
    key_to_action[GLFW_KEY_A] = MOVE_LEFT;
    key_to_action[GLFW_KEY_D] = MOVE_RIGHT;
    key_to_action[GLFW_KEY_W] = MOVE_FORWARD;
    key_to_action[GLFW_KEY_S] = MOVE_BACKWARD;
    
    key_to_action[GLFW_KEY_ENTER] = CONFIRM;
    key_to_action[GLFW_KEY_ESCAPE] = CANCEL;
    
    key_to_action[GLFW_KEY_O] = MOVE_BLOCK_UP;
    key_to_action[GLFW_KEY_L] = MOVE_BLOCK_DOWN;
    key_to_action[GLFW_KEY_LEFT] = MOVE_BLOCK_LEFT;
    key_to_action[GLFW_KEY_RIGHT] = MOVE_BLOCK_RIGHT;
    key_to_action[GLFW_KEY_UP] = MOVE_BLOCK_FORWARD;
    key_to_action[GLFW_KEY_DOWN] = MOVE_BLOCK_BACKWARD;
    
    mouse_to_action[GLFW_MOUSE_BUTTON_LEFT] = CLICK_DESTROY;
    mouse_to_action[GLFW_MOUSE_BUTTON_RIGHT] = CLICK_CREATE;
    
    for (unsigned int i = 0; i < sizeof(toggleable_keys); i++) {
        key_toggled[toggleable_keys[i]] = false;
    }
    
    place_into = 0;
    
    bar = new ItemBar(128*10, 128);
    
    bar->set_index(0);
    CursorItem* first = new CursorBlock(9);
    bar->set_current(first);
    
    bar->set_index(1);
    CursorSuperObject* second = new CursorSuperObject();
    second->set_block(0.0f, 0.0f, 0.0f, 2);
    second->set_block(0.0f, 1.0f, 0.0f, 2);
    second->set_block(0.0f, 1.0f, 1.0f, 2);
    bar->set_current(second);
    
    bar->set_index(0);
        
    return 0;
}
    
// Called in the main render loop. Pass the rendering to the appropriate entries
void Game::render() {
    // get transform from player's perspective
    fmat4* transform = player->set_camera();
    // render world in player's perspective
    world->render(transform);
    // get depth coordinates
    player->query_depth();
    // render current item based on those depth coordinates
    if (bar->get_current()) {
        bar->get_current()->render_and_position(transform);
    }
    
    // render the cursor
    player->render();
    
    // Render UI elements here
    // always render the item bar (for now)
    bar->render();
}

// runs one frame of the game
void Game::frame() {
    bool need_update = false;
    for (auto& key : key_toggled) {
        if (key.second) {
            bool this_key_need_update = true;
            Action do_this = key_to_action[key.first];
            switch (do_this) {
                case MOVE_UP:
                    player->move_up();
                    break;
                case MOVE_DOWN:
                    player->move_down();
                    break;
                case MOVE_LEFT:
                    player->move_left();
                    break;
                case MOVE_RIGHT:
                    player->move_right();
                    break;
                case MOVE_FORWARD:
                    player->move_forward();
                    break;
                case MOVE_BACKWARD:
                    player->move_backward();
                    break;
                default:
                    // do nothing
                    this_key_need_update = false;
                    ;
            }
            need_update = need_update | this_key_need_update;
        }
    }
    if (need_update)
        check_need_update();
}

float get_dst(fvec3* a, fvec3* b) {
    return sqrtf(powf(a->x - b->x, 2) + powf(a->y - b->y, 2) + powf(a->z - b->z, 2));
}

void Game::check_need_update() {
    fvec3* player_pos = player->get_pos();
    if (get_dst(&last_player_pos, player_pos) >= CHUNK_UPDATE_TRIGGER_DISTANCE) {
        world->update_chunks(&last_player_pos, player_pos);
        last_player_pos = fvec3(player_pos->x, player_pos->y, player_pos->z);
    }
}

void Game::switch_current_item(int to_index) {
    bar->set_index(to_index);
    placed_current_item = false;
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
        Action do_this = key_to_action[key];
        if (do_this == CONFIRM) {
            if (placed_current_item) {
                // this code is duplicated somewhere else!
                if (place_into) {
                    printf("Placing template into template!\n");
                }
                else {
                    printf("Placing template into world!\n");
                }
                // we need to place this object into the world
                // if we can't place this object, this should fail
                if (bar->get_current()->set_blocks(world, place_into)) {
                    // placing blocks was successful
                    placed_current_item = false;
                }
            }
            else if (place_into) {
                printf("Exporting template to current cursor!\n");
                // we're done making this template.
                // publish it to either the baseworld or to a new superobject
                // do we need to free? what if its already in inventory
                // let's not free this l8 on, since we still need to render if its in inventory
                // then l8 we can have a separate memory scheme for if this is in memory or not
                if (bar->get_current()) {
                    bar->get_current()->cleanup_all();
                }
                bar->set_current(create_from_template(world, place_into));
                delete place_into;
                place_into = 0;
            }
            else {
                printf("Making new template\n");
                // create a new place_info
                place_into = new TemporaryTemplate();
                // TODO do stuff to dim the world except all newly placed blocks
                // i.e. set a uniform variable to tune the intensity
            }
        }
        else if (do_this == CANCEL) {
            if (placed_current_item) {
                printf("Cancelling current template placing 1\n");
                placed_current_item = false;
            }
            else if (place_into) {
                // we're canceling making this template
                printf("Cancelling current template\n");
                place_into->unpublish(world);
            }
            else {
                printf("Escape pressed with no context. Exitting\n");
                exit_game();
            }
        }
        else if (do_this == MOVE_BLOCK_UP) {
            if (placed_current_item) {
                printf("Moving placed template\n");
                bar->get_current()->move_block(ivec3(0, 1, 0));
            }
        }
        else if (do_this == MOVE_BLOCK_DOWN) {
            if (placed_current_item) {
                printf("Moving placed template\n");
                bar->get_current()->move_block(ivec3(0, -1, 0));
            }
        }
        else if (do_this == MOVE_BLOCK_LEFT) {
            if (placed_current_item) {
                printf("Moving placed template\n");
                ivec3 left = player->get_rounded_left();
                bar->get_current()->move_block(ivec3(-left.x, 0, -left.z));
            }
        }
        else if (do_this == MOVE_BLOCK_RIGHT) {
            if (placed_current_item) {
                printf("Moving placed template\n");
                bar->get_current()->move_block(player->get_rounded_left());
            }
        }
        else if (do_this == MOVE_BLOCK_FORWARD) {
            if (placed_current_item) {
                printf("Moving placed template\n");
                bar->get_current()->move_block(player->get_rounded_forward());
            }
        }
        else if (do_this == MOVE_BLOCK_BACKWARD) {
            if (placed_current_item) {
                printf("Moving placed template\n");
                ivec3 forward = player->get_rounded_forward();
                bar->get_current()->move_block(ivec3(-forward.x, 0, -forward.z));
            }
        }
        else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_9) {
            int to_index = key - GLFW_KEY_1;
            switch_current_item(to_index);
        }
    }
}

void Game::mouse_move_callback(double xdiff, double ydiff) {
    player->update_direction(xdiff, ydiff);
}

void Game::mouse_button_callback(int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        Action do_this = mouse_to_action[button];
        if (do_this == CLICK_DESTROY) {
            if (placed_current_item) {
                printf("Cancelling current template placing2\n");
                placed_current_item = false;
                bar->get_current()->unlock();
            }
            else {
                // get block here instead of outside
                printf("Destroying block from world\n");
                ivec3 removed_at;
                if (world->kill_block(&removed_at)) {
                    if (place_into) {
                        printf("Destroying it from the template as well rofl\n");
                        place_into->remove_block(removed_at);
                    }
                }
            }
        }
        else if (do_this == CLICK_CREATE) {
            // get block here instead of outside
            if (placed_current_item) {
                if (place_into) {
                    printf("Placing template into template!\n");
                }
                else {
                    printf("Placing template into world!\n");
                }
                // we need to place this object into the world
                bar->get_current()->set_blocks(world, place_into);
                placed_current_item = false;
            }
            else if (bar->get_current()) {
                printf("Placing block\n");
                placed_current_item = bar->get_current()->place_blocks(world, place_into);
            }
        }
    }
}

void Game::scroll_callback(double xoffset, double yoffset) {
    if (yoffset < 0) {
        bar->set_to_right();
    }
    else if (yoffset > 0) {
        bar->set_to_left();
    }
}

// TODO if we scroll mouse, make sure to set placed_current_item to false
// use switch_current_item(index) to switch

Game::~Game() {
    printf("Cleaning up game\n");
    // cleanup assets
    // TODO make sure below works
    //for (auto i : cursor_items) {
    //    delete i;
    //}
    world->free_resources();
    clean_vbo_for_widgets();
    delete bar;
    delete place_into;
    delete player;
    delete world;
}

