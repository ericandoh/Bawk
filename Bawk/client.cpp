//
//  client.cpp
//  Bawk
//
//  Created by Eric Oh on 2/16/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "client.h"

#include "world.h"

#include "importsdl.h"  // ?
#include "display.h"    // for UI element sizing

#include "cursoritem.h"
#include "cursorselector.h"
#include "gametemplate.h"
#include "cursorsuperobject.h"

// input/output
#include "game_input_receiver.h"
#include "game_window_holder.h"

#include "program.h"

#include "debug_action.h"

#include "blocktracer.h"

#include "client_accessor.h"
#include "common_accessor.h"
#include "engine_accessor.h"

void Client::init() {
    
    // --- PLAYER POS ---
    // update player pos and render scoped variables
    last_player_pos = get_player()->get_viewpoint();
    update_render(last_player_pos);
    
    // --- UI ---
    game_window_holder = new GameWindowHolder();
    
    // --- Game Inputs ---
    game_receiver = new GameInputReceiver();
    mount_receiver = new MountInputReceiver();
    
    client_receiver = InputMultiplexer();
    
    client_receiver.add_receiver(game_receiver);
    client_receiver.add_receiver(game_window_holder);
    client_receiver.add_receiver(mount_receiver);
    
    client_receiver.init();
    
    
    // --- SPRITE MANAGER ---
    sprite_manager = SpriteManager();
}

void Client::cleanup() {
    client_receiver.cleanup();
    delete game_receiver;
    delete mount_receiver;
    delete game_window_holder;
}

void Client::check_need_update() {
    fvec3 player_pos = get_player()->get_viewpoint();
    if (get_fvec3_distance(last_player_pos - player_pos) >= CHUNK_UPDATE_TRIGGER_DISTANCE) {
        get_world()->update_render(&player_pos);
        last_player_pos = player_pos;
    }
}

void Client::toggle_mount_ui(bool mounted) {
    if (mounted) {
        client_receiver.add_receiver(mount_receiver);
    }
    else {
        client_receiver.remove_receiver(mount_receiver);
    }
}

void Client::toggle_action(Action do_this, int ms) {
    get_engine()->react(do_this, get_player(), ms);
}

void Client::toggle_player_movement(double x, double y) {
    get_player()->update_direction(x, y);
}

void Client::update_render(fvec3 player_pos) {
    get_world()->update_render(&player_pos);
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
    if (get_item_bar()->get_current()) {
        get_item_bar()->get_current()->render_in_world(&transform);
    }
}

void Client::render_ui() {
    glDisable(GL_CULL_FACE);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_DEPTH_TEST);
    // render the cursor
    get_player()->render();
    client_receiver.render();
}

void Client::render() {
    // get transform from player's perspective
    get_player()->set_camera();
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
    get_world()->set_light_camera_for_lighting(get_player());
    
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof vertex, vertex, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(OGLAttr::lighting_shader.coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    
    // --- OTHER LIGHTING ---
    // render a light around the player for now
    get_player()->set_camera();
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    fvec3 player_viewpoint_pos = get_player()->get_viewpoint();
    
    fmat4 transform(1);
    get_world()->render_lights(&transform, player_viewpoint_pos);
    sprite_manager.render_lights(&transform, player_viewpoint_pos);
    if (get_item_bar()->get_current()) {
        get_item_bar()->get_current()->render_light_in_world(&transform, player_viewpoint_pos);
    }
}

void Client::render_shadows() {
    // get transform from light's perspective
    get_world()->set_light_camera(get_player());
    // only render geometry of world
    render_geometry();
    //fmat4 transform(1);
    //world->render_background(&transform, player);
}

// runs one frame of the game
void Client::frame(int ms) {
    check_need_update();
    
    // get depth coordinates
    get_player()->query_depth(get_world());
    sprite_manager.step(ms);
    if (get_item_bar()->get_current()) {
        get_item_bar()->get_current()->step(ms);
    }
}

bool Client::key_callback(Action do_this, int ms) {
    return client_receiver.key_callback(do_this, ms);
}

bool Client::mouse_move_callback(double xdiff, double ydiff) {
    return client_receiver.mouse_move_callback(xdiff, ydiff);
}

bool Client::mouse_clicked_callback(Action do_this) {
    return client_receiver.mouse_clicked_callback(do_this);
}

bool Client::mouse_clicking_callback(Action do_this, int ms) {
    return client_receiver.mouse_clicking_callback(do_this, ms);
}

bool Client::scroll_callback(double xoffset, double yoffset) {
    return client_receiver.scroll_callback(xoffset, yoffset);
}
