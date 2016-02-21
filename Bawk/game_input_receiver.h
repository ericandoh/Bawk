//
//  game_input_receiver.h
//  Bawk
//
//  Created by Eric Oh on 2/19/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__game_input_receiver__
#define __Bawk__game_input_receiver__

#include <stdio.h>
#include "input_receiver.h"

class GameInputReceiver: public InputReceiver {
protected:
    float get_player_speed(int ms);
public:
    bool key_callback(Action do_this, int ms) override;
    bool mouse_move_callback(double xdiff, double ydiff) override;
    bool mouse_clicked_callback(Action do_this) override;
    bool mouse_clicking_callback(Action do_this, int ms) override;
    bool scroll_callback(double xdiff, double ydiff) override;
};

#endif /* defined(__Bawk__game_input_receiver__) */
