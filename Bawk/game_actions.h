//
//  game_actions.h
//  Bawk
//
//  Created by Eric Oh on 11/12/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef Bawk_game_actions_h
#define Bawk_game_actions_h

enum Action {
    MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT, MOVE_FORWARD, MOVE_BACKWARD, CONFIRM, CANCEL,
    MOVE_BLOCK_UP, MOVE_BLOCK_DOWN, MOVE_BLOCK_LEFT, MOVE_BLOCK_RIGHT, MOVE_BLOCK_FORWARD,
    MOVE_BLOCK_BACKWARD, OPEN_INV, CLICK_CREATE, CLICK_DESTROY, SAVE_TEMPLATE, DEBUG_ACTION,
};

#endif
