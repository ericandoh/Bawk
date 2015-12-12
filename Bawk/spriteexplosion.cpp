//
//  spriteexplosion.cpp
//  Bawk
//
//  Created by Eric Oh on 12/11/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "spriteexplosion.h"
#include "settings.h"
#include "worldrender.h"

class SpriteExplosiveRenderable: public SpriteRenderable {
public:
    int texture_id;
    float start_radius, end_radius;
    
    void render(fmat4* transform, SpriteRender* render) override;
    
    SpriteRenderable* copy() override;
};

void SpriteExplosiveRenderable::render(fmat4 *transform, SpriteRender *render) {
    // assume radius gets larger...or does it
    float radius = start_radius + (end_radius - start_radius) * render->get_perc_age();
    fvec3 textures[4];
    
    float width = 0.99f / TILES_PER_TEXTURE;
    float texture_x = (texture_id % TILES_PER_TEXTURE + 0.005f) / TILES_PER_TEXTURE;
    float texture_y = 1.0f - (texture_id / TILES_PER_TEXTURE + 0.005f) * 1.0f / TILES_PER_TEXTURE;
    //fvec3 texture(texture_id % TILES_PER_TEXTURE, texture_id / TILES_PER_TEXTURE, 0);
    textures[0] = fvec3(texture_x, texture_y, 0.0f);
    textures[2] = fvec3(texture_x, texture_y - width, 0.0f);
    textures[1] = fvec3(texture_x + width, texture_y, 0.0f);
    textures[3] = fvec3(texture_x + width, texture_y - width, 0.0f);
    
    /*for (int i = 0; i < 4; i++) {
        textures[i] = fvec3(0.3f,0.4f,0.5f);
    }*/
    
    // drawing from block texture
    set_block_draw_mode(0);
    // bind a rectangle pointing in direction of player, at pos
    render->render_rectangle(transform, radius, textures);
}

SpriteRenderable* SpriteExplosiveRenderable::copy() {
    // allow copy, since we could have explosions of different radii
    SpriteExplosiveRenderable* copy = new SpriteExplosiveRenderable();
    copy->texture_id = texture_id;
    copy->start_radius = start_radius;
    copy->end_radius = end_radius;
    return copy;
}

SpriteRenderable* get_sprite_explosive(int texture_id, float start_radius, float end_radius) {
    SpriteExplosiveRenderable* sprite_render = new SpriteExplosiveRenderable();
    sprite_render->texture_id = texture_id;
    sprite_render->start_radius = start_radius;
    sprite_render->end_radius = end_radius;
    return sprite_render;
}
