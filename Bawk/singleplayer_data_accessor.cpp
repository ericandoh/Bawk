//
//  singleplayer_data_accessor.cpp
//  Bawk
//
//  Created by Eric Oh on 5/29/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "singleplayer_data_accessor.h"
#include "engine_accessor.h"
#include "player.h"
#include "world.h"

bool SinglePlayerClientAccessor::register_player(uint32_t pid) {
    get_engine()->register_player(pid);
    return true;
}

void SinglePlayerClientAccessor::deregister_player(uint32_t pid) {
    get_engine()->deregister_player(pid);
}

Player* SinglePlayerClientAccessor::get_player_data(uint32_t pid) {
    return get_engine()->get_player_from(pid);
}

Entity* SinglePlayerClientAccessor::get_entity_data(uint32_t eid) {
    // TODO fill
    return 0;
}

World* SinglePlayerClientAccessor::get_world() {
    return get_engine()->get_world();
}
