//
//  client_accessor_model.h
//  Bawk
//
//  Created by Eric Oh on 5/29/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef Bawk_client_accessor_model_h
#define Bawk_client_accessor_model_h

#include <stdio.h>

class Player;
class Entity;
class World;

class ClientAccessorModel {
public:
    virtual bool register_player(uint32_t pid) = 0;
    virtual void deregister_player(uint32_t pid) = 0;
    virtual Player* get_player_data(uint32_t pid) = 0;
    virtual Entity* get_entity_data(uint32_t eid) = 0;
    virtual World* get_world() = 0;
};

#endif
