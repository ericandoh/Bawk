//
//  client.cpp
//  Bawk
//
//  Created by Eric Oh on 2/16/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "client.h"

#include "world.h"

#include "importsdl.h"
#include "display.h"    // for UI element sizing

#include "cursoritem.h"
#include "cursorselector.h"
#include "gametemplate.h"
#include "cursorsuperobject.h"

// input/output
#include "game_input_receiver.h"

#include "program.h"

#include "debug_action.h"

#include "blocktracer.h"

namespace UIValues {
    int toggleable_keys[] = {SDLK_SPACE,
        SDLK_c, SDLK_a, SDLK_d,
        SDLK_w, SDLK_s, SDLK_TAB, SDLK_LSHIFT,
        SDLK_q, SDLK_e};
    
    std::map<int, Action> key_to_action;
    std::map<int, Action> mouse_to_action;
    std::map<int, bool> key_toggled;
    std::map<int, bool> mouse_toggled;
}

using namespace UIValues;

int Client::init(uint32_t p) {
    pid = p;
    
    // --- INPUT SETUP ---
    
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
    key_to_action[SDLK_v] = TOGGLE_VIEWPOINT;
    
    key_to_action[SDLK_m] = DEBUG_ACTION;
    
    key_to_action[SDLK_y] = MOUNTING;
    
    key_to_action[SDLK_DELETE] = DELETE;
    
    // is there a less stupid way to do this lol
    key_to_action[SDLK_1] = KEY0;
    key_to_action[SDLK_2] = KEY1;
    key_to_action[SDLK_3] = KEY2;
    key_to_action[SDLK_4] = KEY3;
    key_to_action[SDLK_5] = KEY4;
    key_to_action[SDLK_6] = KEY5;
    key_to_action[SDLK_7] = KEY6;
    key_to_action[SDLK_8] = KEY7;
    key_to_action[SDLK_9] = KEY8;
    key_to_action[SDLK_0] = KEY9;
    
    
    mouse_to_action[SDL_BUTTON_LEFT] = CLICK_MAIN;
    mouse_to_action[SDL_BUTTON_RIGHT] = CLICK_SECONDARY;
    
    for (unsigned int i = 0; i < sizeof(toggleable_keys); i++) {
        key_toggled[toggleable_keys[i]] = false;
    }
    for (auto &i: mouse_to_action) {
        mouse_toggled[i.first] = false;
    }
    
    // --- SERVER LINKING ---
    // later separate loading the player and the world - this might get complicated!
    // request player info from the game
    player = register_player(pid);
    last_player_pos = player->get_viewpoint();
    update_render(last_player_pos);
    
    // --- UI ---
    int width, height;
    get_window_size(&width, &height);
    story = new ParentWidget(0, 0, width, height);
    int bar_width = width * 7 / 100;
    // TODO is this okay??? it should be...
    bar = new ItemBar(player->inventory, bar_width*10, bar_width);
    story->add_child(bar);
    
    // --- Input/Output ---
    client_receiver = InputMultiplexer();
    
    GameInputReceiver* game_receiver = new GameInputReceiver();
    mount_receiver = new MountInputReceiver();
    
    client_receiver.add_receiver(game_receiver);
    client_receiver.add_receiver(bar);
    client_receiver.add_receiver(story);
    
    inventory_ui = new MainInventoryWidget(bar, player->inventory, width / 2, height / 2);
    
    bar->set_index(3);
    
    default_item = new CursorSelector(0);
    
    ui_open = false;
    
    // --- SPRITE MANAGER ---
    sprite_manager = SpriteManager();
    
    return 0;
}

void Client::cleanup() {
    deregister_player(player->getID());
    delete default_item;
    delete story;
    delete bar;
}

Player* Client::get_player() {
    return player;
}

void Client::check_need_update() {
    fvec3 player_pos = player->get_viewpoint();
    if (get_fvec3_distance(last_player_pos - player_pos) >= CHUNK_UPDATE_TRIGGER_DISTANCE) {
        get_world()->update_render(&player_pos);
        last_player_pos = player_pos;
    }
}

void Client::set_first_available(CursorItem* obj) {
    bar->set_first_available(obj);
}

void Client::toggle_mount_ui(bool mounted) {
    if (mounted) {
        client_receiver.add_receiver(mount_receiver);
    }
    else {
        client_receiver.remove_receiver(mount_receiver);
    }
}

// Called in the main render loop. Pass the rendering to the appropriate entries
void Client::render_geometry() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_CULL_FACE);
    // render world in player's perspective
    OGLAttr::current_shader->set_shader_intensity(1.0f);
    
    fmat4 transform(1);
    get_world()->render(&transform);
    if (bar->get_current()) {
        bar->get_current()->render_in_world(&transform);
    }
}

void Client::render_ui() {
    glDisable(GL_CULL_FACE);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_DEPTH_TEST);
    // render the cursor
    player->render();
    story->render();
}

void Client::render() {
    // get transform from player's perspective
    player->set_camera();
    OGLAttr::current_shader->set_enable_shadows(true);
    render_geometry();
    OGLAttr::current_shader->set_enable_shadows(false);
    fmat4 transform(1);
    get_world()->render_background(&transform);
    sprite_manager.render(&transform);
    render_ui();
}

void Client::render_lights() {
    
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
    get_world()->set_light_camera_for_lighting(player);
    
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof vertex, vertex, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(OGLAttr::lighting_shader.coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    
    // --- OTHER LIGHTING ---
    // render a light around the player for now
    player->set_camera();
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    fvec3 player_viewpoint_pos = player->get_viewpoint();
    
    fmat4 transform(1);
    get_world()->render_lights(&transform, player_viewpoint_pos);
    sprite_manager.render_lights(&transform, player_viewpoint_pos);
    if (bar->get_current()) {
        bar->get_current()->render_light_in_world(&transform, player_viewpoint_pos);
    }
}

void Client::render_shadows() {
    // get transform from light's perspective
    get_world()->set_light_camera(player);
    // only render geometry of world
    render_geometry();
    //fmat4 transform(1);
    //world->render_background(&transform, player);
}

// runs one frame of the game
void Client::frame(int ms) {
    for (auto& key : key_toggled) {
        if (key.second) {
            Action do_this = key_to_action[key.first];
            
            client_receiver.key_callback(do_this, ms);
            //toggle_action(do_this, ms);
        }
    }
    
    for (auto &key: mouse_toggled) {
        if (key.second) {
            Action do_this = mouse_to_action[key.first];
            
            client_receiver.mouse_clicking_callback(do_this, ms);
            //toggle_action(do_this, ms);
        }
    }
    check_need_update();
    
    // get depth coordinates
    player->query_depth(get_world());
    sprite_manager.step(ms);
    if (bar->get_current()) {
        bar->get_current()->step(ms);
    }
}

void Client::key_callback(int key, int scancode, int action, int mods) {
    if (key == SDLK_DELETE or key == SDLK_p) {
        exit_game();
        return;
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
        Action do_this = key_to_action[key];
        client_receiver.key_callback(do_this, 0);
    }
}

void Client::mouse_move_callback(double xdiff, double ydiff) {
    client_receiver.mouse_move_callback(xdiff, ydiff);
}

void Client::mouse_button_callback(int button, int action, int mods) {
    if (action == SDL_MOUSEBUTTONDOWN) {
        Action do_this = mouse_to_action[button];
        mouse_toggled[button] = true;
        
        client_receiver.mouse_clicked_callback(do_this);
    }
    else if (action == SDL_MOUSEBUTTONUP) {
        //Action do_this = mouse_to_action[button];
        mouse_toggled[button] = false;
    }
}

void Client::scroll_callback(double xoffset, double yoffset) {
    client_receiver.scroll_callback(xoffset, yoffset);
}
