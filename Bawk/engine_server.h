//
//  engine_server.h
//  Bawk
//
//  Created by Eric Oh on 2/18/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__engine_server__
#define __Bawk__engine_server__

#include <stdio.h>
#include "engine.h"

class EngineServer: public Engine {
    // list of entities that need to be pushed for update
    std::vector<Entity*> synchronize_list;
    // every time the world needs synch again, this is updated
    // kind of like a version, each action that needs to be pushed is linked with this #
    // can this rollback?
    
public:
    void synchronize() override;
    void register_action(Entity* ent, EngineAction action) override;
};

#endif /* defined(__Bawk__engine_server__) */
