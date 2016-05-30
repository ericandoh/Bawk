//
//  common_accessor.h
//  Bawk
//
//  We'll be running some code on both client + server
//  We'll want that code to be run on both the worlds on client and server
//
//  Created by Eric Oh on 2/18/16.
//  Copyright (c) 2016 Eric Oh. All rights reserved.
//

#ifndef __Bawk__common_accessor__
#define __Bawk__common_accessor__

#include <stdio.h>
#include "world.h"

void set_is_client();
void set_is_server();

World* get_world();

#endif /* defined(__Bawk__common_accessor__) */
