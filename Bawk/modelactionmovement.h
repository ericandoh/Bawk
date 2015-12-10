//
//  modelactionmovement.h
//  Bawk
//
//  Created by Eric Oh on 12/9/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__modelactionmovement__
#define __Bawk__modelactionmovement__

#include <stdio.h>
#include "blockaction.h"

bool model_forward_callback(Game* game, Entity* owner, Entity* piece);
bool model_backward_callback(Game* game, Entity* owner, Entity* piece);
bool model_turn_left_callback(Game* game, Entity* owner, Entity* piece);
bool model_turn_right_callback(Game* game, Entity* owner, Entity* piece);
bool model_up_callback(Game* game, Entity* owner, Entity* piece);
bool model_down_callback(Game* game, Entity* owner, Entity* piece);
bool model_pitch_up_callback(Game* game, Entity* owner, Entity* piece);
bool model_pitch_down_callback(Game* game, Entity* owner, Entity* piece);
bool model_roll_left_callback(Game* game, Entity* owner, Entity* piece);
bool model_roll_right_callback(Game* game, Entity* owner, Entity* piece);

#endif /* defined(__Bawk__modelactionmovement__) */
