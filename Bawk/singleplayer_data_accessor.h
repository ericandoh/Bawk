//
//  singleplayer_data_accessor.h
//  Bawk
//
//  Created by Eric Oh on 5/29/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__singleplayer_data_accessor__
#define __Bawk__singleplayer_data_accessor__

#include <stdio.h>
#include "basic_types.h"
#include "client_accessor_model.h"

class SinglePlayerClientAccessor: public ClientAccessorModel {
public:
    bool register_player(uint32_t pid) override;
    void deregister_player(uint32_t pid) override;
    Player* get_player_data(uint32_t pid) override;
    Entity* get_entity_data(uint32_t eid) override;
    World* get_world() override;
};

#endif /* defined(__Bawk__singleplayer_data_accessor__) */
