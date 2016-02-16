//
//  texture_loader.cpp
//  Bawk
//
//  Created by Eric Oh on 10/2/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "texture_loader.h"
#include <string>
#include "importsdl.h"
#include "SOIL.h"
#include "texture_allocator.h"

namespace Textures {
    // the texture data themselves in OpenGL
    GLuint all_textures[TextureName::TOTAL_TEXTURES];
    // active texture unit used by each texture
    // 0 may be shared by other textures
    // 1 and above are UNIQUE - so binding that will give us our texture immediately
    GLuint all_active_textures[TextureName::TOTAL_TEXTURES];
    const char* texture_names[TextureName::TOTAL_TEXTURES];
    // the current texture data bound to texture unit 0
    TextureName current_texture_for_default;
    int default_active_texture_unit;
}

using namespace Textures;

GLuint load_texture(const char* val);

GLuint get_image_id(Textures::TextureName resource) {
    return all_textures[resource];
}

int get_texture_unit_id(Textures::TextureName resource) {
    return all_active_textures[resource];
}

void bind_image_id(Textures::TextureName resource) {
    glBindTexture(GL_TEXTURE_2D, get_image_id(resource));
}

void bind_to_active_texture(int active_texture_unit, TextureName resource) {
    if (default_active_texture_unit == active_texture_unit) {
        current_texture_for_default = resource;
    }
    set_active_texture(active_texture_unit);
    bind_image_id(resource);
    all_active_textures[resource] = active_texture_unit;
}

void load_textures() {
    // this invalidates current texture
    current_texture_for_default = TextureName::TOTAL_TEXTURES;
    
    all_textures[TextureName::TILES] = load_texture("tiles.png");
    all_textures[TextureName::FONTS] = load_texture("fonts.png");
    all_textures[TextureName::SKYBOX] = load_texture("skybox.png");
    
    texture_names[TextureName::TILES] = "tiles";
    texture_names[TextureName::FONTS] = "fonts";
    texture_names[TextureName::SKYBOX] = "skybox";
    
    default_active_texture_unit = reserve_n_active_textures(3);
    bind_to_active_texture(default_active_texture_unit, TextureName::TILES);
    bind_to_active_texture(default_active_texture_unit + 1, TextureName::FONTS);
    bind_to_active_texture(default_active_texture_unit + 2, TextureName::SKYBOX);
    
    // NOTE: for uncommonly used textures, the below is actually not even necessary (and probably frivious)
    // bind_to_active_texture(default_active_texture, TextureName::UNCOMMONLY_USED_TEXTURE);
    // bind_to_active_texture(default_active_texture + n, TextureName::COMMONLY_USED_TEXTURE);
}

GLuint load_texture(const char* val) {
    
    // load an image file directly as a new OpenGL texture
    GLuint texture_id = SOIL_load_OGL_texture(val,
                                              SOIL_LOAD_AUTO,
                                              SOIL_CREATE_NEW_ID,
                                              //SOIL_FLAG_MIPMAPS |
                                              SOIL_FLAG_TEXTURE_REPEATS |
                                              SOIL_FLAG_INVERT_Y |
                                              SOIL_FLAG_NTSC_SAFE_RGB |
                                              SOIL_FLAG_COMPRESS_TO_DXT);
    // check for an error during the load process
    if( 0 == texture_id )
    {
        printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
    }
    return texture_id;
}

Textures::TextureName get_texturename_from_string(std::string name) {
    if (name.compare("") == 0) {
        return TextureName::TILES;
    }
    for (int i = 0; i < TextureName::TOTAL_TEXTURES; i++) {
        if (name.compare(texture_names[i]) == 0) {
            return static_cast<TextureName>(i);
        }
    }
    return TextureName::TOTAL_TEXTURES;
}

int bind_and_get_active_texture_unit(Textures::TextureName resource) {
    int active_texture_unit = get_texture_unit_id(resource);
    
    if (active_texture_unit == default_active_texture_unit) {
        if (current_texture_for_default == resource) {
            // the default texture unit is already set to our texture, we're good to go!
            return active_texture_unit;
        }
        set_active_texture(default_active_texture_unit);
        bind_image_id(resource);
    }
    return active_texture_unit;
}

void save_tile() {
    // TODO re-enable taking screenshot!
    printf("Saving screenies disabled for now\n");
    //int width, height;
    //get_window_size(&width, &height);
    //int result = SOIL_save_screenshot("/Users/Eric/haha.png", SOIL_SAVE_TYPE_BMP, 0, 0, width, height);
    //printf("%d\n", result);
}

void cleanup_textures() {
    for (int i = 0; i < TextureName::TOTAL_TEXTURES; i++) {
        GLuint image_id = get_image_id(static_cast<TextureName>(i));
        glDeleteTextures(1, &image_id);
    }
}