//
//  spriterender.cpp
//  Bawk
//
//  Created by Eric Oh on 12/8/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "spriterender.h"
#include "includeglfw.h"
#include "player.h"
#include "worldrender.h"

SpriteRender::SpriteRender() {
    player = 0;
    steppable = 0;
    renderable = 0;
    ttl = 0;
    duration = 0;
    light = RenderableLight();
    
    can_collide = false;
    can_rotate = true;
}

void SpriteSteppable::step(Game* game, SpriteRender* render) { }
SpriteSteppable* SpriteSteppable::copy() { return this; }
void SpriteRenderable::render(fmat4 *transform, SpriteRender *render) { }
SpriteRenderable* SpriteRenderable::copy() { return this; }

float SpriteRender::get_perc_age() {
    // 0.0f = start, 1.0f = end
    if (duration == 0) {
        return 0.0f;
    }
    return 1.0f - (ttl * 1.0f / duration);
}

void SpriteRender::set_relative_transform() {
    // set this sprite's angle to point toward player
    fvec3 dir = pos - player->get_rwc_pos();
    dir = glm::normalize(dir);
    fvec3 up = glm::cross(player->angle.right, dir);
    up = glm::normalize(up);
    
    Rotation pointing = Rotation();
    pointing.set_to_point(dir, up);
    set_angle(pointing);
}

void SpriteRender::render_rectangle(fmat4* transform, float width, float height, fvec3 texture_corners[4]) {
    
    fmat4 view;
    get_mvp(&view);
    view = *transform * view;
    // add to the transformation matrix a scaling factor for the radius
    view = glm::scale(view, fvec3(0, height, width));
    set_transform_matrix(&view);
    
    int num_triangles = 6;
    
    GLfloat rectangle_vertex_data[6][3] = {
        {0, -1, -1},
        {0, -1, 1},
        {0, 1, -1},
        {0, 1, -1},
        {0, -1, 1},
        {0, 1, 1},
    };
    
    fvec3 texture_binds[6];
    texture_binds[0] = texture_corners[0];
    texture_binds[1] = texture_corners[1];
    texture_binds[2] = texture_corners[2];
    texture_binds[3] = texture_corners[2];
    texture_binds[4] = texture_corners[1];
    texture_binds[5] = texture_corners[3];
    
    glBindBuffer(GL_ARRAY_BUFFER, get_vertex_attribute_vbo());
    glBufferData(GL_ARRAY_BUFFER, sizeof rectangle_vertex_data, rectangle_vertex_data, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(geometry_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, get_texture_attribute_vbo());
    glBufferData(GL_ARRAY_BUFFER, sizeof texture_binds, &(texture_binds[0]), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(geometry_texture_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glDrawArrays(GL_TRIANGLES, 0, num_triangles);
}

void SpriteRender::step(Game* game) {
    if (ttl == 0 && duration > 0) {
        // TODO this sprite should die
        //((SuperObject*)parent)->remove_sprite(...);
        ((SuperObject*)parent)->remove_entity(this);
        return;
    }
    ttl -= 1;
    if (steppable) {
        steppable->step(game, this);
    }
}

void SpriteRender::render(fmat4* transform) {
    if (renderable) {
        renderable->render(transform, this);
    }
}

void SpriteRender::render_lights(fmat4* transform, fvec3 player_pos) {
    if (light.should_render()) {
        light.render_light(transform, pos, player_pos);
    }
}

SpriteRender* SpriteRender::copy() {
    // steppable->copy() (if needs copy, return itself)
    SpriteRender* copy = new SpriteRender();
    copy->duration = duration;
    copy->ttl = duration;
    copy->light = light;
    if (steppable) {
        copy->steppable = steppable->copy();
    }
    if (renderable) {
        copy->renderable = renderable->copy();
    }
    return copy;
}

// position

// size/shape (ie face player?)

// texture/color/how to render/how this changes with time

// duration

// lighting

// count?