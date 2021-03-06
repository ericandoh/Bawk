//
//  entity_type.h
//  Bawk
//
//  Created by Eric Oh on 11/29/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__entity_type__
#define __Bawk__entity_type__

#include <stdio.h>

enum EntityType {
    ENTITY,
    BASEWORLD,
    PLAYER,
    SUPEROBJECT,
    CURSORSUPEROBJECT,
    GAMETEMPLATE,
    MODELENTITY,
    CURSORMODELENTITY
};

void print_entity_type(EntityType type);
#endif /* defined(__Bawk__entity_type__) */
