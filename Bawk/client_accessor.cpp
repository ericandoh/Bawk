//
//  client_accessor.cpp
//  Bawk
//
//  Created by Eric Oh on 2/18/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "client_accessor.h"
#include "client_accessor_model.h"
#include "singleplayer_data_accessor.h"
#include "game_window_holder.h"

Client* current_client = 0;
Player* current_player = 0;
ClientAccessorModel* client_accessor;

void set_client(Client* client) {
    current_client = client;
}

void setup_for_singleplayer() {
    client_accessor = new SinglePlayerClientAccessor();
}

void setup_for_multiplayer() {
    // TODO multiplayer not yet supported. build out a multiplayer accessor
    // that gets data over the network and has a caching layer.
    // this would be in a new file multiplayer_client_accessor.c
    printf("Multiplayer not yet supported");
    setup_for_singleplayer();
}

Client* get_client() {
    return current_client;
}

ItemBar* get_item_bar() {
    return current_client->game_window_holder->get_item_bar();
}

MainInventoryWidget* get_inventory_widget() {
    return current_client->game_window_holder->get_inventory_widget();
}

void toggle_mount(bool mounted) {
    current_client->toggle_mount_ui(mounted);
}

Player* register_player(uint32_t pid) {
    if (client_accessor->register_player(pid)) {
        current_player = get_player_data(pid);
        return current_player;
    }
    return 0;
}

void deregister_current_player() {
    if (get_player()->game_template) {
        delete get_player()->game_template;
    }
    client_accessor->deregister_player(get_player()->pid);
}

Player* get_player() {
    return current_player;
}

Player* get_player_data(uint32_t pid) {
    return client_accessor->get_player_data(pid);
}

Entity* get_entity_data(uint32_t eid) {
    return client_accessor->get_entity_data(eid);
}

World* get_client_world() {
    return client_accessor->get_world();
}
