//
//  entity_type.cpp
//  Bawk
//
//  Created by Eric Oh on 12/2/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include <stdio.h>
#include "entity_type.h"

void print_entity_type(EntityType type) {
    if (type == EntityType::ENTITY) {
        printf("entity");
    }
    else if (type == EntityType::BASEWORLD) {
        printf("baseworld");
    }
    else if (type == EntityType::PLAYER) {
        printf("player");
    }
    else if (type == EntityType::SUPEROBJECT) {
        printf("superobject");
    }
    else if (type == EntityType::CURSORSUPEROBJECT) {
        printf("cursorsuperobject");
    }
    else if (type == EntityType::GAMETEMPLATE) {
        printf("gametemplate");
    }
    else if (type == EntityType::MODELENTITY) {
        printf("modelentity");
    }
    else if (type == EntityType::CURSORMODELENTITY) {
        printf("cursormodelentity");
    }
    else {
        printf("unknown");
    }
}