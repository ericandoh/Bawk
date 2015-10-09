//
//  block.h
//  Bawk
//
//  Created by Eric Oh on 10/2/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__block__
#define __Bawk__block__

#include <stdio.h>
#include <stdint.h>

// gets the transparency of a block. 0 is opaque
int get_transparency(uint16_t block);

// if a block is independent, any template made with it will become an independent superobject
int creates_independence(uint16_t block);

#endif /* defined(__Bawk__block__) */
