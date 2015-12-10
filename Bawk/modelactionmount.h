//
//  blockactionmount.h
//  Bawk
//
//  Created by Eric Oh on 12/9/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__modelactionmount__
#define __Bawk__modelactionmount__

#include <stdio.h>
#include "blockaction.h"

bool model_mount_callback(Game* game, Entity* owner, Entity* piece);
bool model_unmount_callback(Game* game, Entity* owner, Entity* piece);

#endif /* defined(__Bawk__modelactionmount__) */
