//
//  Loads textures for the game and binds them to OpenGL
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
//  http://www.lonesock.net/soil.html
//
//  Created by Eric Oh on 10/2/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#ifndef __Bawk__texture_loader__
#define __Bawk__texture_loader__

#include <stdio.h>
#include <string>
#include "importopengl.h"

namespace Textures {
    enum TextureName {
        TILES, FONTS, SKYBOX,
        TOTAL_TEXTURES
    };
}

void load_textures();
Textures::TextureName get_texturename_from_string(std::string name);
int bind_and_get_active_texture_unit(Textures::TextureName resource);
void save_tile();
void cleanup_textures();

#endif /* defined(__Bawk__texture_loader__) */
