//
//  An abstract class outlining behaviours of a displayable object
//  - Functions that are called when it is made, rendered, destroyed
//  - Also captures input
//  -
//
//  Used by:
//  - display.h, program.h
//  - Overridden by game.h
//
//  Uses:
//  -
//  -
//  -
//
//  Notes
//  -
//  -
//
//  Created by Eric Oh on 9/30/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef Bawk_displayable_h
#define Bawk_displayable_h

#include "basic_types.h"
#include "game_actions.h"

class Displayable {
public:
    virtual void init() EMPTY_FUNCTION;
    virtual void cleanup() EMPTY_FUNCTION;
    
    virtual void render() EMPTY_FUNCTION;
    virtual void render_lights() EMPTY_FUNCTION;
    virtual void render_shadows() EMPTY_FUNCTION;
    
    virtual void frame(int ms) EMPTY_FUNCTION;
    
    virtual bool key_callback(Action do_this, int ms) EMPTY_BOOL_FUNCTION;
    virtual bool mouse_move_callback(double xdiff, double ydiff) EMPTY_BOOL_FUNCTION;
    virtual bool mouse_clicked_callback(Action do_this) EMPTY_BOOL_FUNCTION;
    virtual bool mouse_clicking_callback(Action do_this, int ms) EMPTY_BOOL_FUNCTION;
    virtual bool scroll_callback(double xdiff, double ydiff) EMPTY_BOOL_FUNCTION;
};


#endif
