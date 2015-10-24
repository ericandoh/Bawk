//
//  item_barlet.cpp
//  Bawk
//
//  Created by Eric Oh on 10/10/15.
//  Copyright (c) 2015 Eric Oh. All rights reserved.
//

#include "item_barlet.h"
#include "base_widget.h"
#include <glm/gtc/matrix_transform.hpp>
#include "worldrender.h"

ItemBarlet::ItemBarlet(int x, int y, int width, int height): BaseWidget(x, y, width, height) {
    entity = 0;
    current = false;
}

ItemBarlet::~ItemBarlet() {
    if (entity) {
        entity->delete_self();
    }
}

void ItemBarlet::render_elements() {
    // first, fill with black box
    set_block_draw_mode(2);
    float vertex[6][3] = {
        {-1, -1, 0},
        {1, -1, 0},
        {-1, 1, 0},
        {-1, 1, 0},
        {1, -1, 0},
        {1, 1, 0},
    };
    
    /*float texture[6][3] = {
        {0, 0, 0},
        {1, 0, 0},
        {0, 1, 0},
        {0, 1, 0},
        {1, 0, 0},
        {1, 1, 0},
    };*/
    float xfill, yfill, zfill;
    xfill = yfill = zfill = 0;
    if (current) {
        xfill = 0.3f;
        yfill = 0.4f;
        zfill = 0.5f;
    }
    float texture[6][3];
    for (int p = 0; p < 6; p++) {
        texture[p][0] = xfill;
        texture[p][1] = yfill;
        texture[p][2] = zfill;
    }
    
    glm::mat4 one(1);
    set_transform_matrix(one);
    
    glBindBuffer(GL_ARRAY_BUFFER, get_vertex_attribute_vbo());
    glBufferData(GL_ARRAY_BUFFER, sizeof vertex, vertex, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(block_attribute_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, get_texture_attribute_vbo());
    glBufferData(GL_ARRAY_BUFFER, sizeof texture, texture, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(texture_attribute_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    if (entity) {
        entity->render_at_zero(&mvp);
    }
}

CursorItem* ItemBarlet::get_cursor_item() {
    return entity;
}

void ItemBarlet::set_cursor_item(CursorItem* item) {
    if (entity)
        entity->delete_self();
    
    entity = item;
    
    if (!entity) {
        return;
    }
    ivec3 upper;
    entity->get_bounds(&upper);
    
    float length = sqrtf(upper.x*upper.x + upper.y*upper.y + upper.z*upper.z) / 1.4f;
    
    fvec3 v1 = fvec3(-upper.x, 0, upper.z);
    fvec3 v2 = fvec3(0, -upper.y, upper.z);
    fvec3 perp = glm::cross(v1, v2);
    // normalize perp
    fvec3 perp_normalized = glm::normalize(perp);
    perp.x = perp_normalized.x * length * 2.0f;
    perp.y = perp_normalized.y * length * 2.0f;
    perp.z = perp_normalized.z * length * 2.0f;
    
    fvec3 upper_pos = fvec3(perp.x*2.0f, perp.y*2.0f, perp.z*2.0f);
    fvec3 toward_pos = fvec3(upper.x/2.0f, upper.y/2.0f, upper.z/2.0f);
    
    fmat4 view = glm::lookAt(upper_pos, toward_pos, fvec3(0, 1, 0));
    //fmat4 projection = glm::perspective(45.0f, 1.0f, 0.01f, 100.0f); //1.0f * width/height
    
    // left, right, bottom, top
    fmat4 projection = glm::ortho(-length, length, -length, length, 0.01f, 100.0f);
    mvp = projection * view;
}

void ItemBarlet::set_current(bool curr) {
    current = curr;
}