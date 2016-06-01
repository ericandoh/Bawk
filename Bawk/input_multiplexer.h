//
//  input_multiplexer.h
//  Bawk
//
//  Created by Eric Oh on 2/19/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

// DEPRECATED

#ifndef __Bawk__input_multiplexer__
#define __Bawk__input_multiplexer__

#include <stdio.h>
#include <vector>
#include "displayable.h"

class InputMultiplexer: public Displayable {
    std::vector<Displayable*> receivers;
public:
    
    void add_receiver(Displayable* r);
    void remove_receiver(Displayable* r);
    void pop_receiver();
    
    // --- Displayable ---
    virtual void init() override;
    virtual void cleanup() override;
    
    virtual void render() override;
    // the below 2 might be unused...
    virtual void render_lights() override;
    virtual void render_shadows() override;
    
    virtual void frame(int ms) override;
    
    bool key_callback(Action do_this, int ms) override;
    bool mouse_move_callback(double xdiff, double ydiff) override;
    bool mouse_clicked_callback(Action do_this) override;
    bool mouse_clicking_callback(Action do_this, int ms) override;
    bool scroll_callback(double xdiff, double ydiff) override;
};


#endif /* defined(__Bawk__input_multiplexer__) */
