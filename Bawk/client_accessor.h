//
//  client_accessor.h
//  Bawk
//
//  A class to easily access client-side singleton variables
//  Accesses both client and server calls. Server calls are
//  abstracted under client_accessor_model however
//
//  Created by Eric Oh on 2/18/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__client_accessor__
#define __Bawk__client_accessor__

#include <stdio.h>
#include "client.h"

class Player;
class Entity;
class ItemBar;
class MainInventoryWidget;

void set_client(Client* client);

void setup_for_singleplayer();
void setup_for_multiplayer();

// to be DEPRECATED
Client* get_client();

ItemBar* get_item_bar();
MainInventoryWidget* get_inventory_widget();
void toggle_mount(bool mounted);

Player* register_player(uint32_t pid);
void deregister_current_player();
Player* get_player();
Player* get_player_data(uint32_t pid);
Entity* get_entity_data(uint32_t eid);

// the client side accessor for world. accesses the world stored on client side
World* get_client_world();


// sync functions. TODO
// void block_change(block_type from, block_type to, SuperObject* on);
// void object_added2world(SuperObject* x);
// void object_removed4world(SuperObject* y);
// void object_added(Entity* x, SuperObject* y)
// void object_removed(Entity* x, SuperObject* y);
// void sync_world(...);    // receive updates from server or client and update with timestamps
// void sprite_event_received();  // make a sprite event happen somewhere
// things we'll send over server/client: synchronize positions, rotations, block content
// things we will NOT send over server/client: client actions? or should we? we could

// TODO memory leak, handle

#endif /* defined(__Bawk__client_accessor__) */
