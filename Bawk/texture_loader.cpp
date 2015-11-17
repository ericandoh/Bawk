//
//  texture_loader.cpp
//  Bawk
//
//  Created by Eric Oh on 10/2/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "texture_loader.h"
#include <string>
#include "SOIL.h"
#include "display.h"

const char* TILE_IMAGE = "tiles.png";

GLuint load_tiles() {
    /* load an image file directly as a new OpenGL texture */
    GLuint tiles = SOIL_load_OGL_texture(TILE_IMAGE,
                                         SOIL_LOAD_AUTO,
                                         SOIL_CREATE_NEW_ID,
                                         //SOIL_FLAG_MIPMAPS |
                                         SOIL_FLAG_TEXTURE_REPEATS |
                                         SOIL_FLAG_INVERT_Y |
                                         SOIL_FLAG_NTSC_SAFE_RGB |
                                         SOIL_FLAG_COMPRESS_TO_DXT);
    /* check for an error during the load process */
    if( 0 == tiles )
    {
        printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
    }
    return tiles;
}

void save_tile() {
    int width, height;
    get_window_size(&width, &height);
    int result = SOIL_save_screenshot("/Users/Eric/haha.png", SOIL_SAVE_TYPE_BMP, 0, 0, width, height);
    printf("%d\n", result);
}