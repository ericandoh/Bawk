//
//  cursorscantool.h
//  Bawk
//
//  Created by Eric Oh on 1/2/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__cursorscantool__
#define __Bawk__cursorscantool__

#include <stdio.h>
#include "cursoritem.h"
#include "block_orientation.h"

enum ScanStages {
    SETTING_LOWER, SETTING_UPPER, EXTENDING, EXTENDED, SELECTED
};

class CursorScanTool: public CursorItem {
    ScanStages current_stage;
    ivec3 lower, upper, extended;
    int_bounding_box box;
    int_bounding_box extending_box;
    BlockOrientation player_orientation;
public:
    // a tool to scan blocks. very useful!
    
    // --- CursorScanTool
    CursorScanTool(CursorItemInfo* i);
    
    // --- CursorItem ---
    //virtual void init();
    void reset() override;
    
    // behaviour when this cursor item is clicked
    bool clicked(Game* game, Action mouse) override;
    bool pushed(Game* game, Action key) override;
    
    // behaviour when this cursor item is entered
    bool confirmed(Game* game) override;
    bool canceled(Game* game) override;
    bool handle_movement(ivec3 dir) override;
    
    // behaviour to update at each step
    void step(Game* game, int ms) override;
    
    // render the item with transform in a small box or whatnot
    void render_item() override;
    // render the item in the world
    void render_in_world(fmat4* transform) override;
};

#endif /* defined(__Bawk__cursorscantool__) */
