//
//  input_receiver.h
//  Bawk
//
//  Abstract class that takes in input
//
//  Created by Eric Oh on 2/19/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__input_receiver__
#define __Bawk__input_receiver__

#include <stdio.h>
#include "game_actions.h"
#include "basic_types.h"

class InputReceiver {
public:
    virtual bool key_callback(Action do_this, int ms) EMPTY_BOOL_FUNCTION;
    virtual bool mouse_move_callback(double xdiff, double ydiff) EMPTY_BOOL_FUNCTION;
    virtual bool mouse_clicked_callback(Action do_this) EMPTY_BOOL_FUNCTION;
    virtual bool mouse_clicking_callback(Action do_this, int ms) EMPTY_BOOL_FUNCTION;
    virtual bool scroll_callback(double xdiff, double ydiff) EMPTY_BOOL_FUNCTION;
};

#endif /* defined(__Bawk__input_receiver__) */
