//
//  engine_actions.h
//  Bawk
//
//  Created by Eric Oh on 2/18/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__engine_actions__
#define __Bawk__engine_actions__

#include <stdio.h>

enum EngineAction {
    SET_BLOCK, UPDATE_POS, UPDATE_ANGLE, MOUNT, UNMOUNT,
    TOTAL_ENGINE_ACTIONS
};

#endif /* defined(__Bawk__engine_actions__) */
