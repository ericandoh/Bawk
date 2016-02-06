//
//  cursoritem_loader.h
//  Bawk
//
//  Created by Eric Oh on 2/5/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__cursoritem_loader__
#define __Bawk__cursoritem_loader__

#include <stdio.h>
#include "basic_types.h"

class CursorItem;
class CursorItemInfo;
class Player;

enum CursorType {
    CURSOR_DEFAULT,
    CURSOR_BLOCK,
    CURSOR_MODEL,
    CURSOR_SUPEROBJECT,
    CURSOR_WEAPON,
    CURSOR_SCANNER,
    CURSOR_DRILL,
    CURSOR_SELECTOR
};

CursorItem* get_cursor_item_from(CursorItemInfo* info);
CursorItem* copy_cursor_item(Player* player, CursorItem* src);
void delete_cursor_item_from_memory(CursorItem* entity);

#endif /* defined(__Bawk__cursoritem_loader__) */
