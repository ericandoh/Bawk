//
//  entity_loader.h
//  Bawk
//
//  Created by Eric Oh on 10/26/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__entity_loader__
#define __Bawk__entity_loader__

#include <stdio.h>
#include "basic_types.h"
#include "entity_type.h"

class Entity;
class World;
class Player;

Entity* get_entity_from(uint32_t pid, uint32_t vid, EntityType entity_class);
Entity* copy_entity(Player* player, Entity* src);
void delete_entity_from_memory(Entity* entity);


#endif /* defined(__Bawk__entity_loader__) */
