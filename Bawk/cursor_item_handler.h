//
//  cursor_item_handler.h
//  Bawk
//
//  Created by Eric Oh on 5/29/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__cursor_item_handler__
#define __Bawk__cursor_item_handler__

#include <stdio.h>
#include "displayable.h"

class ItemBar;

class CursorItemHandler: public Displayable {
public:
    // --- Displayable ---
    void frame(int ms) override;
    
    bool key_callback(Action do_this, int ms) override;
    bool mouse_clicked_callback(Action do_this) override;
    bool mouse_clicking_callback(Action do_this, int ms) override;
};

#endif /* defined(__Bawk__cursor_item_handler__) */
