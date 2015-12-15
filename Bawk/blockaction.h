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
#include <map>
#include "block.h"
#include "game_actions.h"

class Game;
class Entity;

// callback functions for blocks
// @param owner: the top level object
// @param rwc: the rwc position
typedef bool (*block_callback_func)(Game* game, Entity* owner, block_type* blk, ivec3 rwc);

// used by gameloader to get appropriate action functions
void get_block_mouse_callback_for(std::string name, std::map<Action, block_callback_func> &action_map);
void get_block_keyboard_callback_for(std::string name, std::map<Action, block_callback_func> &default_keymap);

#endif /* defined(__Bawk__blockaction__) */
