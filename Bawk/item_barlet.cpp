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
        entity->cleanup();
    }
}

void ItemBarlet::render_elements() {
    // first, fill with black box
    OGLAttr::current_shader->set_block_draw_mode(BlockDrawMode::COLOR);
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
    else {
        xfill = 0.1f;
        yfill = 0.15f;
        zfill = 0.2f;
    }
    float texture[6][3];
    for (int p = 0; p < 6; p++) {
        texture[p][0] = xfill;
        texture[p][1] = yfill;
        texture[p][2] = zfill;
    }
    
    set_unitary_transform_matrix();
    
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof vertex, vertex, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(OGLAttr::current_shader->coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, OGLAttr::common_texture_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof texture, texture, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(OGLAttr::current_shader->texture_coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    if (entity) {
        entity->render_item();
    }
}

CursorItem* ItemBarlet::get_cursor_item() {
    return entity;
}

void ItemBarlet::set_cursor_item(CursorItem* item) {
    if (entity)
        entity->cleanup();
    entity = item;
}

void ItemBarlet::set_current(bool curr) {
    current = curr;
    if (entity)
        entity->reset();
}