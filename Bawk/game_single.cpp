//
//  game_single.cpp
//  Bawk
//
//  Created by Eric Oh on 2/16/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "game_single.h"

#include "player.h"
#include "gametemplate.h"
#include "common_accessor.h"

Player* GameSP::register_player(uint32_t pid) {
    Player* player = get_player_from(pid);
    if (player) {
        return player;
    }
    // create the player for the game
    player = new Player(pid);
    get_world()->add_player(player);
    // set its position at spawn
    // TODO ^
    return player;
}

void GameSP::deregister_player(uint32_t pid) {
    player->unmount();
    if (player->game_template) {
        delete player->game_template;
    }
    player->save_selfs();
}

void GameSP::update_render(fvec3 player_pos) {
    world->update_render(&player_pos);
}

World* GameSP::get_world() {
    return world;
}

void GameSP::toggle_action(Action do_this) {
    react(do_this, player);
}

void GameSP::toggle_player_movement(double x, double y) {
    player->update_direction(x, y);
}