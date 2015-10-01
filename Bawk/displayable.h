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

class Displayable {
public:
    virtual int init() = 0;
    virtual void render() = 0;
    virtual void key_callback(int key, int scancode, int action, int mods) = 0;
    virtual void mouse_move_callback(double xdiff, double ydiff) = 0;
    virtual void close() = 0;
};

#endif
