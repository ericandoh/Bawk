//
//  blockaction.h
//  Bawk
//
//  Created by Eric Oh on 10/25/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__blockaction__
#define __Bawk__blockaction__

#include <stdio.h>
#include <vector>
#include "block.h"
#include "world.h"
#include "game_actions.h"

class Game;

typedef bool (*block_mouse_callback_func)(Game* game, Entity* owner, block_type* blk, fvec3 rwc, Action button);
typedef bool (*block_keyboard_callback_func)(Game* game, Entity* owner, block_type* blk, ivec3 rwc, Action action);

// used by gameloader to get appropriate action functions
block_mouse_callback_func get_block_mouse_callback_for(std::string name);

block_keyboard_callback_func get_block_keyboard_callback_for(std::string name, std::vector<Action> &default_keymap);


#endif /* defined(__Bawk__blockaction__) */
