//
//  cursoritem_loader.cpp
//  Bawk
//
//  Created by Eric Oh on 2/5/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#include "cursoritem_loader.h"
#include "entity_loader.h"

#include "cursoritem.h"
#include "cursorblock.h"
#include "cursormodelobject.h"
#include "cursorsuperobject.h"
#include "cursorweapon.h"
#include "cursorscantool.h"
#include "cursordrill.h"
#include "cursorselector.h"

CursorItem* get_cursor_item_from(CursorItemInfo* info) {
    if (!info)
        return 0;
    CursorType type = info->type;
    
    switch (type) {
        case CursorType::CURSOR_DEFAULT:
            return 0;
        case CursorType::CURSOR_BLOCK:
            return new CursorBlock(info);
        case CursorType::CURSOR_MODEL:
            return new CursorModelObject(info);
        case CursorType::CURSOR_SUPEROBJECT:
        {
            CursorSuperObject* obj = new CursorSuperObject(info);
            obj->init();
            return obj;
        }
        case CursorType::CURSOR_WEAPON:
            return new CursorWeapon(info);
        case CursorType::CURSOR_SCANNER:
            return new CursorScanTool(info);
        case CursorType::CURSOR_DRILL:
            return new CursorDrill(info);
        case CursorType::CURSOR_SELECTOR:
            return new CursorSelector(info);
        default:
            return 0;
    }
}

CursorItem* copy_cursor_item(Player* player, CursorItem* src) {
    // TOFU not yet implemented
    printf("Copying cursor items not yet implemented, sorry!\n");
    return 0;
}

void delete_cursor_item_from_memory(CursorItem* entity) {
    // TODO do not delete cursoritems from this path
    
    CursorType type = entity->info->type;
    
    if (type == CursorType::CURSOR_SUPEROBJECT) {
        delete_entity_from_memory((CursorSuperObject*)entity);
    }
    
    /*
    switch (type) {
        case CursorType::CURSOR_DEFAULT:
            break;
        case CursorType::CURSOR_BLOCK:
            delete (CursorBlock*)(entity);
            break;
        case CursorType::CURSOR_MODEL:
            delete (CursorModelObject*)(entity);
            break;
        case CursorType::CURSOR_SUPEROBJECT:
        {
            delete_entity_from_memory((CursorSuperObject*)entity);
            delete (CursorSuperObject*)(entity);
            break;
        }
        case CursorType::CURSOR_WEAPON:
            delete (CursorWeapon*)(entity);
            break;
        case CursorType::CURSOR_SCANNER:
            delete (CursorScanTool*)(entity);
            break;
        case CursorType::CURSOR_DRILL:
            delete (CursorDrill*)(entity);
            break;
        case CursorType::CURSOR_SELECTOR:
            delete (CursorSelector*)(entity);
            break;
        default:
            return;
    }*/
}