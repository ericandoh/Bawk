//
//  engine.cpp
//  Bawk
//
//  Created by Eric Oh on 2/16/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "engine.h"
#include "settings.h"

#include "block_loader.h"

#include "block_orientation.h"
#include "game_info_loader.h"

#include "world.h"
#include "player.h"

int Engine::start(std::string world_name) {
    if (load_game_data()) {
        return 1;
    }
    
    // initialize some rotation pre-calculations
    init_orientation_rotation_calculations();
    
    // load in game info
    load_game_info();
    
    world = new World(world_name);
    
    return 0;
}

void Engine::stop() {
    free_game_info();
    world->save_self();
    for (Player* player: players) {
        player->save_selfs();
    }
    delete world;
    world_free_resources();
}

int Engine::load_game_data() {
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

int Engine::save_game_data() {
    IODataObject write(get_path_to_game());
    if (write.save())
    return 1;
    
    write.save_value(VERSION);
    write.close();
    return 0;
}

// runs one frame of the game
void Engine::step(int ms) {
    world->step(ms);
}

void Engine::react(Action do_this, Player* player, int ms) {
    // TODO THIS IS UNUSED
    printf("frog\n");
    bool actioned = false;
    SuperObject* mount = player->get_mount();
    if (mount) {
        // if mounted, always capture all key movements even if not valid
        // this is because all the other below default options should be DISABLE when mounted
        if (mount->block_keyboard_callback(player, do_this, mount, ms)) {
            actioned = true;
        }
    }
    if (!actioned) {
        float speed = player->weight * 5.0f * convert_milli_to_sec(ms);
        switch (do_this) {
            case MOVE_UP: {
                player->move_up_flat(speed);
                break;
            }
            case MOVE_DOWN: {
                player->move_down_flat(speed);
                break;
            }
            case MOVE_LEFT: {
                player->move_left(speed);
                break;
            }
            case MOVE_RIGHT: {
                player->move_right(speed);
                break;
            }
            case MOVE_FORWARD: {
                player->move_forward_flat(speed);
                break;
            }
            case MOVE_BACKWARD: {
                player->move_backward_flat(speed);
                break;
            }
            default: {
                // do nothing
                ;
            }
            
        }
    }
}

World* Engine::get_world() {
    return world;
}

Player* Engine::get_player_from(uint32_t pid) {
    for (Player* player: players) {
        if (player->getID() == pid) {
            return player;
        }
    }
    return 0;
}



