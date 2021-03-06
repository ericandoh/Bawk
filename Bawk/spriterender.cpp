//
//  spriterender.cpp
//  Bawk
//
//  Created by Eric Oh on 12/8/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "spriterender.h"
#include "importopengl.h"
#include "player.h"
#include "worldrender.h"
#include "client_accessor.h"

SpriteMultiplexer* SpriteMultiplexer::copy() { return this; }

SpriteRender::SpriteRender() {
    multiplexer = 0;
    ttl = 0;
    duration = 0;
    light = RenderableLight();
    
    can_rotate = true;
    
    track_player = false;
}

float SpriteRender::get_perc_age() {
    // 0.0f = start, 1.0f = end
    if (duration == 0) {
        return 0.0f;
    }
    return 1.0f - (ttl * 1.0f / duration);
}

void SpriteRender::set_to_face_player(Player* player) {
    // set this sprite's angle to point toward player
    fvec3 dir = pos - player->get_viewpoint();
    dir = glm::normalize(dir);
    Rotation* lookdir = player->get_viewpoint_angle();
    fvec3 up = glm::cross(lookdir->get_right(), dir);
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
    OGLAttr::current_shader->set_transform_matrix(&view);
    
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
    
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof rectangle_vertex_data, rectangle_vertex_data, GL_DYNAMIC_DRAW);
    OGLAttr::current_shader->set_coord_attribute(GL_FLOAT);
    
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_texture_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof texture_binds, &(texture_binds[0]), GL_DYNAMIC_DRAW);
    OGLAttr::current_shader->set_texture_coord_attribute(GL_FLOAT);
    
    glDrawArrays(GL_TRIANGLES, 0, num_triangles);
}

void SpriteRender::step(int ms) {
    if (ttl == 0 && duration > 0) {
        // TODO this sprite should die
        //((SuperObject*)parent)->remove_sprite(...);
        get_client()->sprite_manager.remove_sprite(this);
        return;
    }
    ttl -= 1;
    if (track_player) {
        // set pos to player's position
        set_pos(get_player()->get_viewpoint());
    }
    if (multiplexer) {
        multiplexer->step(ms, this);
    }
}

void SpriteRender::render(fmat4* transform) {
    if (multiplexer) {
        multiplexer->render(transform, this);
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
    if (multiplexer) {
        copy->multiplexer = multiplexer->copy();
    }
    return copy;
}

void SpriteRender::set_track_player(bool val) {
    track_player = val;
}
