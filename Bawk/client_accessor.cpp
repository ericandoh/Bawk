//
//  client_accessor.cpp
//  Bawk
//
//  Created by Eric Oh on 2/18/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "client_accessor.h"

Client* current_client = 0;

void set_client(Client* client) {
    current_client = client;
}

Client* get_client() {
    return current_client;
}

Player* get_player() {
    return current_client->get_player();
}