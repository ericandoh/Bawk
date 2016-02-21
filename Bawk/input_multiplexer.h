//
//  input_multiplexer.h
//  Bawk
//
//  Created by Eric Oh on 2/19/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__input_multiplexer__
#define __Bawk__input_multiplexer__

#include <stdio.h>
#include <vector>
#include "input_receiver.h"

class InputMultiplexer: public InputReceiver {
    std::vector<InputReceiver*> receivers;
public:
    
    void add_receiver(InputReceiver* r);
    void remove_receiver(InputReceiver* r);
    void pop_receiver();
    
    // --- InputReceiver ---
    bool key_callback(Action do_this, int ms) override;
    bool mouse_move_callback(double xdiff, double ydiff) override;
    bool mouse_clicked_callback(Action do_this) override;
    bool mouse_clicking_callback(Action do_this, int ms) override;
    bool scroll_callback(double xdiff, double ydiff) override;
};

#endif /* defined(__Bawk__input_multiplexer__) */
