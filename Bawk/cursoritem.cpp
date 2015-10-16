//
//  cursoritem.cpp
//  Bawk
//
//  Created by Eric Oh on 10/6/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "cursoritem.h"

bool CursorItem::update_pointing_position(int* tx, int* ty, int* tz, bool nonempty) {
    ivec4 looking_at;
    if (!get_look_at(&looking_at)) {
        return false;
    }
    ivec3 bounds;
    get_bounds(&bounds);
    int mx = looking_at.x;
    int my = looking_at.y;
    int mz = looking_at.z;
    BlockOrientation face = static_cast<BlockOrientation>(looking_at.w);
    if (nonempty) {
        if(face == BlockOrientation::FRONT)
            mx++;
        if(face == BlockOrientation::BACK)
            mx-=bounds.x;
        if(face == BlockOrientation::TOP)
            my++;
        if(face == BlockOrientation::BOTTOM)
            my-=bounds.y;
        if(face == BlockOrientation::RIGHT)
            mz++;
        if(face == BlockOrientation::LEFT)
            mz-=bounds.z;
    }
    *tx = mx;
    *ty = my;
    *tz = mz;
    
    return true;
}

void CursorItem::unlock() {
    // does nothing
}