//
//  Header file for global constants/file names/strings/etc
//  -
//  -
//  -
//
//  Used by:
//  -
//
//  Uses:
//  -
//  -
//  -
//
//  Notes
//  -
//  -
//
//  Created by Eric Oh on 9/28/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef Bawk_settings_h
#define Bawk_settings_h

#include <string>

// dimensions of our chunk
#define CX 16
#define CY 16
#define CZ 16

#define MAX_HEALTH 1024

const unsigned int TILES_PER_TEXTURE = 16;

const int CHUNK_RENDER_DIST = 7;

const int BREAKING_STAGES = 3;

const std::string EXAMPLE_CONSTANT = "this is a sample constant!";

#endif
